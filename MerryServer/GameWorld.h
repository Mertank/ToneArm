/*
========================

Copyright (c) 2014 Vinyl Games Studio

	GameWorld.h

		Created by: Vladyslav Dolhopolov
		Created on: 9/17/2014 4:28:51 PM

========================
*/
#ifndef __GAMEWORLD_H__
#define __GAMEWORLD_H__

#include "UIDManager.h"
#include "PlayerMapHelper.h"

namespace RakNet
{
	struct SystemAddress;
}

namespace merrymen
{
	struct Effect;
}

namespace vgs
{
	class ServerEngine;
	class ServerEntity;
	class Level;
	class Player;
	class GameMode;

	typedef std::unordered_set<Player*>			PlayerSet;
	typedef std::unordered_set<ServerEntity*>	EntitySet;

	/*
	========================

		GameWorld

			Description.

			Created by: Vladyslav Dolhopolov
			Created on: 9/17/2014 4:28:51 PM

	========================
	*/
	class GameWorld
	{
	public:
								GameWorld(int maxPlayers);
								~GameWorld();

		bool					Init();
		void					Update(float dt);
		void					ProcessMessage(unsigned char pid, RakNet::Packet* packet);

		Player*					AddPlayer(const RakNet::RakString& name, char characterClass, char team, const RakNet::SystemAddress& address);
		void					RemovePlayer(unsigned char uid);
		unsigned char			RemovePlayer(RakNet::SystemAddress& address);
		
		void					SendPregameSetup		(Player* player);														
		void					SendPregameUpdate		(Player* player);														
		void					SendIngameUpdate		(Player* player);																			
		void					SendNewPlayerConnected	(Player* player);													
		void					SendPlayerHit			(Player* victim, Player* shooter);						
		void					SendPlayerKilled		(Player* victim, Player* killer);											
		void					SendPlayerStatsChanged	(Player* player);															
		void					SendBuffApplied			(Player* player, const merrymen::Effect& effect);
		void					SendRespawn				(Player* player);															
		void					SendReload				(Player* player);
		void					SendWeaponChanged		(Player* player);
		void					SendObjectPicked		(Player* player, unsigned char objectType);

		int						GetNowPlayers()	const							{ return m_nowPlayers; }
		int						GetMaxPlayers()	const							{ return m_maxPlayers; }

		bool					HasRoom() const									{ return m_nowPlayers < m_maxPlayers; }
		Level*					GetLevel() const								{ return m_level; }

		PlayerMapHelper&		GetPlayers()									{ return m_players; }

	private:
		GameMode*				m_gm;
		UIDManager				m_uidMgr;

		int						m_nowPlayers;
		int						m_maxPlayers;
	
		PlayerMapHelper			m_players;
		//EntitySet				m_entities;

		Level*					m_level;
	};

} // namespace vgs

#endif __GAMEWORLD_H__