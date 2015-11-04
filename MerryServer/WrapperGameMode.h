/*
========================

Copyright (c) 2014 Vinyl Games Studio

	WrapperGameMode.h

		Created by: Vladyslav Dolhopolov
		Created on: 9/18/2014 2:06:12 PM

========================
*/
#ifndef __WrapperGameMode_H__
#define __WrapperGameMode_H__

#include "GameMode.h"

namespace vgs
{
	class GameWorld;
	class ServerEngine;
	class PlayerMapHelper;

	/*
	========================

		WrapperGameMode

			Base class for decorating game modes.
			Children have to call this class's methods before adding their's logic.
			This class makes sure that parent's methods get called in a chain right to the core class.

			Created by: Vladyslav Dolhopolov
			Created on: 9/18/2014 2:06:12 PM

	========================
	*/
	class WrapperGameMode : public GameMode
	{
	public:
										WrapperGameMode(GameMode* dec);
										~WrapperGameMode()																					override;

		virtual bool					Init()																								override;
		virtual void					Update(float dt)																					override;
		virtual void					ProcessMessage(unsigned char pid, RakNet::Packet* packet)											override;
		
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

	protected:
		GameMode*						m_wrapped;

		// just refs
		ServerEngine*					m_engine;
		PlayerMapHelper*				m_players;
		GameWorld*						m_gw;
	};

} // namespace vgs

#endif __WrapperGameMode_H__