/*
========================

Copyright (c) 2014 Vinyl Games Studio

	WrapperGameMode.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 9/18/2014 2:06:12 PM

========================
*/
#include "WrapperGameMode.h"
#include "ServerEngine.h"
#include "GameWorld.h"
#include "PlayerMapHelper.h"

using namespace vgs;

WrapperGameMode::WrapperGameMode(GameMode* dec)
	: m_wrapped(dec)
{
}

WrapperGameMode::~WrapperGameMode()
{
	delete m_wrapped;
}

bool WrapperGameMode::Init()													
{
	m_engine	= ServerEngine::GetInstance();
	m_gw		= m_engine->GetGameWorld();
	m_players	= &m_gw->GetPlayers();
	return m_wrapped->Init() && m_engine && m_gw && m_players;
}

void WrapperGameMode::Update(float dt)										
{
	m_wrapped->Update(dt);
}

void WrapperGameMode::ProcessMessage(unsigned char pid, RakNet::Packet* packet)
{
	m_wrapped->ProcessMessage(pid, packet);
}

RakNet::BitStream* WrapperGameMode::WritePregameSetup(RakNet::BitStream* bs, Player* player)														
{
	m_wrapped->WritePregameSetup(bs, player);
	return bs;
}

RakNet::BitStream* WrapperGameMode::WritePregameUpdate(RakNet::BitStream* bs, Player* player)														
{
	m_wrapped->WritePregameUpdate(bs, player);
	return bs;
}

RakNet::BitStream* WrapperGameMode::WriteIngameUpdate(RakNet::BitStream* bs, Player* player)																			
{
	m_wrapped->WriteIngameUpdate(bs, player);
	return bs;
}

RakNet::BitStream* WrapperGameMode::WriteNewPlayerConnected(RakNet::BitStream* bs, Player* player)														
{
	m_wrapped->WriteNewPlayerConnected(bs, player);
	return bs;
}

RakNet::BitStream* WrapperGameMode::WritePlayerHit(RakNet::BitStream* bs, Player* victim, Player* shooter)							
{
	m_wrapped->WritePlayerHit(bs, victim, shooter);
	return bs;
}

RakNet::BitStream* WrapperGameMode::WritePlayerKilled(RakNet::BitStream* bs, Player* victim, Player* killer)	
{
	m_wrapped->WritePlayerKilled(bs, victim, killer);
	return bs;
}

RakNet::BitStream* WrapperGameMode::WritePlayerStatsChanged(RakNet::BitStream* bs, Player* player)															
{
	m_wrapped->WritePlayerStatsChanged(bs, player);
	return bs;
}

RakNet::BitStream* WrapperGameMode::WriteBuffApplied(RakNet::BitStream* bs, Player* player, const merrymen::Effect& effect)
{
	m_wrapped->WriteBuffApplied(bs, player, effect);
	return bs;
}

RakNet::BitStream* WrapperGameMode::WriteRespawn(RakNet::BitStream* bs, Player* player)															
{
	m_wrapped->WriteRespawn(bs, player);
	return bs;
}

RakNet::BitStream* WrapperGameMode::WriteReload(RakNet::BitStream* bs, Player* player)															
{
	m_wrapped->WriteReload(bs, player);
	return bs;
}

RakNet::BitStream* WrapperGameMode::WriteWeaponChanged(RakNet::BitStream* bs, Player* player)
{
	m_wrapped->WriteWeaponChanged(bs, player);
	return bs;
}

RakNet::BitStream* WrapperGameMode::WriteObjectPicked(RakNet::BitStream* bs, Player* player, unsigned char objectType) {

	m_wrapped->WriteObjectPicked(bs, player, objectType);
	return bs;
}