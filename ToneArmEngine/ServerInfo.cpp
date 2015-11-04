/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ServerInfo.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 5/26/2014 5:53:51 PM

========================
*/
#include "ServerInfo.h"
#include "Engine.h"
#include "NetworkModule.h"
#include "NetworkNode.h"
#include "UICommand.h"
#include "OpenGLRenderModule.h"
#include "LANMenu.h"
#include "OpenGLGUIPass.h"
#include "GUIManager.h"

#include "../Merrymen/GameScene.h"
#include "../Merrymen/TeamDeathMatchMH.h"

using namespace vgs;

ServerInfo::ServerInfo()
	: capacity(0)
	, nowPlaying(0)
	, ping(0)
	, clicked(false)
{
}

void ServerInfo::Connect()
{
	////if (!clicked)
	//{
	//	NetworkModule* net = Engine::GetInstance()->GetModuleByType<NetworkModule>(EngineModuleType::NETWORKER);
	//	if (net)
	//	{
	//		//
	//		// on logic thread version
	//		//
	//		//net->ConnectToServer(address.GetPort(), address.ToString(false));
	//		//net->GetMsgHandler()->SendJoinRequest();

	//		//
	//		// on rendering thread
	//		//
	//		unsigned short port = address.GetPort();
	//		const char* host = address.ToString(false);
	//		size_t hostLen = strlen(address.ToString(false)) + 1;

	//		// len = port + len of ip str + ip str
	//		unsigned int	len		= sizeof(unsigned short) + sizeof(size_t) + hostLen + 1;
	//		unsigned char*	data	= new unsigned char[len]; // for \0
	//		unsigned int	offset	= 0;

	//		memcpy(data + offset, &port, sizeof(unsigned short));
	//		offset += sizeof(unsigned short);
	//		memcpy(data + offset, &hostLen, sizeof(size_t));
	//		offset += sizeof(size_t);
	//		memcpy(data + offset, host, hostLen);

	//		data[len - 1] = '\0';

	//		UICommand::AddNetCommand(UICommand(UICommand::CMD_NET_CONNECT_TO, len, data));
	//		UICommand::AddNetCommand(UICommand(UICommand::CMD_NET_SEND_JOIN_REQUEST, 0, 0));
	//	}

	//	clicked = true;
	//}
}

void ServerInfo::Reconnect()
{
}

void ServerInfo::Selected()
{
	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager();
	LANMenu* lanM = guiMgr->GetLayer<LANMenu>("lan_menu");
	lanM->OnRowSelection(this);
}

bool ServerInfo::operator ==(const ServerInfo& other) 
{
	return address == other.address;
}

bool ServerInfo::operator ==(const RakNet::SystemAddress& other) 
{
	return address == other;
}