/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ServerEngine.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/22/2014 1:57:27 PM

========================
*/
#ifndef __SERVERENGINE_H__
#define __SERVERENGINE_H__

#include "NetworkServer.h"

#include "../ToneArmEngine/CachedResourceLoader.h"

#include <NetConsts.h>
#include <Modular.h>

#include <unordered_map>

#include <RakNetTypes.h>

namespace vgs
{
	class GameWorld;

	/*
	========================

		ServerEngine

			Manages client connections and runs server's update loop.

			Created by: Vladyslav Dolhopolov
			Created on: 5/22/2014 1:57:27 PM

	========================
	*/
	class ServerEngine : public Modular
	{
		friend GameWorld;
		typedef std::unordered_map<unsigned char, RakNet::SystemAddress> UIDToAddressMap;

	public:
		static ServerEngine*			GetInstance();
		static void						FreeInstance();

		bool							Startup(GameWorld* gm);
		void							Shutdown();

		void							Run();
		void							ProcessMessages();
		void							UpdatePingResponse();

		void							CloseConnectionAndNotify(unsigned char uid, RakNet::SystemAddress& address);

		void							SaveClientAddress(unsigned char uid, RakNet::SystemAddress& address)		{ m_addressMap[uid] = address; }
		void							RemoveClientAddress(unsigned char uid)										{ m_addressMap.erase(uid); }
		RakNet::SystemAddress&			GetClientAddressByUID(unsigned char uid)									{ return m_addressMap[uid]; }
		unsigned char					SearchForUIDByClientAddress(RakNet::SystemAddress& address);

		const Tick&						GetTick() const																{ return m_tick; }
		GameWorld*						GetGameWorld()																{ return m_gw; }

	private:
										ServerEngine();
										~ServerEngine();

		unsigned char					GetPacketIdentifier(RakNet::Packet *p);
		// keep it private so all message take off happens only through GameWorld class
		NetworkServer&					GetNetworkInterface()														{ return m_netInterface; }

	private:
		static ServerEngine*			s_instance;

		NetworkServer					m_netInterface;
		GameWorld*						m_gw;
		Tick							m_tick;
		UIDToAddressMap					m_addressMap;
	};

} // namespace vgs

#endif __SERVERENGINE_H__