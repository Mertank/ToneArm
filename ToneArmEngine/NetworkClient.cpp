/*
========================

Copyright (c) 2014 Vinyl Games Studio

	NetworkClient.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 03/14/2014

========================
*/
#include "NetworkClient.h"
#include "NetworkMessageIDs.h"
#include "Log.h"
#include "InitializationFileResource.h"
#include "Engine.h"
#include "CachedResourceLoader.h"
#include "FileManager.h"

#include <NetConsts.h>

#include <RakPeerInterface.h>
#include <RakSleep.h>
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <BitStream.h>

#include <iostream>
#include <sstream>

using namespace vgs;

NetworkClient::NetworkClient()
	: m_peer(RakNet::RakPeerInterface::GetInstance())
	, m_serverAddress(new RakNet::SystemAddress())
	, m_serverGUID(new RakNet::RakNetGUID())
{

}

NetworkClient::~NetworkClient()
{
	if (m_peer != NULL)
	{
		RakNet::RakPeerInterface::DestroyInstance(m_peer);
	}

	delete m_serverAddress;
	delete m_serverGUID;
}

// returns true if the server is started
bool NetworkClient::Start(unsigned short& port)
{
	if (m_peer != NULL)
	{
		RakNet::SocketDescriptor sd(port, NULL);
		sd.socketFamily = AF_INET;
		RakNet::StartupResult result = m_peer->Startup(1, &sd, 1);

		switch (result)
		{
		case RakNet::RAKNET_STARTED:
			Log::GetInstance()->WriteToLog("Network", "Client started");
			
			m_peer->SetTimeoutTime(g_clientNetInfo.timeoutTime, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
			m_peer->SetOccasionalPing(true);
			
			g_clientNetInfo.actualPort = port;
			
			return true;

		case RakNet::RAKNET_ALREADY_STARTED:
			Log::GetInstance()->WriteToLog("Network", "Client was already started");
			g_clientNetInfo.actualPort = port;
			return true;

		case RakNet::SOCKET_PORT_ALREADY_IN_USE:
			{
				std::stringstream ss;
				ss << "Socket port " << port << " is already in use. Trying " << port + 1 << "...";
				Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
				Start(++port);
			}
			return true;

		default:
			Log::GetInstance()->WriteToLog("Network", "Unknown potentially bad startup result in NetworkClient::Start");
			return false;
		}
	}

	std::cerr << "Client has a null m_peer" << std::endl;
	return false;
}

void NetworkClient::Shutdown(bool sendNotifications)
{
	if (m_peer != NULL)
	{
		Log::GetInstance()->WriteToLog("Network", "Client shutting down");
		m_peer->Shutdown(sendNotifications ? 0 : 300, 0, LOW_PRIORITY);
	}
}

bool NetworkClient::IsActive()
{
	return m_peer != NULL ? m_peer->IsActive() : false;
}

bool NetworkClient::Connect(unsigned short port, const char* ip)
{
	if (m_peer == NULL)
	{
		Log::GetInstance()->WriteToLog("Network", "m_peer is null");
		return false;
	}
	else if (!m_peer->IsActive())
	{
		Log::GetInstance()->WriteToLog("Network", "Client has not started yet. Call start before you call connect");
		return false;
	}

	RakNet::ConnectionAttemptResult result = m_peer->Connect(ip, port, NULL, 0);

	std::stringstream ss;

	switch (result)
	{
	case RakNet::CONNECTION_ATTEMPT_STARTED:
	case RakNet::CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS:
		Log::GetInstance()->WriteToLog("Network", "Relax... Trying to connect...");
		break;

	case RakNet::ALREADY_CONNECTED_TO_ENDPOINT:
		ss << "You have already succesfully connected to " << ip << ":" << port;
		Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
		return true;

	default:
		ss << "Attempt to connect returned code " << result << " which is currently unhandled";
		Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
		return false;
	}

	bool done = false;
	while (!done)
	{
		RakNet::Packet* packet = m_peer->Receive();
		if (packet != NULL)
		{
			// handle the packet
			unsigned char pid = packet->data[0];

			switch (pid)
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				ss << "Connection succesful to " << ip << ":" << port;
				Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
				ss.str(std::string());

				*m_serverAddress	= packet->systemAddress;
				*m_serverGUID		= packet->guid;
				done				= true;
				break;

			case ID_CONNECTION_ATTEMPT_FAILED:
				ss << "Connection not succesful to " << ip << ":" << port;
				Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
				ss.str(std::string());
				done = true;
				break;

			case ID_ALREADY_CONNECTED:
				ss << "Already connected to " << ip << ":" << port;
				Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
				ss.str(std::string());
				done = true;
				break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
				ss << "Server on " << ip << ":" << port << " doesn't have available connections";
				Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
				ss.str(std::string());
				done = true;
				break;

			default:
				ss << "Unhandled message in NetworkClient::Connect() " << ip << ":" << port << " with pid " << (unsigned)pid;
				Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
				ss.str(std::string());
				break;
			}

			// clean up
			m_peer->DeallocatePacket(packet);
		}
		RakSleep(50);
	}

	return IsConnected();
}

void NetworkClient::Disconnect(bool sendNotification)
{
	if (m_peer != NULL)
	{
		m_peer->CloseConnection(*m_serverGUID, sendNotification, 0, HIGH_PRIORITY);
	}
}

bool NetworkClient::IsConnected()
{
	if (m_peer != NULL && m_serverGUID != NULL)
	{
		return m_peer->GetConnectionState(*m_serverGUID) == RakNet::ConnectionState::IS_CONNECTED;
	}
	return false;
}

RakNet::Packet* NetworkClient::Receive()
{
	RakNet::Packet* packet = m_peer->Receive();
	
	while (packet != NULL)
	{
		unsigned char pid = packet->data[0];

		switch (pid)
		{
		case ID_TIMESTAMP:
		case ID_UNCONNECTED_PONG:
			return packet;

		case ID_CONNECTION_LOST:
			{
				std::stringstream ss;
				ss << "Connection was lost to " << packet->systemAddress.ToString();
				Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
			}
			break;

		case ID_DISCONNECTION_NOTIFICATION:
			{
				std::stringstream ss;
				ss << "Received disconnection notification from" << packet->systemAddress.ToString();
				Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
			}
			break;

		default:
			// engine and game packets
			if (pid >= (unsigned char)ToneArmMessage::ID_ENGINE_SHUTDOWN && pid < (unsigned char)MerryMessage::ID_MERRY_LAST_PACKET_ENUM)
			{
				return packet;
			}
			else
			{
				std::stringstream ss;
				ss << "Unhanled packet in NetworkClient::Receive() with pid " << (unsigned)pid << " from " << packet->systemAddress.ToString();
				Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
			}
			break;
		}

		m_peer->DeallocatePacket(packet);
		packet = m_peer->Receive();
	}

	return NULL;
}

void NetworkClient::Send(const RakNet::BitStream& bs)
{
	m_peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, *m_serverAddress, false);
}

void NetworkClient::Send(char* data, int len)
{
	m_peer->Send(data, len, HIGH_PRIORITY, RELIABLE_ORDERED, 0, *m_serverAddress, false);
}

void NetworkClient::DeallocatePacket(RakNet::Packet* packet)
{
	m_peer->DeallocatePacket(packet);
}

void NetworkClient::PingAddress(RakNet::SystemAddress* ad)
{
	m_peer->Ping(ad->ToString(false), ad->GetPort(), false);
}

void NetworkClient::BroadcastPing(unsigned int port)
{
	m_peer->Ping("255.255.255.255", port, false);
}