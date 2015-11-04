/*
========================

Copyright (c) 2014 Vinyl Games Studio

	CoreGameMode.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 9/18/2014 1:58:46 PM

========================
*/
#include "CoreGameMode.h"

#include <NetworkMessageIDs.h>

#include <BitStream.h>
#include <RakNetTypes.h>
#include <GetTime.h>

using namespace vgs;

RakNet::BitStream* CoreGameMode::WritePregameSetup(RakNet::BitStream* bs, Player* player)														
{
	bs->Write((unsigned char)MerryMessage::ID_MERRY_PREGAME_SETUP_ANSWER);
	return bs;
}

RakNet::BitStream* CoreGameMode::WritePregameUpdate(RakNet::BitStream* bs, Player* player)														
{
	bs->Write((unsigned char)MerryMessage::ID_MERRY_PREGAME_UPDATE);
	return bs;
}

RakNet::BitStream* CoreGameMode::WriteIngameUpdate(RakNet::BitStream* bs, Player* player)																			
{
	// stamped
	bs->Write((unsigned char)ID_TIMESTAMP);
	bs->Write(RakNet::GetTime());
	bs->Write((unsigned char)MerryMessage::ID_MERRY_INGAME_UPDATE);
	return bs;
}

RakNet::BitStream* CoreGameMode::WriteNewPlayerConnected(RakNet::BitStream* bs, Player* player)														
{
	bs->Write((unsigned char)MerryMessage::ID_MERRY_NEW_PLAYER_CONNECTED);
	return bs;
}

RakNet::BitStream* CoreGameMode::WritePlayerHit(RakNet::BitStream* bs, Player* victim, Player* shooter)							
{
	bs->Write((unsigned char)MerryMessage::ID_MERRY_INGAME_HIT);
	return bs;
}

RakNet::BitStream* CoreGameMode::WritePlayerKilled(RakNet::BitStream* bs, Player* victim, Player* killer)	
{
	bs->Write((unsigned char)MerryMessage::ID_MERRY_INGAME_KILLED);
	return bs;
}

RakNet::BitStream* CoreGameMode::WritePlayerStatsChanged(RakNet::BitStream* bs, Player* player)															
{
	bs->Write((unsigned char)MerryMessage::ID_MERRY_INGAME_BASIC_STATS_CHANGED);
	return bs;
}

RakNet::BitStream* CoreGameMode::WriteBuffApplied(RakNet::BitStream* bs, Player* player, const merrymen::Effect& effect)
{
	bs->Write((unsigned char)MerryMessage::ID_MERRY_INGAME_BUFF_APPLIED);
	return bs;
}

RakNet::BitStream* CoreGameMode::WriteRespawn(RakNet::BitStream* bs, Player* player)															
{
	bs->Write((unsigned char)MerryMessage::ID_MERRY_INGAME_RESPAWNED);
	return bs;
}

RakNet::BitStream* CoreGameMode::WriteReload(RakNet::BitStream* bs, Player* player)															
{
	bs->Write((unsigned char)MerryMessage::ID_MERRY_INGAME_RELOAD);
	return bs;
}

RakNet::BitStream* CoreGameMode::WriteWeaponChanged(RakNet::BitStream* bs, Player* player)
{
	bs->Write((unsigned char)MerryMessage::ID_MERRY_INGAME_CHANGE_WEAPON);
	return bs;
}

RakNet::BitStream* CoreGameMode::WriteObjectPicked(RakNet::BitStream* bs, Player* player, unsigned char objectType)
{
	bs->Write((unsigned char)MerryMessage::ID_MERRY_INGAME_OBJECT_PICKED);
	return bs;
}