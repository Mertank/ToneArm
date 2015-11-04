/*
========================

Copyright (c) 2014 Vinyl Games Studio

	CoreGameMode.h

		Created by: Vladyslav Dolhopolov
		Created on: 9/18/2014 1:58:46 PM

========================
*/
#ifndef __COREGAMEMODE_H__
#define __COREGAMEMODE_H__

#include "GameMode.h"

namespace vgs
{
	/*
	========================

		CoreGameMode

			The one in the chain which doesn't call parent's methods.

			Created by: Vladyslav Dolhopolov
			Created on: 9/18/2014 1:58:46 PM

	========================
	*/
	class CoreGameMode : public GameMode
	{
	public:
										CoreGameMode() {}
										~CoreGameMode() {}

		virtual bool					Init() { return true; }
		virtual void					Update(float dt) {}
		virtual void					ProcessMessage(unsigned char pid, RakNet::Packet* packet) {}
		
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
	};

} // namespace vgs

#endif __COREGAMEMODE_H__