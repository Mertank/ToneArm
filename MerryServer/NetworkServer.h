/*
========================

Copyright (c) 2014 Vinyl Games Studio

	NetworkServer.h

		Created by: Vladyslav Dolhopolov
		Created on: 03/14/2014

========================
*/
#ifndef __NETWORKSERVER_H__
#define __NETWORKSERVER_H__

#include <string>
#include <vector>
#include <unordered_set>

namespace RakNet
{
	struct	Packet;
	struct	AddressOrGUID;
	struct	SystemAddress;
	
	class	RakPeerInterface;
	class	BitStream;
}

namespace vgs
{
	class Player;

	typedef std::unordered_set<Player*> PlayerSet;

	/*
	========================

		NetworkServer

			A class which represents a wrapper around RakNet peer
			to manage server needs.

			Created by: Vladyslav Dolhopolov
			Created on: 03/14/2014

	========================
	*/
	class NetworkServer
	{
	public:
									NetworkServer();
									~NetworkServer();

		bool						Start(unsigned short port, unsigned int maxConnections);
		void						Shutdown(unsigned int blockDuration);
		bool						IsActive();

		void						CloseConnection(const RakNet::AddressOrGUID& systemIdentifier);

		void						SetPingResponse(const char* data, const unsigned int length);

		//unsigned short				GetMaxConnections() const;
		//unsigned short				GetMaxIncomingConnections() const;
		//unsigned short				GetCurrentConnections() const;

		RakNet::Packet*				Receive();

		void						Send(char* data, int len, const RakNet::AddressOrGUID& systemIdentifier, bool broadcast = false) const;
		void						Send(RakNet::BitStream& bs, const RakNet::AddressOrGUID& systemIdentifier, bool broadcast = false) const;
		
		void						Broadcast(char* data, int len) const;
		void						Broadcast(RakNet::BitStream& bs) const;

		void						Multicast(char* data, int len, const PlayerSet& players) const;
		void						Multicast(RakNet::BitStream& bs, const PlayerSet& players) const;

		void						DeallocatePacket(RakNet::Packet* packet) const;

	private:
		RakNet::RakPeerInterface*	m_peer;

	};

}

#endif __NETWORKSERVER_H__