/*
========================

Copyright (c) 2014 Vinyl Games Studio

	NetworkServer.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 03/14/2014

========================
*/
#include "NetworkServer.h"
#include "Player.h"

#include <NetworkMessageIDs.h>
#include <NetConsts.h>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include <iostream>

using namespace vgs;

NetworkServer::NetworkServer()
	: m_peer(RakNet::RakPeerInterface::GetInstance())
{

}

NetworkServer::~NetworkServer()
{
	if (m_peer != NULL)
	{
		RakNet::RakPeerInterface::DestroyInstance(m_peer);
	}
}

// returns true if the server is started
bool NetworkServer::Start(unsigned short port, unsigned int maxConnections)
{
	if (m_peer != NULL)
	{
		RakNet::SocketDescriptor sd(port, NULL);
		sd.socketFamily = AF_INET;
		RakNet::StartupResult result = m_peer->Startup(maxConnections, &sd, 1);

		switch (result)
		{
		case RakNet::RAKNET_STARTED:
			std::cout << "Server started" << std::endl;

			m_peer->SetMaximumIncomingConnections(maxConnections);
			m_peer->SetOccasionalPing(true);
			m_peer->SetTimeoutTime(g_serverNetInfo.timeoutTime, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
			return true;

		case RakNet::RAKNET_ALREADY_STARTED:
			std::cout << "Server was already started" << std::endl;
			return true;

		case RakNet::SOCKET_PORT_ALREADY_IN_USE:
			std::cerr << "Server could't bind to port " << port
				<< "because its already in use" << std::endl;
			return false;

		default:
			std::cerr << "Unknown potentially bad Startup result" << std::endl;
			return false;
		}
	}

	std::cerr << "Sever has a null m_peer" << std::endl;
	return false;
}

void NetworkServer::Shutdown(unsigned int blockDuration)
{
	if (m_peer != NULL)
	{
		std::cout << "Server shutting down with blockduration of " << blockDuration << std::endl;
		m_peer->Shutdown(blockDuration, 0, HIGH_PRIORITY);
	}
}

bool NetworkServer::IsActive()
{
	return m_peer != NULL ? m_peer->IsActive() : false;
}

RakNet::Packet* NetworkServer::Receive()
{
	RakNet::Packet* packet = m_peer->Receive();
	
	while (packet != NULL)
	{
		unsigned char pid = packet->data[0];

		switch (pid)
		{
		case ID_UNCONNECTED_PING:
			break;

		case ID_NEW_INCOMING_CONNECTION:
			std::cout << "Incoming connection from " << packet->systemAddress.ToString() << std::endl;
			break;

		case ID_DISCONNECTION_NOTIFICATION:
			std::cout << "Disconnection notification from " << packet->systemAddress.ToString() << std::endl;
			return packet;

		case ID_CONNECTION_LOST:
            std::cout << "Lost connection to " << packet->systemAddress.ToString() << std::endl;
            return packet;

		case ID_TIMESTAMP:
			return packet;

		default:
			// engine and game packets
			if (pid >= (unsigned char)ToneArmMessage::ID_ENGINE_SHUTDOWN && pid < (unsigned char)MerryMessage::ID_MERRY_LAST_PACKET_ENUM)
			{
				return packet;
			}
			else
			{
				std::cout << "Unhanled packet in Receive with pid " << (int)pid << " from " << packet->systemAddress.ToString() << std::endl;
			}
			break;
		}

		m_peer->DeallocatePacket(packet);
		packet = m_peer->Receive();
	}

	return NULL;
}

void NetworkServer::Send(char* data, int len, const RakNet::AddressOrGUID& systemIdentifier, bool broadcast) const
{
	m_peer->Send(data, len, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemIdentifier, broadcast);
}

void NetworkServer::Send(RakNet::BitStream& bs, const RakNet::AddressOrGUID& systemIdentifier, bool broadcast) const
{
	m_peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemIdentifier, broadcast);
}

void NetworkServer::Broadcast(char* data, int len) const
{
	Send(data, len, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void NetworkServer::Broadcast(RakNet::BitStream& bs) const
{
	Send(bs, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void NetworkServer::Multicast(char* data, int len, const PlayerSet& players) const
{
	for (const auto player : players)
	{
		Send(data, len, player->GetClientAddress(), false);
	}
}

void NetworkServer::Multicast(RakNet::BitStream& bs, const PlayerSet& players) const
{
	for (const auto player : players)
	{
		Send(bs, player->GetClientAddress(), false);
	}
}

void NetworkServer::DeallocatePacket(RakNet::Packet* packet) const
{
	m_peer->DeallocatePacket(packet);
}

void NetworkServer::SetPingResponse(const char* data, const unsigned int length)
{
	m_peer->SetOfflinePingResponse(data, length);
}

void NetworkServer::CloseConnection(const RakNet::AddressOrGUID& systemIdentifier)
{
	m_peer->CloseConnection(systemIdentifier, false);
}