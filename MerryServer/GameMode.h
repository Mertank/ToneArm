/*
========================

Copyright (c) 2014 Vinyl Games Studio

	GameMode.h

		Created by: Vladyslav Dolhopolov
		Created on: 9/17/2014 4:29:32 PM

========================
*/
#ifndef __GameMode_H__
#define __GameMode_H__

namespace RakNet
{
	struct Packet;
	struct SystemAddress;
	class  BitStream;
}

namespace merrymen
{
	struct Effect;
}

namespace vgs
{
	class Player;
	class GameWorld;

	/*
	========================

		GameMode

			Base game mode interface to make sure all the methods are implemented in children classes.

			Created by: Vladyslav Dolhopolov
			Created on: 9/17/2014 4:29:32 PM

	========================
	*/
	class GameMode
	{
	public:
		virtual							~GameMode() {}

		virtual bool					Init()																								= 0;
		virtual void					Update(float dt)																					= 0;
		virtual void					ProcessMessage(unsigned char pid, RakNet::Packet* packet)											= 0;

		virtual RakNet::BitStream*		WritePregameSetup		(RakNet::BitStream* bs, Player* player)										= 0;
		virtual RakNet::BitStream*		WritePregameUpdate		(RakNet::BitStream* bs, Player* player)										= 0;
		virtual RakNet::BitStream*		WriteIngameUpdate		(RakNet::BitStream* bs, Player* player)										= 0;
		virtual RakNet::BitStream*		WriteNewPlayerConnected	(RakNet::BitStream* bs, Player* player)										= 0;
		virtual RakNet::BitStream*		WritePlayerHit			(RakNet::BitStream* bs, Player* victim, Player* shooter)					= 0;
		virtual RakNet::BitStream*		WritePlayerKilled		(RakNet::BitStream* bs, Player* victim, Player* killer)						= 0;
		virtual RakNet::BitStream*		WritePlayerStatsChanged	(RakNet::BitStream* bs, Player* player)										= 0;
		virtual RakNet::BitStream*		WriteBuffApplied		(RakNet::BitStream* bs, Player* player, const merrymen::Effect& effect)		= 0;
		virtual RakNet::BitStream*		WriteRespawn			(RakNet::BitStream* bs, Player* player)										= 0;
		virtual RakNet::BitStream*		WriteReload				(RakNet::BitStream* bs, Player* player)										= 0;
		virtual RakNet::BitStream*		WriteWeaponChanged		(RakNet::BitStream* bs, Player* player)										= 0;
		virtual RakNet::BitStream*		WriteObjectPicked		(RakNet::BitStream* bs, Player* player, unsigned char objectType)			= 0;
	};

} // namespace vgs

#endif __GameMode_H__