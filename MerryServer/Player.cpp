/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Player.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 9/8/2014 12:37:43 PM

========================
*/
#include "Player.h"
#include "CharacterEntity.h"
#include "ServerEngine.h"
#include "PickableBuffSS.h"
#include "Level.h"

#include <PickableItem.h>

#include <BitStream.h>

using namespace vgs;

SharedPlayerState Player::s_sharedState;

void PlayerState::SetPhase(PlayerState::GamePhase ph)
{
	m_phase = ph;
	Player::GetSharedState().phaseChanged = true;
}

void PlayerState::SetStatus(PlayerState::IngameStatus st)
{
	m_status = st;
	Player::GetSharedState().statusChanged = true;
}

Player::Player(unsigned char uid, const RakNet::RakString& name, char characterClass, char team, const RakNet::SystemAddress& address)
	: CharacterController(characterClass)
	, m_uid(uid)
	, m_name(name)
	, m_class(characterClass)
	, m_team(team)
	, m_kills(0)
	, m_deaths(0)
	, m_killingSpreeCounter(0)
	, m_clientAddress(address)
	, m_lastUpdateSent(0)
	, m_mustDie(false)
	, m_respawnCounter(0)
{
	s_sharedState.SetAll(true);
}

Player::~Player()
{
	s_sharedState.SetAll(true);
}

void Player::WritePregameSetup(RakNet::BitStream& bsOut) const
{
	// uid
	bsOut.Write(m_uid);

	bsOut.Write(m_team);
	bsOut.Write(m_class);
	bsOut.Write(m_name);

	// initial position
	bsOut.WriteFloat16(m_character->GetPosition().x, -5000.f, +5000.f);
	bsOut.WriteFloat16(m_character->GetPosition().y, -5000.f, +5000.f);
	bsOut.WriteFloat16(m_character->GetPosition().z, -5000.f, +5000.f);
}

void Player::WritePregameUpdate(RakNet::BitStream& bsOut) const
{
	// uid
	bsOut.Write(m_uid);
	// player name
	bsOut.Write(m_name);

	bsOut.Write(m_team);
	bsOut.Write(m_class);
	bsOut.Write(m_kills);
	bsOut.Write(m_deaths);

	// holding weapon's id
	bsOut.Write(m_character->GetWeapons().begin()->first);

	// initial position
	bsOut.WriteFloat16(m_character->GetPosition().x, -5000.f, +5000.f);
	bsOut.WriteFloat16(m_character->GetPosition().y, -5000.f, +5000.f);
	bsOut.WriteFloat16(m_character->GetPosition().z, -5000.f, +5000.f);
}

void Player::WriteIngameUpdate(RakNet::BitStream& bsOut) const
{
	// uid
	bsOut.Write(m_uid);
	// position
	bsOut.WriteFloat16(m_character->GetPosition().x, -5000.f, +5000.f);
	bsOut.WriteFloat16(m_character->GetPosition().z, -5000.f, +5000.f);
	// rotation
	bsOut.WriteFloat16(m_character->GetRotation().y, -360.f, +360.f);

	// sprinting
	if (m_character->IsSprinting())
	{
		bsOut.Write1();
	}
	else
	{
		bsOut.Write0();
	}

	// aiming
	if (m_character->IsAiming()) 
	{
		bsOut.Write1();
	}
	else
	{
		bsOut.Write0();
	}

	// shooting bit
	if (m_character->IsShooting())
	{
		bsOut.Write1();
	}
	else
	{
		bsOut.Write0();
	}

	// send new ammo amount
	if (m_character->GetActiveWeapon()->IsDirty())
	{
		bsOut.Write1();
		bsOut.Write(m_character->GetActiveWeapon()->GetWeaponStats().Ammo);
	}
	else
	{
		bsOut.Write0();
	}
}

void Player::WriteNewPlayerConnected(RakNet::BitStream& bsOut) const
{
	// uid
	bsOut.Write(m_uid);

	bsOut.Write(m_name);
	bsOut.Write(m_team);
	bsOut.Write(m_class);

	// new position
	bsOut.WriteFloat16(m_character->GetPosition().x, -5000.f, +5000.f);
	bsOut.WriteFloat16(m_character->GetPosition().y, -5000.f, +5000.f);
	bsOut.WriteFloat16(m_character->GetPosition().z, -5000.f, +5000.f);
}

void Player::WriteRespawn(RakNet::BitStream& bsOut) const
{
	bsOut.Write(m_uid);
	// starting values
	bsOut.WriteFloat16(m_character->GetCharacterStats().MaxHP, 0.0f, 255.0f);
	bsOut.WriteFloat16(m_character->GetCharacterStats().MaxStamina, 0.0f, 255.0f);
	bsOut.WriteFloat16(m_character->GetCharacterStats().MaxArmour, 0.0f, 255.0f);
}

void Player::WriteStatsChanged(RakNet::BitStream& bsOut) const
{
	bsOut.Write(m_uid);
	// current values
	bsOut.WriteFloat16(m_character->GetCharacterStats().HP, 0.0f, 255.0f);
	bsOut.WriteFloat16(m_character->GetCharacterStats().Stamina, 0.0f, 255.0f);
	bsOut.WriteFloat16(m_character->GetCharacterStats().Armour, 0.0f, 255.0f);
}