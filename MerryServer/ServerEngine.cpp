/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ServerEngine.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 5/22/2014 1:57:27 PM

========================
*/
#include "ServerEngine.h"
#include "GameMode.h"
#include "GameWorld.h"

#include "../ToneArmEngine/PhysicsModule.h"

#include <NetworkMessageIDs.h>

#include <RakNetTypes.h>
#include <BitStream.h>
#include <RakPeer.h>
#include <RakSleep.h>

#include <time.h>
#include <iostream>

using namespace vgs;

ServerEngine* ServerEngine::s_instance = nullptr;

ServerEngine* ServerEngine::GetInstance()
{
	if (!s_instance)
	{
		s_instance = new ServerEngine();
	}
	return s_instance;
}

void ServerEngine::FreeInstance()
{
	delete s_instance;
	s_instance = nullptr;
}

ServerEngine::ServerEngine()
	: m_gw(nullptr)
{
}

ServerEngine::~ServerEngine()
{
	delete m_gw;
	//delete m_resourceLoader;
}

bool ServerEngine::Startup(GameWorld* gw)
{
	if (!gw)
	{
		std::cout << "Null gw" << std::endl;
		return false;
	}

	srand((unsigned int)time(0));

	// create engine modules
	m_modules[EngineModuleType::RESOURCELOADER]	= CachedResourceLoader::Create();
	m_modules[EngineModuleType::PHYSICS]		= PhysicsModule::Create();

	// keep ref
	m_gw = gw;

	// to send player amounts in respond msgs
	UpdatePingResponse();

	bool gwresult	= m_gw->Init();
	bool netresult	= m_netInterface.Start(g_serverNetInfo.defaultPort, gw->GetMaxPlayers());

	return gwresult && netresult;
}

void ServerEngine::Shutdown()
{
	m_netInterface.Shutdown(300);

	// shutdown for modules
	ModuleMap::iterator iter = m_modules.begin();

	while (iter != m_modules.end()) {

		if (iter->second) {
			iter->second->Shutdown();
		}

		++iter;
	}
}

void ServerEngine::Run()
{
	std::cout << "Update rate: " << g_serverNetInfo.ticksPerSecond << std::endl;
	std::cout << "Fixed delta time: " << g_serverNetInfo.deltaTime << std::endl;

	m_tick.dt		= g_serverNetInfo.deltaTime;
	double overflow = 0;
	bool running	= true;

	while (running)
	{
		// update global timing parameters
        ++m_tick.frameno;
		m_tick.t = m_tick.frameno * g_serverNetInfo.deltaTime;

		// convert ms to s
		double frameStart = 0.001 * RakNet::GetTime();   

		// update modules
		for (ModuleMap::iterator iter = m_modules.begin(); iter != m_modules.end(); ++iter) 
		{
			if (iter->second)
			{
				iter->second->Update(m_tick.dt);
			}
		}

		ProcessMessages();
		m_gw->Update(m_tick.dt);

		// wait for the rest of the frame time to elapse
        double frameTime;
        while (true)
		{
            // figure out if we need to sleep for a bit
            double currTime = 0.001 * RakNet::GetTime();
            frameTime = overflow + currTime - frameStart;
            
			if (frameTime >= g_serverNetInfo.deltaTime)
			{
                break;
			}

            // chill for a bit
            RakSleep(1);
        }

        // correct for overshooting (if waited too long)
        overflow = frameTime - g_serverNetInfo.deltaTime;

		// FIXME: sdn't be win only
		// CTRL + X
		if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState((0x58)) < 0 && GetConsoleWindow() == GetForegroundWindow())
		{
			running = false;
		}
	}
}

void ServerEngine::ProcessMessages()
{
	RakNet::Packet* packet = m_netInterface.Receive();
	while (packet != nullptr)
	{
		unsigned char pid = GetPacketIdentifier(packet);

		// proceed if its a system msg
		// pass to gamemode if its game specific
		if (pid >= (unsigned char)ToneArmMessage::ID_ENGINE_LAST_PACKET_ENUM)
		{
			m_gw->ProcessMessage(pid, packet);
			
			m_netInterface.DeallocatePacket(packet);
			packet = m_netInterface.Receive();
			continue;
		}

		switch (pid)
		{
		case ToneArmMessage::ID_ENGINE_JOIN_REQUEST:
			{
				std::cout << "Join request from " << packet->systemAddress.ToString() << std::endl;

				if (m_gw->HasRoom())
				{
					std::cout << "Request accepted" << std::endl;

					// send back accepted message
					char data;
					data = (char)ToneArmMessage::ID_ENGINE_JOIN_REQUEST_ACCEPTED; // other information needed
					m_netInterface.Send(&data, sizeof(char), packet->systemAddress, false);
				}
				else
				{
					// send back join_rejected msg
					char data[2];
					data[0] = (char)ToneArmMessage::ID_ENGINE_JOIN_REQUEST_REJECTED; // other information needed
					data[1] = (char)RejectReason::ID_REJECT_SERVER_FULL;
					m_netInterface.Send(data, 2, packet->systemAddress, false);

					std::cout << "Request rejected with code :" << (unsigned)data[1] << std::endl;
				}
			}
			break;

		// not in use
		case ToneArmMessage::ID_ENGINE_FORCE_DISCONNECT_REQUEST:
			{
				unsigned char uid = m_gw->RemovePlayer(packet->systemAddress);

				if (uid > 0)
				{
					CloseConnectionAndNotify(uid, packet->systemAddress);
				}
			}
			break;

		case ToneArmMessage::ID_ENGINE_CLIENT_DISCONNECTED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);

				unsigned char uid;

				bsIn.IgnoreBytes(1);
				bsIn.Read(uid);

				m_gw->RemovePlayer(uid);

				CloseConnectionAndNotify(uid, packet->systemAddress);
			}
			break;

		case ID_DISCONNECTION_NOTIFICATION:
		case ID_CONNECTION_LOST:
			{
				unsigned char uid = m_gw->RemovePlayer(packet->systemAddress);

				if (uid > 0)
				{
					CloseConnectionAndNotify(uid, packet->systemAddress);
				}
			}
			break;

		default:
			break;
		}

		m_netInterface.DeallocatePacket(packet);
		packet = m_netInterface.Receive();
	}
}

void ServerEngine::UpdatePingResponse()
{
	// set up ping response
	// which includes current and max player amounts
	char response[2];
	char now = m_gw->GetNowPlayers();
	char max = m_gw->GetMaxPlayers();

	memcpy(response, &now, 1);
	memcpy(response + 1, &max, 1);

	m_netInterface.SetPingResponse(response, 2);
}

unsigned char ServerEngine::GetPacketIdentifier(RakNet::Packet *p)
{
	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else
	{
		return (unsigned char)p->data[0];
	}
}

unsigned char ServerEngine::SearchForUIDByClientAddress(RakNet::SystemAddress& address)
{
	for (auto pair : m_addressMap)
	{
		if (pair.second == address)
		{
			return pair.first;
		}
	}
	// zero means its no one's uid
	return 0;
}

void ServerEngine::CloseConnectionAndNotify(unsigned char uid, RakNet::SystemAddress& address)
{
	char data[2];

	data[0] = (unsigned char)ToneArmMessage::ID_ENGINE_CLIENT_DISCONNECTED;
	data[1] = uid;

	m_netInterface.Broadcast(data, 2);
	m_netInterface.CloseConnection(address);

	UpdatePingResponse();
}