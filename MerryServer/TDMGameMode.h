/*
========================

Copyright (c) 2014 Vinyl Games Studio

	TDMGameMode.h

		Created by: Vladyslav Dolhopolov
		Created on: 9/18/2014 2:28:15 PM

========================
*/
#ifndef __TDMGAMEMODE_H__
#define __TDMGAMEMODE_H__

#include "WrapperGameMode.h"

#include <unordered_set>

namespace vgs
{
	class ServerEngine;
	class PlayerMapHelper;

	typedef std::unordered_set<Player*>	PlayerSet;

	enum class TeamTag : char
	{
		NONE = -1,
		TEAM_A,
		TEAM_B
	};

	/*
	========================

		TDMGameMode

			Covers basic mechanics of the game, such as shooting, moving and picking up items.

			Created by: Vladyslav Dolhopolov
			Created on: 9/18/2014 2:28:15 PM

	========================
	*/
	class TDMGameMode : public WrapperGameMode
	{
	public:
										TDMGameMode(GameMode* dec);

		virtual bool					Init()																							override;
		virtual void					Update(float dt)																				override;
		virtual void					ProcessMessage(unsigned char pid, RakNet::Packet* packet)										override;
		
		/* Not for inside usage
		*/
		virtual RakNet::BitStream*		WritePregameSetup		(RakNet::BitStream* bs, Player* player)										override;
		virtual RakNet::BitStream*		WritePregameUpdate		(RakNet::BitStream* bs, Player* player)										override;
		virtual RakNet::BitStream*		WriteIngameUpdate		(RakNet::BitStream* bs, Player* player)										override;
		virtual RakNet::BitStream*		WriteNewPlayerConnected	(RakNet::BitStream* bs, Player* player)										override;
		virtual RakNet::BitStream*		WritePlayerHit			(RakNet::BitStream* bs, Player* victim, Player* shooter)					override;
		virtual RakNet::BitStream*		WritePlayerKilled		(RakNet::BitStream* bs, Player* victim, Player* killer)						override;
		virtual RakNet::BitStream*		WritePlayerStatsChanged	(RakNet::BitStream* bs, Player* player)										override;
		virtual RakNet::BitStream*		WriteBuffApplied		(RakNet::BitStream* bs, Player* player, const merrymen::Effect& effect)		override;
		virtual RakNet::BitStream*		WriteRespawn			(RakNet::BitStream* bs, Player* player)										override;
		virtual RakNet::BitStream*		WriteReload				(RakNet::BitStream* bs, Player* player)										override;
		virtual RakNet::BitStream*		WriteWeaponChanged		(RakNet::BitStream* bs, Player* player)										override;
		virtual RakNet::BitStream*		WriteObjectPicked		(RakNet::BitStream* bs, Player* player, unsigned char objectType)			override;

	private:
		void							UpdateRespawnCounters(float dt);
		void							UpdateTransform(Player* player, float dt);
		void							UpdateCombat(Player* player, float dt);
		void							UpdateStats(Player* player, float dt);

		void							SwitchFromAliveToDead(Player* player);
		void							SwitchFromDeadToAlive(Player* player);

		char							GetNextTeamTag();

	private:
		const float						m_respawnDelay;
		bool							m_friendlyFireOn;
	};

} // namespace vgs

#endif __TDMGAMEMODE_H__