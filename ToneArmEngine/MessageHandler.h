/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MessageHandler.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/22/2014 5:38:53 PM

========================
*/
#ifndef __MESSAGEHANDLER_H__
#define __MESSAGEHANDLER_H__

#include <RakString.h>

#include <unordered_map>

namespace RakNet
{
	class BitStream;

	struct Packet;
}

namespace vgs
{
	class NetworkModule;
	class NetworkNode;

	/*
	========================

		MessageHandler

			Base class for ingame handling messages, each child will represent certain game modes.

			Created by: Vladyslav Dolhopolov
			Created on: 5/22/2014 5:38:53 PM

	========================
	*/
	class MessageHandler
	{
	public:
		typedef std::unordered_map<unsigned char, NetworkNode*> NetworkNodeMap;

		virtual					~MessageHandler( void ) {}

		virtual void			ProcessMessage(unsigned int pid, RakNet::Packet* packet)				= 0;
		
		virtual void			SendSetupRequest()														= 0;
		virtual void			ProcessPlayerDisconnection(unsigned int uid)							= 0;

		virtual void			SendJoinRequest();
		virtual void			SendDiconnectMessage(unsigned char uid, unsigned char reason);
		virtual void			SendInputPacket(const NetworkNode* player, RakNet::BitStream& data);

		virtual void			OpenPregameSetupWin()													= 0;

		void					SetModule(NetworkModule* ptr)											{ m_module = ptr; }		

		void					SaveNode(NetworkNode* node);
		void					RemoveNode(unsigned int uid);

		const NetworkNodeMap*	GetNetworkNodeMap( void )												{ return &m_netNodes; }

	protected:
		NetworkModule*			m_module;
		NetworkNodeMap			m_netNodes;

	};

} // namespace vgs

#endif __MESSAGEHANDLER_H__