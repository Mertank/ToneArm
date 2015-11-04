/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MessageHandler.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 5/22/2014 5:38:53 PM

========================
*/
#include "MessageHandler.h"
#include "NetworkMessageIDs.h"
#include "NetworkModule.h"
#include "NetworkNode.h"

#include <BitStream.h>

using namespace vgs;

void MessageHandler::SaveNode(NetworkNode* node)
{
	m_netNodes[node->GetUID()] = node;
}

void MessageHandler::RemoveNode(unsigned int uid)
{
	m_netNodes.erase(uid);
}

void MessageHandler::SendJoinRequest()
{
	char pid = (char)ToneArmMessage::ID_ENGINE_JOIN_REQUEST;
	m_module->Send(&pid, sizeof(char));
}

void MessageHandler::SendDiconnectMessage(unsigned char uid, unsigned char reason)
{
	char data[2];
	
	data[0] = (char)ToneArmMessage::ID_ENGINE_CLIENT_DISCONNECTED;
	data[1] = uid;
	// TODO: include what caused disconnection (enum)

	m_module->Send(data, 2);
}

void MessageHandler::SendInputPacket(const NetworkNode* player, RakNet::BitStream& data)
{
	RakNet::BitStream bs;

	bs.Write(MerryMessage::ID_MERRY_INPUT);
	bs.Write(player->GetUID());
	bs.Write(data, data.GetNumberOfBitsUsed());

	m_module->Send(bs);
}