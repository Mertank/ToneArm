/*
========================

Copyright (c) 2014 Vinyl Games Studio

	HeistTestGameMode.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 10/3/2014 4:14:15 PM

========================
*/
#include "HeistTestGameMode.h"
#include "GameWorld.h"
#include "Level.h"
#include "ServerEngine.h"
#include "CharacterEntity.h"
#include "Player.h"

#include "../ToneArmEngine/Trigger.h"
#include "../ToneArmEngine/PhysicsModule.h"

#include <iostream>
#include <algorithm>

using namespace vgs;

HeistTestGameMode::HeistTestGameMode(GameMode* dec)
	: WrapperGameMode(dec)
{
}

bool HeistTestGameMode::Init()		
{
	WrapperGameMode::Init();

	PhysicsModule* pm = m_engine->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
	
	std::vector<Trigger*> triggers = pm->GetAllTriggers();
	for (auto trigger : triggers)
	{
		trigger->SetOnEnterCallback(std::bind(&HeistTestGameMode::OnTriggerEnter, this, std::placeholders::_1, std::placeholders::_2));
		trigger->SetOnExitCallback(std::bind(&HeistTestGameMode::OnTriggerExit, this, std::placeholders::_1, std::placeholders::_2));
		trigger->SetActive(true);
	}

	return true;
}

void HeistTestGameMode::Update(float dt)										
{
	WrapperGameMode::Update(dt);
}

void HeistTestGameMode::ProcessMessage(unsigned char pid, RakNet::Packet* packet)
{
	WrapperGameMode::ProcessMessage(pid, packet);
}

void HeistTestGameMode::OnTriggerEnter(GameObject* const character, Trigger* const trigger)
{
	CharacterEntity* ch = static_cast<CharacterEntity*>(character);
	Player* owner = static_cast<Player*>(ch->GetOwner());
	std::cout << " On Trigger Enter event: " << owner->GetName().C_String() << "(" << (unsigned)owner->GetTeamTag() << ") >> " << trigger->GetTag() << std::endl;
}

void HeistTestGameMode::OnTriggerExit(GameObject* const character, Trigger* const trigger)
{
	CharacterEntity* ch = static_cast<CharacterEntity*>(character);
	Player* owner = static_cast<Player*>(ch->GetOwner());
	std::cout << " On Trigger Exit event: " << owner->GetName().C_String() << "(" << (unsigned)owner->GetTeamTag() << ") >> " << trigger->GetTag() << std::endl;
}