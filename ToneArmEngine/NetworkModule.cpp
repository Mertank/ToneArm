/*
========================

Copyright (c) 2014 Vinyl Games Studio

	NetworkModule.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 5/14/2014 5:38:57 PM

========================
*/
#include "NetworkModule.h"
#include "ProcessUtils.h"
#include "Log.h"
#include "MessageHandler.h"
#include "NetworkMessageIDs.h"
#include "Engine.h"
#include "OpenGLRenderModule.h"
#include "GUIManager.h"
#include "LANMenu.h"
#include "VGSAssert.h"
#include "NetworkNode.h"
#include "UICommand.h"
#include "DebugGUILayer.h"
#include "OpenGLGUIPass.h"
#include "GUIManager.h"

#include <RakNetTypes.h>
#include <RakSleep.h>
#include <BitStream.h>
#include <GetTime.h>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace vgs;

INITIALIZE_DEBUG_COMMAND_CALLBACK(vgs::NetworkModule, network)

bool	NetworkModule::s_enableInterpolation	= true;
float	NetworkModule::s_clientDelay			= 0.1f;

NetworkModule::NetworkModule()
	: m_serverStarted(false)
	, m_serverAddress("N/A")
	, m_serverPort(0)
	, m_state(ClientState::SINGLE)
	, m_msgHandler(nullptr)
	, m_pingRate(1)
	, m_pingCounter(0)
{
}

NetworkModule::~NetworkModule()
{
	delete m_msgHandler;
}

NetworkModule* NetworkModule::Create()
{
	return new NetworkModule();
}

void NetworkModule::SetMsgHandler(MessageHandler* mh)
{
	m_msgHandler = mh;
	mh->SetModule(this);
}

void NetworkModule::Startup(void)
{
	// save pointer to the LAN gui layer
	//m_lanMenu = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager()->GetLayer<LANMenu>("lan_menu");
	//VGSAssert(m_lanMenu != nullptr, "Layer not found");

	StartupNetwork();
}

void NetworkModule::Update(float dt)
{
	// process commands from rendering thread, if there is any
	if (UICommand::SizeNet() > 0)
	{
		ProcessUICommands();
	}

	// if network is up and client has not connected to a server
	// send pings to LAN each second
	if (m_interface.IsActive() && !m_interface.IsConnected())
	{
		if (m_pingCounter > m_pingRate)
		{
			m_interface.BroadcastPing(g_serverNetInfo.defaultPort);
			m_pingCounter = 0;
		}
		m_pingCounter += dt;
	}

	RakNet::Packet* packet = m_interface.Receive();
	while (packet != nullptr)
	{
		unsigned char pid = GetPacketIdentifier(packet);

		// proceed if its a system msg
		// pass to msg handler if its game specific
		if ((unsigned)pid >= (unsigned)ToneArmMessage::ID_ENGINE_LAST_PACKET_ENUM)
		{
			m_msgHandler->ProcessMessage(pid, packet);

			m_interface.DeallocatePacket(packet);
			packet = m_interface.Receive();
			continue;
		}

		switch (pid)
		{
		case ID_UNCONNECTED_PONG:
			{
				m_lanMenu = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager()->GetLayer<LANMenu>("lan_menu");
				if (m_lanMenu)
				{
					RakNet::TimeMS		current = RakNet::GetTimeMS();
					RakNet::TimeMS		time;
					RakNet::BitStream	bsIn(packet->data, packet->length, false);
				
					unsigned char		nowPlayer;
					unsigned char		maxPlayers;

					bsIn.IgnoreBytes(1);
					bsIn.Read(time);
					bsIn.Read(nowPlayer);
					bsIn.Read(maxPlayers);
				
					short ping = current - time - Engine::GetInstance()->GetTick().st;
					ping = ping < 0 ? 0 : ping;
					m_lanMenu->SendUpdateMsg(UpdateServerMsg(packet->systemAddress, maxPlayers, nowPlayer, ping));
				}
			}
			break;

		case ToneArmMessage::ID_ENGINE_JOIN_REQUEST_ACCEPTED:
			{
				Log::GetInstance()->WriteToLog("Network", "Server accepted this client. Sending setup...");
				
				// open character selection dialog before sending setup request
				m_msgHandler->OpenPregameSetupWin();

			}
			break;

		case ToneArmMessage::ID_ENGINE_JOIN_REQUEST_REJECTED:
			{
				std::cout << "Server rejected this client" << std::endl;
				std::cout << "Disconnecting..." << std::endl;

				RakNet::BitStream bsIn(packet->data, packet->length, false);
				
				unsigned char reasonCode;

				bsIn.IgnoreBytes(1);
				bsIn.Read(reasonCode);

				std::stringstream ss;
				ss << "Server rejected this client. Code: " << (unsigned)reasonCode << ". Disconnecting...";
				Log::GetInstance()->WriteToLog("Network", ss.str().c_str());

				// dont need to tell server, it already knows
				m_interface.Disconnect(false);
			}
			break;

		case ToneArmMessage::ID_ENGINE_CLIENT_DISCONNECTED:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);

				unsigned char uid;
				bsIn.IgnoreBytes(1);
				bsIn.Read(uid);

				m_msgHandler->ProcessPlayerDisconnection(uid);
			}
			break;

		default:
			break;
		}

		m_interface.DeallocatePacket(packet);
		packet = m_interface.Receive();
	}
}

void NetworkModule::Shutdown(void)
{
	// TODO: handle the situation when hosting player quits
	switch (m_state)
	{
	case vgs::NetworkModule::ClientState::HOST:

		break;
	case vgs::NetworkModule::ClientState::REMOTE:

		break;
	case vgs::NetworkModule::ClientState::SINGLE:

		break;
	default:
		break;
	}

	ShutdownNetwork();
}

bool NetworkModule::StartupNetwork()
{
	if (!m_interface.IsActive())
	{
		unsigned short actualPort = g_clientNetInfo.defaultPort; 
		if (m_interface.Start(actualPort))
		{
			std::stringstream ss;
			ss << "Client now uses " << actualPort << " port";
			Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void NetworkModule::ShutdownNetwork()
{
	if (m_interface.IsActive())
	{
		if (m_interface.IsConnected())
		{
			//m_msgHandler->SendDiconnectMessage(NetworkNode::GetLocalPlayer()->GetUID(), '-');
			//RakSleep(100);
			m_interface.Disconnect(true);
		}
		m_interface.Shutdown(true);
	}
}

bool NetworkModule::StartServer()
{
	// ignore if client is connected to another server
	if (m_state == ClientState::REMOTE)
	{
		Log::GetInstance()->WriteToLog("Network", "Cannot start server while connected to another");
		return false;
	}

	if (!m_serverStarted)
	{
		if (ProcessUtils::StartProcess("MerryServer.exe", ""))
		{
			m_serverStarted = true;
			m_state = ClientState::HOST;

			Log::GetInstance()->WriteToLog("Network", "Server launched");
			return true;

			// start client peer if successful
			if (m_interface.IsActive())
			{
				// connect to just created server
				m_serverAddress = "127.0.0.1";

				RakSleep(500);
				ConnectToServer(m_serverPort, m_serverAddress.c_str());
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			Log::GetInstance()->WriteToLog("Network", "Was unable to start server process");
			return false;
		}
	}
	else
	{
		Log::GetInstance()->WriteToLog("Network", "Server start failed. Server is already running");
		return false;
	}
}

void NetworkModule::ShutdownServer()
{
	// TODO: send server an abort msg
}

bool NetworkModule::ConnectToServer(unsigned int port, const char* ip)
{
	std::stringstream ss;

	// TODO: check for valid port and ip

	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager();
	LANMenu* lanMenu = guiMgr->GetLayer<LANMenu>("lan_menu");

	ss.clear();
	ss.str(std::string());

	if (m_interface.Connect(port, ip))
	{
		ss << "Connected to " << ip << ":" << port;
		Log::GetInstance()->WriteToLog("Network", ss.str().c_str());

		m_serverPort = port;

		return true;
	}
	else
	{
		ss << "Could not connect to " << ip << ":" << port;
		Log::GetInstance()->WriteToLog("Network", ss.str().c_str());

		if (lanMenu)
		{
			lanMenu->SendSetSmartLabelMsg(SetSmartLabelMsg(ss.str(), SetSmartLabelMsg::CONNECTION_FAILURE));
		}

		return false;
	}
}

void NetworkModule::Disconnect()
{
	m_interface.Disconnect(true);

	std::stringstream ss;
	ss << "Disconnecting from " << m_interface.GetServerAddress()->ToString(true);
	Log::GetInstance()->WriteToLog("Network", ss.str().c_str());
}

void NetworkModule::Send(char* data, int len)
{
	VGSAssert(data != nullptr, "Cannot send empty packet");
	m_interface.Send(data, len);
}

void NetworkModule::Send(const RakNet::BitStream& bs)
{
	m_interface.Send(bs);
}

unsigned char NetworkModule::GetPacketIdentifier(RakNet::Packet *p)
{
	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	} 
	else 
	{
		return (unsigned char)p->data[0];
    }
}

void NetworkModule::OnServerConnectionLost(RakNet::SystemAddress* address)
{
	//int x = .9;
}

void NetworkModule::ProcessUICommands()
{
	// create temp queue
	UICommand::CmdQueue commands;
	// swap with existing
	UICommand::SwapNet(commands);

	while (!commands.empty())
	{
		UICommand cmd = commands.front();

		switch (cmd.GetId())
		{
		case UICommand::CMD_NET_CONNECT_TO:
			{
				unsigned short	port = 0;
				size_t			len = 0;

				memcpy(&port, cmd.GetData(), sizeof(unsigned short));
				memcpy(&len, cmd.GetData() + sizeof(unsigned short), sizeof(size_t));
				
				char* host = new char[len];
				memcpy(host, cmd.GetData() + sizeof(unsigned short) + sizeof(size_t), len);

				ConnectToServer(port, host);

				delete [] host;
			}
			break;

		case UICommand::CMD_NET_SEND_JOIN_REQUEST:
			{
				GetMsgHandler()->SendJoinRequest();
			}
			break;

		case UICommand::CMD_NET_PREGAME_SETUP_REQUEST:
			{
				Send(*cmd.GetStream());
			}
			break;

		default:
			break;
		}

		// free memory
		cmd.Free();
		// remove from list
		commands.pop_front();
	}
}

BEGIN_CALLBACK_FUNCTION(NetworkModule, network)
	if ( args.size() > 0 ) 
	{
		for ( unsigned int i = 0; i < args.size(); i += 2)
		{
			char* arg = args[i];

			if (StringUtils::IsEqual(arg, "interpolation"))
			{
				char* enable	= args[i + 1];
				int value		= StringUtils::IsEqual(enable, "on") ? 1 : StringUtils::IsEqual(enable, "off") ? 0 : -1;

				if (value > -1)
				{
					NetworkModule::SetEnableInterpolation(value != 0);
				}
				else
				{
					std::cout << "Valid values for interpolation are 'on' and 'off'" << std::endl;
				}
			}
			else if (StringUtils::IsEqual(arg, "delay"))
			{
				char* svalue = args[i + 1];
				float fvalue;

				std::stringstream ss(svalue);
				ss >> fvalue;

				if (ss.fail())
				{
					std::cout << "Couldn't parse client delay value" << std::endl;
					break;
				}

				if (fvalue > 0)
				{
					NetworkModule::SetClientDelay(fvalue);
				}
				else
				{
					std::cout << "Client delay value has to be positive" << std::endl;
				}
			}
		}
	}
END_CALLBACK_FUNCTION