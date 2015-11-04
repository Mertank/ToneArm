/*
========================

Copyright (c) 2014 Vinyl Games Studio

	GameWorld.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 9/17/2014 4:28:51 PM

========================
*/
#include "GameWorld.h"
#include "CoreGameMode.h"
#include "TDMGameMode.h"
#include "HeistTestGameMode.h"
#include "ServerEngine.h"
#include "Level.h"
#include "NetworkServer.h"
#include "Player.h"
#include "CharacterEntity.h"

#include <BitStream.h>

#include <iostream>
#include <memory>

using namespace vgs;

#define UNICAST_MESSAGE(__writeMeth__, ...)													\
{																							\
	std::unique_ptr<RakNet::BitStream> bs(new RakNet::BitStream);							\
	m_gm->__writeMeth__(bs.get(), __VA_ARGS__);												\
	ServerEngine* s = ServerEngine::GetInstance();											\
	s->GetNetworkInterface().Send(*bs, s->GetClientAddressByUID(player->GetUID()), false);	\
}																							\

#define MULTICAST_MESSAGE(__writeMeth__, __playerSet__, ...)								\
{																							\
	std::unique_ptr<RakNet::BitStream> bs(new RakNet::BitStream);							\
	m_gm->__writeMeth__(bs.get(), __VA_ARGS__);												\
	ServerEngine* s = ServerEngine::GetInstance();											\
	s->GetNetworkInterface().Multicast(*bs, __playerSet__);									\
}																							\

GameWorld::GameWorld(int maxPlayers)
	: m_gm(nullptr)
	, m_maxPlayers(maxPlayers)
	, m_nowPlayers(0)
	, m_level(nullptr)
{
}

GameWorld::~GameWorld()
{
	delete m_gm;
	delete m_level;
	// TODO: delete entities
}

bool GameWorld::Init()
{
	m_level = new Level("Casino/Casino.vgsLevel");

	m_gm = new HeistTestGameMode(new TDMGameMode(new CoreGameMode()));
	return m_gm->Init();
}

void GameWorld::Update(float dt)
{
	try
	{
		m_gm->Update(dt);
	}
	catch (const PlayerNonexists& ex)
	{
		std::cout << "Exception caught in GameWorld::Update: " << ex.what() << std::endl;
	}
}

void GameWorld::ProcessMessage(unsigned char pid, RakNet::Packet* packet)
{
	try
	{
		m_gm->ProcessMessage(pid, packet);
	}
	catch (const PlayerNonexists& ex)
	{
		std::cout << "Exception caught in GameWorld::ProcessMessage: " << ex.what() << std::endl;
	}
}

Player* GameWorld::AddPlayer(const RakNet::RakString& name, char characterClass, char team, const RakNet::SystemAddress& address)
{
	if (!HasRoom())
	{
		return nullptr;
	}

	unsigned char uid = m_uidMgr.GenerateUID();
	Player* player = new Player(uid, name, characterClass, team, address);

	// add to containers
	try
	{
		m_players.Insert(uid, player);
		//m_entities.insert(player->GetCharacter());
	}
	catch (const PlayerAlreadyExists& ex)
	{
		std::cout << "Exception caught in GameWorld::AddPlayer: " << ex.what() << std::endl;
		delete player;
		return nullptr;
	}

	m_nowPlayers++;

	// pick spawn point from a map
#ifdef TONEARM_DEBUG
	m_level->SpawnPlayer(player, SpawnMode::RANDOM);
#else
	m_level->SpawnPlayer(player, SpawnMode::RANDOM);
#endif

	return player;
}

void GameWorld::RemovePlayer(unsigned char uid)
{
	Player* player = nullptr;

	try
	{
		player = m_players[uid];
		m_players.Remove(uid);
		m_nowPlayers--;

		// TODO: put in players destructor
		//m_entities.erase(player->GetCharacter());
		delete player->GetCharacter();

		ServerEngine::GetInstance()->RemoveClientAddress(uid);

		std::cout << "Player (" << (unsigned)uid  << ") was removed from the server" << std::endl;
	}
	catch (const PlayerNonexists& ex)
	{
		std::cout << "Exception caught in GameWorld::RemovePlayer: " << ex.what() << std::endl;
		std::cout << "Couldn't remove player from the server" << std::endl;
	}
}

unsigned char GameWorld::RemovePlayer(RakNet::SystemAddress& address)
{
	unsigned char uid = ServerEngine::GetInstance()->SearchForUIDByClientAddress(address);
	if (uid > 0)
	{
		RemovePlayer(uid);
	}
	return uid;
}

void GameWorld::SendPregameSetup(Player* player)
{
	UNICAST_MESSAGE(WritePregameSetup, player)
}

void GameWorld::SendPregameUpdate(Player* player)
{
	UNICAST_MESSAGE(WritePregameUpdate, player)
}
													
void GameWorld::SendIngameUpdate(Player* player)
{
	UNICAST_MESSAGE(WriteIngameUpdate, player)
}
																		
void GameWorld::SendNewPlayerConnected(Player* player)
{
	MULTICAST_MESSAGE(WriteNewPlayerConnected, m_players.GetAllSet(), player)
}
												
void GameWorld::SendPlayerHit(Player* victim, Player* shooter)
{
	MULTICAST_MESSAGE(WritePlayerHit, m_players.GetByPhaseSet(PlayerState::GamePhase::INGAME), victim, shooter)
}
						
void GameWorld::SendPlayerKilled(Player* victim, Player* killer)
{
	MULTICAST_MESSAGE(WritePlayerKilled, m_players.GetByPhaseSet(PlayerState::GamePhase::INGAME), victim, killer)
}
										
void GameWorld::SendPlayerStatsChanged(Player* player)
{
	MULTICAST_MESSAGE(WritePlayerStatsChanged, m_players.GetByPhaseSet(PlayerState::GamePhase::INGAME), player)
}
													
void GameWorld::SendBuffApplied(Player* player, const merrymen::Effect& effect)
{
	MULTICAST_MESSAGE(WriteBuffApplied, m_players.GetByPhaseSet(PlayerState::GamePhase::INGAME), player, effect)
}

void GameWorld::SendRespawn(Player* player)
{
	MULTICAST_MESSAGE(WriteRespawn, m_players.GetByPhaseSet(PlayerState::GamePhase::INGAME), player)
}
															
void GameWorld::SendReload(Player* player)
{
	MULTICAST_MESSAGE(WriteReload, m_players.GetByPhaseSet(PlayerState::GamePhase::INGAME), player)
}
							
void GameWorld::SendWeaponChanged(Player* player)
{
	MULTICAST_MESSAGE(WriteWeaponChanged, m_players.GetByPhaseSet(PlayerState::GamePhase::INGAME), player)
}

void GameWorld::SendObjectPicked(Player* player, unsigned char objectType) {

	MULTICAST_MESSAGE(WriteObjectPicked, m_players.GetByPhaseSet(PlayerState::GamePhase::INGAME), player, objectType)
}