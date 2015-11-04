/*
========================

Copyright (c) 2014 Vinyl Games Studio

	NetworkClient.h

		Created by: Vladyslav Dolhopolov
		Created on: 03/14/2014

========================
*/
#ifndef __NetworkClient_H__
#define __NetworkClient_H__

#include <string>

namespace RakNet
{
	struct	Packet;
	struct	SystemAddress;
	struct	RakNetGUID;
	
	class	RakPeerInterface;
	class	BitStream;
}

namespace vgs
{
	/*
	========================

		NetworkClient

			A class which represents a wrapper around RakNet peer
			to manage client network side.

			Created by: Vladyslav Dolhopolov
			Created on: 03/14/2014

	========================
	*/
	class NetworkClient
	{
	
	public:
									NetworkClient();
									~NetworkClient();
	
		bool						Start(unsigned short& port);
		void						Shutdown(bool sendNotification);
		bool						IsActive();
	
		bool						Connect(unsigned short port, const char* ip);
		void						Disconnect(bool sendNotification);
		bool						IsConnected();
	
		RakNet::Packet*				Receive();
	
		void						Send(char* data, int len);
		void						Send(const RakNet::BitStream& bs);
		void						DeallocatePacket(RakNet::Packet* packet);
		
		RakNet::SystemAddress*		GetServerAddress() const { return m_serverAddress; }

		void						PingAddress(RakNet::SystemAddress* ad);
		void						BroadcastPing(unsigned int port);

	private:
		RakNet::RakPeerInterface*	m_peer;
		RakNet::RakNetGUID*			m_serverGUID;
		RakNet::SystemAddress*		m_serverAddress;

	};
}

#endif __NetworkClient_H__