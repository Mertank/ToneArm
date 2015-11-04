/*
========================

Copyright (c) 2014 Vinyl Games Studio

	TDMGameMode.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 9/18/2014 2:28:15 PM

========================
*/
#include "TDMGameMode.h"
#include "Player.h"
#include "PlayerMapHelper.h"
#include "ServerEngine.h"
#include "GameWorld.h"
#include "CharacterEntity.h"
#include "Level.h"
#include "ShootingHelperMP.h"
#include "PickableBuffSS.h"
#include "Player.h"

#include "../ToneArmEngine/InputModule.h"

#include <Effect.h>
#include <PickableItem.h>
#include <Damage.h>
#include <NetworkMessageIDs.h>

#include <BitStream.h>

#include <iostream>
#include <unordered_set>

using namespace vgs;

TDMGameMode::TDMGameMode(GameMode* dec)
	: WrapperGameMode(dec)
	, m_respawnDelay(5.f) // sec
	, m_friendlyFireOn(true)
{
}

bool TDMGameMode::Init()													
{
	return WrapperGameMode::Init();
}

void TDMGameMode::Update(float dt)										
{
	WrapperGameMode::Update(dt);

	UpdateRespawnCounters(dt);

	PlayerSet aliveSet = m_players->GetByStatusSet(PlayerState::IngameStatus::ALIVE);
	if (!aliveSet.empty())
	{
		for (auto it = aliveSet.begin(); it != aliveSet.end(); ++it)
		{
			Player* player = *it;

			// update players transform
			UpdateTransform(player, dt);
			// update player's basic stats
			UpdateStats(player, dt);
			// update shooting timers and check if hit someone
			UpdateCombat(player, dt);

			// see if this client needs updating
			if (m_engine->GetTick().frameno - player->GetLastUpdateSent() >= g_serverNetInfo.updatePeriod)
			{
				m_gw->SendIngameUpdate(player);
				// change client's last update value
                player->GetLastUpdateSent() = m_engine->GetTick().frameno;
			}

			// move to dead list if needed
			if (player->GetMustDie())
			{
				SwitchFromAliveToDead(player);
			}
		}
	}
}

void TDMGameMode::UpdateRespawnCounters(float dt)
{
	// update death counters
	PlayerSet dead = m_players->GetByStatusSet(PlayerState::IngameStatus::DEAD);
	for (auto player : dead)
	{
		float& counter = player->GetRespawnCounter();
		counter -= dt;

		if (counter < 0)
		{
			SwitchFromDeadToAlive(player);

			// resets player's params
			player->GetCharacter()->Respawn();

			// pick spawn point from a map
#ifdef TONEARM_DEBUG
			m_gw->GetLevel()->SpawnPlayer(player, SpawnMode::DEBUG);
#else
			m_gw->GetLevel()->SpawnPlayer(player, SpawnMode::RANDOM);
#endif

			// send respawn message
			m_gw->SendRespawn(player);
			
			std::cout << "Player " << player->GetName() << " has respawned" << std::endl;
		}
	}
}

void TDMGameMode::UpdateTransform(Player* player, float dt)
{
	CharacterEntity* character = player->GetCharacter();

	glm::vec3& dir	= character->GetDirection();
	glm::vec3 pos	= character->GetPosition();

	glm::vec3 forward = character->GetForward();

	if (character->IsSprinting() && (dir.x != 0.0f || dir.z != 0.0f))
	{
		dir = forward;
	}

	// save character speed
	float movementSpeed = character->GetCharacterStats().WalkSpeed * dt;

	if (character->IsSprinting()) 
	{
		movementSpeed = character->GetCharacterStats().SprintSpeed * dt;
	}
	else if (character->IsAiming())
	{
		movementSpeed = character->GetCharacterStats().WalkSpeedAiming * dt;
	}

	// initial move
	if (dir.x != 0 || dir.z != 0)
	{
		pos += dir * movementSpeed;

		character->SetPosition(pos);

		character->SetMoving(true);
	}
	else
	{
		character->SetMoving(false);
	}

	character->SetPosition(pos);
}

void TDMGameMode::UpdateCombat(Player* player, float dt)
{
	CharacterEntity* character = player->GetCharacter();
	WeaponSS* activeWeapon = character->GetActiveWeapon();

	// update the weapon
	activeWeapon->Update(dt);

	bool shotPerformed = false;

	// send shoot signal to a weapon when lmouse is pressed
	if (character->IsShooting())
	{
		shotPerformed = character->GetActiveWeapon()->Shoot();
	}
	else
	{
		character->GetActiveWeapon()->SetShooting(false);
	}

	// a shot was performed
	if (activeWeapon && shotPerformed)
	{
		unsigned char shooterUID = player->GetUID();
		std::multimap<CharacterEntity*, float> victims;
		std::shared_ptr<ShotResultMP> hitObjectPtr;

		// prepare data for ray casting
		std::vector<ColliderComponent*> geometryColliders;
		std::vector<CharacterEntity*> targets;
		std::unordered_set<Player*> players;

		// get the list of enemies
		// with friendly fire
		if (!m_friendlyFireOn) {

			players = m_players->GetByStatusSet(vgs::PlayerState::IngameStatus::ALIVE);

			for (auto pl : players) {

				if (pl != player) {
					targets.push_back(pl->GetCharacter());
				}
			}
		}
		// without friendly fire
		else {

			char enemiesTeam = (player->GetTeamTag() == (char)TeamTag::TEAM_A) ? (char)TeamTag::TEAM_B : (char)TeamTag::TEAM_A;
			players = m_players->GetByTeamSet(enemiesTeam);

			for (auto pl : players) {

				if (pl != player && pl->GetState().GetStatus() == PlayerState::IngameStatus::ALIVE) {
					targets.push_back(pl->GetCharacter());
				}
			}
		}

		ShootingHelperMP helper = ShootingHelperMP();

		if (targets.size() > 0) {

			// prepare geometry data (colliders for ray casting)
			helper.PreapareGeometryData(geometryColliders, targets);

			glm::vec3 shotOrigin = character->GetPosition() + glm::vec3(0.0f, CHARACTER_HEIGHT / 2, 0.0f) + character->GetForward() * WEAPON_LENGTH;

			// run raycast checks
			for (int i = 0; i < activeWeapon->GetProjectileStats().ProjectilesPerShot; i++) {

				ShotResultMP* hitPlayer = nullptr;

				// caclulate shot direction for that particular projectile
				glm::vec3 shotDirection = helper.CalculateShotDirection(player->GetCharacter(), activeWeapon->GetWeaponStats());

				activeWeapon->RunRaycastChecks(shotOrigin, shotDirection, geometryColliders, targets, victims);
			}

			geometryColliders.clear();
			players.clear();
			targets.clear();

			// go through all of the characters which were hit and send appropriate messages to all of the clients
			for (std::multimap<CharacterEntity*, float>::iterator victimItr = victims.begin(); victimItr!= victims.end(); ++victimItr)
			{
				// calculate the amount of damage to be dealt
				const int count = victims.count(victimItr->first);

				unsigned short weaponDamage = activeWeapon->GetWeaponStats().Damage;
				float aimingDistance = activeWeapon->GetWeaponStats().AimingDistance;

				merrymen::Damage dmg = merrymen::Damage::CalculateDamage(count, weaponDamage, victimItr->second, aimingDistance, activeWeapon->GetProjectileStats());

				Player* shooter = (*m_players)[shooterUID];
				Player* victim	= static_cast<Player*>(victimItr->first->GetOwner());

				victim->GetCharacter()->TakeDamage(dmg, activeWeapon->GetProjectileStats().Type, shooter->GetCharacter());

				m_gw->SendPlayerHit(victim, shooter);

				std::multimap<CharacterEntity*, float>::iterator currentItr = victimItr;
				std::advance(victimItr, count - 1);
				victims.erase(currentItr, victimItr);
			}

			victims.clear();
		}
	}
}

void TDMGameMode::UpdateStats(Player* player, float dt) {

	if (!player->GetMustDie()) {

		// send an update package to this player
		if (player->GetCharacter()->UpdateStats(dt)) {
			m_gw->SendPlayerStatsChanged(player);
		}
	}
}

void TDMGameMode::SwitchFromAliveToDead(Player* player)
{
	player->SetMustDie(false);
	player->GetState().SetStatus(PlayerState::IngameStatus::DEAD);
	player->GetRespawnCounter() = m_respawnDelay;
}

void TDMGameMode::SwitchFromDeadToAlive(Player* player)
{
	player->GetState().SetStatus(PlayerState::IngameStatus::ALIVE);
}

void TDMGameMode::ProcessMessage(unsigned char pid, RakNet::Packet* packet)
{
	WrapperGameMode::ProcessMessage(pid, packet);

	switch (pid)
	{
	case MerryMessage::ID_MERRY_INPUT:
		{
			RakNet::BitStream bs(packet->data, packet->length, false);

			unsigned char uid;

			bs.IgnoreBytes(1);
			bs.Read(uid);

			InputModule::InputState input;
			input.Read(bs);

			Player* player				= (*m_players)[uid];
			CharacterEntity* character	= player->GetCharacter();	

			// read and save input
			glm::vec3 dir = character->GetDirection();
			input.WriteDirection(dir);

			// set movement direction and rotation
			character->SetDirection(MathHelper::Normalize(dir));
			character->SetRotation(glm::vec3(character->GetRotation().x, input.angle, character->GetRotation().z));

			bool sprintPressed	= (input.m_buttonStates[InputModule::ControlElement::Sprint] == InputModule::BtnState::DOWN);
			bool shootPressed	= (input.m_buttonStates[InputModule::ControlElement::Shoot] == InputModule::BtnState::DOWN);
			bool aimPressed		= (input.m_buttonStates[InputModule::ControlElement::Aim] == InputModule::BtnState::DOWN);

			// sprint if not shooting
			if (sprintPressed && !shootPressed) {

				character->HandleSprintCommand();

				// set the timer to default value if the sprint button was released and pressed again
				if (character->GetSprintInterruptionTimer() > -1.0f && !character->IsSprintInterrupted()) {
					character->SetSprintInterruptionTimer(-1.0f);
				}
			}

			// aiming
			if (aimPressed) {
					
				if (!player->GetCharacter()->IsAimingInterrupted()) {

					character->SetAiming(true);

					// stop sprinting
					if (character->IsSprinting()) {
						
						character->SetSprinting(false);
						character->SetSprintInterrupted(true);
					}
				}
			}
			else {

				character->SetAiming(false);

				// let player sptint again
				if (character->IsSprintInterrupted() && character->GetSprintInterruptionTimer() < 0.0f && !character->GetActiveWeapon()->IsReloading()) {
					character->SetSprintInterrupted(false);
				}

				if (character->IsAimingInterrupted()) {
					character->SetAimingInterrupted(false);
				}
			}

			// shooting
			if (shootPressed) {
				character->HandleShootCommand();
			}
			else {
				character->SetShooting(false);
			}

			if (!sprintPressed) {

				character->SetSprinting(false);

				// reset the sprintIterrupted flag, now player can sprint again
				if (character->IsSprintInterrupted()) {
					character->SetSprintInterrupted(false);
				}

				// player can aim again
				if (character->IsAimingInterrupted()) {
					character->SetAimingInterrupted(false);
				}
			}
		}
		break;

	case MerryMessage::ID_MERRY_PREGAME_SETUP_REQUEST:
		{
			std::cout << "Pregame setup request received" << std::endl;
			std::cout << "Sending answer..." << std::endl;
			
			// save name, give uid, etc.
			RakNet::BitStream bs(packet->data, packet->length, false);
			RakNet::RakString name;
			// teamTag is ignored at this moment
			char teamTag;
			char charClass;

			bs.IgnoreBytes(1);
			bs.Read(name);
			bs.Read(teamTag);
			bs.Read(charClass);

			// server decides which team new player joins
			Player* player = m_gw->AddPlayer(name, charClass, GetNextTeamTag(), packet->systemAddress);

			if (!player)
			{
				std::cout << "Couldn't create player" << std::endl;
				break;
			}

			// has to be called before calling any Send... methods
			m_engine->SaveClientAddress(player->GetUID(), packet->systemAddress);
			m_engine->UpdatePingResponse();

			std::cout << name.C_String() << " was given " << (unsigned)player->GetUID() << " UID" << std::endl; 

			// send replies back to the player
			m_gw->SendPregameSetup(player);
			m_gw->SendPregameUpdate(player);
			// notify other players
			m_gw->SendNewPlayerConnected(player);
		}
		break;

	case MerryMessage::ID_MERRY_PREGAME_UPDATE_PROCESSED:
		{
			std::cout << "Pregame update message processed" << std::endl;

			RakNet::BitStream bs(packet->data, packet->length, false);
			bs.IgnoreBytes(1);

			unsigned char uid;
			bs.Read(uid);

			(*m_players)[uid]->GetState().SetPhase(PlayerState::GamePhase::INGAME);
			(*m_players)[uid]->GetState().SetStatus(PlayerState::IngameStatus::ALIVE);
		}
		break;

	case MerryMessage::ID_MERRY_INGAME_CHANGE_WEAPON:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			unsigned char uid;
			WeaponID wid;

			bsIn.IgnoreBytes(1);
			bsIn.Read(uid);
			bsIn.Read(wid);

			Player* player = (*m_players)[uid];

			if (player->GetState().GetStatus() == PlayerState::IngameStatus::ALIVE)
			{
				player->GetCharacter()->SetActiveWeapon(wid);
			}

			m_gw->SendWeaponChanged(player);
		}
		break;

	case MerryMessage::ID_MERRY_INGAME_RELOAD:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			unsigned char uid;

			bsIn.IgnoreBytes(1);
			bsIn.Read(uid);

			Player* player = (*m_players)[uid];

			if (player->GetState().GetStatus() == PlayerState::IngameStatus::ALIVE)
			{
				player->GetCharacter()->GetActiveWeapon()->Reload();
			}
		}
		break;


	default:
		break;
	}
}

char TDMGameMode::GetNextTeamTag()
{
	int forceBalance = 0;
	for (const auto player : m_players->GetAllSet())
	{
		forceBalance += player->GetTeamTag() == (char)TeamTag::TEAM_A ? 1 : -1;
	}
	return forceBalance > 0 ? (char)TeamTag::TEAM_B : (char)TeamTag::TEAM_A;
}

RakNet::BitStream* TDMGameMode::WritePregameSetup(RakNet::BitStream* bs, Player* player)														
{
	WrapperGameMode::WritePregameSetup(bs, player);
	
	player->WritePregameSetup(*bs);
	return bs;
}

RakNet::BitStream* TDMGameMode::WritePregameUpdate(RakNet::BitStream* bs, Player* player)														
{
	WrapperGameMode::WritePregameUpdate(bs, player);

	// get set of ingame players
	PlayerSet allSet = m_players->GetAllSet();
	
	// amount of ingame players
	// 101715
	// -1 cause we aren't sending client's local player
	bs->Write((unsigned char)(allSet.size() - 1));
	
	for (const auto pl : allSet)
	{
		// 101715
		// adding check to ignore local player for that client in pregame update
		if (player->GetUID() == pl->GetUID())
		{
			continue;
		}

		pl->WritePregameUpdate(*bs);
	}

	return bs;
}

RakNet::BitStream* TDMGameMode::WriteIngameUpdate(RakNet::BitStream* bs, Player* player)																			
{
	WrapperGameMode::WriteIngameUpdate(bs, player);

	// get set of alive players
	PlayerSet aliveSet = m_players->GetByStatusSet(PlayerState::IngameStatus::ALIVE);
	// amount of alive characters
	bs->Write((unsigned char)aliveSet.size());
	for (const auto pl : aliveSet)
	{
		pl->WriteIngameUpdate(*bs); 
	}

	return bs;
}

RakNet::BitStream* TDMGameMode::WriteNewPlayerConnected(RakNet::BitStream* bs, Player* player)														
{
	WrapperGameMode::WriteNewPlayerConnected(bs, player);
	
	player->WriteNewPlayerConnected(*bs);
	return bs;
}

RakNet::BitStream* TDMGameMode::WritePlayerHit(RakNet::BitStream* bs, Player* victim, Player* shooter)							
{
	WrapperGameMode::WritePlayerHit(bs, victim, shooter);

	bs->Write(shooter->GetUID());
	bs->Write(victim->GetUID());
	// new health amount
	bs->WriteFloat16(victim->GetCharacter()->GetCharacterStats().HP, 0.0f, 255.0f);
	bs->WriteFloat16(victim->GetCharacter()->GetCharacterStats().Armour, 0.0f, 255.0f);

	return bs;
}

RakNet::BitStream* TDMGameMode::WritePlayerKilled(RakNet::BitStream* bs, Player* victim, Player* killer)	
{
	WrapperGameMode::WritePlayerKilled(bs, victim, killer);

	bs->Write(killer->GetUID());
	bs->Write(victim->GetUID());
	// how scores have been changed for both players
	bs->Write(killer->GetKills());
	// killing spree value
	bs->Write(killer->GetKillingSpreeCounter());
	bs->Write(victim->GetDeaths());

	return bs;
}

RakNet::BitStream* TDMGameMode::WritePlayerStatsChanged(RakNet::BitStream* bs, Player* player)															
{
	WrapperGameMode::WritePlayerStatsChanged(bs, player);
	
	player->WriteStatsChanged(*bs);
	return bs;
}

RakNet::BitStream* TDMGameMode::WriteBuffApplied(RakNet::BitStream* bs, Player* player, const merrymen::Effect& effect)
{
	WrapperGameMode::WriteBuffApplied(bs, player, effect);

	bs->Write(player->GetUID());
	bs->Write(effect.Type);
	bs->WriteFloat16(effect.Duration, 0.0f, 100000.0f);

	return bs;
}

RakNet::BitStream* TDMGameMode::WriteRespawn(RakNet::BitStream* bs, Player* player)															
{
	WrapperGameMode::WriteRespawn(bs, player);
	
	player->WriteRespawn(*bs);
	return bs;
}

RakNet::BitStream* TDMGameMode::WriteReload(RakNet::BitStream* bs, Player* player)															
{
	WrapperGameMode::WriteReload(bs, player);
	
	bs->Write(player->GetUID());
	return bs;
}

RakNet::BitStream* TDMGameMode::WriteWeaponChanged(RakNet::BitStream* bs, Player* player)
{
	WrapperGameMode::WriteWeaponChanged(bs, player);
	
	bs->Write(player->GetUID());
	bs->Write(player->GetCharacter()->GetActiveWeaponID());

	return bs;
}

RakNet::BitStream* TDMGameMode::WriteObjectPicked(RakNet::BitStream* bs, Player* player, unsigned char objectType) {

	WrapperGameMode::WriteObjectPicked(bs, player, objectType);
	
	bs->Write(player->GetUID());
	bs->Write(objectType);

	return bs;
}