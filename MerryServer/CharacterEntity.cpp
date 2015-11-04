/*
========================

Copyright (c) 2014 Vinyl Games Studio

	CharacterEntity.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 5/28/2014 3:18:13 PM

========================
*/
#include "CharacterEntity.h"
#include "ServerEngine.h"
#include "PickableBuffSS.h"
#include "GameMode.h"
#include "Player.h"
#include "CharacterController.h"
#include "Level.h"
#include "GameWorld.h"

#include <Effect.h>
#include <ProjectileStats.h>

#include "../ToneArmEngine/InitializationFileResource.h"
#include "../ToneArmEngine/CylinderCollider.h"

using namespace merrymen;

IMPLEMENT_RTTI(CharacterEntity, ServerEntity);

CharacterEntity::CharacterEntity(unsigned char characterClass, CharacterController* owner, const char* iniFile)
	: m_stats(nullptr)
	, m_direction(glm::vec3(0.0f))
	, m_shooting(false)
	, m_aiming(false)
	, m_aimingInterrupted(false)
	, m_staminaRecharging(false)
	, m_moving(false)
	, m_sprinting(false)
	, m_sprintInterrupted(false)
	, m_sprintInterruptionTimer(-1.0f)
	, m_activeWeapon(nullptr)
	, m_owner(owner)
{
	if (m_owner)
	{
		// FIXME
		m_ownerUid = static_cast<Player*>(m_owner)->GetUID();
		SetOwner(m_owner);
	}

	CylinderCollider* collider = new CylinderCollider(this, GetPosition() + glm::vec3(0.0f, CHARACTER_HEIGHT / 2, 0.0f), 35.0f, CHARACTER_HEIGHT, true);
	AddComponent(collider);

	// load initialization files
	std::shared_ptr<InitializationFileResource> charactersIni	= ServerEngine::GetInstance()
		->GetModuleByType<CachedResourceLoader>(EngineModuleType::RESOURCELOADER)->LoadResource<InitializationFileResource>(iniFile);
	std::shared_ptr<InitializationFileResource> ammoIni	= ServerEngine::GetInstance()
		->GetModuleByType<CachedResourceLoader>(EngineModuleType::RESOURCELOADER)->LoadResource<InitializationFileResource>("Data/Projectiles.ini");

	// initialize player entity with values from a proper file
	switch(characterClass) {

	case CharacterClass::TANK:

		m_stats = new CharacterStats<CharacterEntity>(this, *charactersIni->GetCategory("Tank"));

		m_weapons[WeaponID::Pistol]		= WeaponSS::Create(this, "Data/Weapons.ini", "Pistol");
		m_weapons[WeaponID::Shotgun]	= WeaponSS::Create(this, "Data/Weapons.ini", "Shotgun");

		m_weapons[WeaponID::Pistol]->SetProjectileStats(ProjectileStats(merrymen::ProjectileType::HollowPointBullet, *ammoIni->GetCategory("Hollow_Point_Bullet")));
		m_weapons[WeaponID::Shotgun]->SetProjectileStats(ProjectileStats(merrymen::ProjectileType::Shot, *ammoIni->GetCategory("Shot")));

		SetActiveWeapon(WeaponID::Shotgun);

		break;

	case CharacterClass::SNIPER:
		
		m_stats = new CharacterStats<CharacterEntity>(this, *charactersIni->GetCategory("Sniper"));

		m_weapons[WeaponID::Pistol]			= WeaponSS::Create(this, "Data/Weapons.ini", "Pistol");
		m_weapons[WeaponID::SniperRifle]	= WeaponSS::Create(this, "Data/Weapons.ini", "Sniper_Rifle");

		m_weapons[WeaponID::Pistol]->SetProjectileStats(ProjectileStats(merrymen::ProjectileType::HollowPointBullet, *ammoIni->GetCategory("Hollow_Point_Bullet")));
		m_weapons[WeaponID::SniperRifle]->SetProjectileStats(ProjectileStats(merrymen::ProjectileType::HollowPointBullet, *ammoIni->GetCategory("Hollow_Point_Bullet")));

		SetActiveWeapon(WeaponID::SniperRifle);

		break;

	default:
		break;
	}
}

CharacterEntity::~CharacterEntity()
{
	WeaponMap::iterator weaponItr = m_weapons.begin();
	while (weaponItr != m_weapons.end())
	{
		delete weaponItr->second;
		weaponItr =	m_weapons.erase(weaponItr);
	}

	delete m_stats;
}

//
// method that applies damage of different types to the character's stats
//
void CharacterEntity::TakeDamage(const merrymen::Damage& damage, merrymen::ProjectileType projectileType, GameObject* shooter)
{
	// deal direct damage to character's stats
	// damage to armour
	m_stats->Armour = std::max(0.0f, m_stats->Armour - damage.ArmourDamage());

	// damage to hp
	if (m_stats->Armour <= 0 ||
		projectileType == merrymen::ProjectileType::ArmourPiercingBullet || projectileType == merrymen::ProjectileType::Shot) {
			
			m_stats->HP = std::max(0.0f, m_stats->HP - damage.HealthDamage());

			if (m_stats->HP <= 0.0f) {
				Die(shooter);
			}
	}
}

//
// handles character's death
//
void CharacterEntity::Die(GameObject* killer) {

	// FIXME
	Player* killerPlayer = static_cast<Player*>((static_cast<CharacterEntity*>(killer))->GetOwner());
	Player* killedPlayer = static_cast<Player*>(this->GetOwner());

	// update killer's statistics
	++(killerPlayer->GetKills());
	++(killerPlayer->GetKillingSpreeCounter());

	// update killed player's statistics
	++(killedPlayer->GetDeaths());
	killedPlayer->GetKillingSpreeCounter() = 0;
	killedPlayer->SetMustDie();
	m_stats->HP = 0.0f;

	// send a proper message
	GameWorld* gw = ServerEngine::GetInstance()->GetGameWorld();
	gw->SendPlayerKilled(killedPlayer, killerPlayer);

	// drop ammo kit
	Effect* ammo = new Effect(merrymen::EffectType::ExtraAmmo, merrymen::EffectReason::PickableItem, GetActiveWeapon()->GetWeaponStats().MaxAmmo * FRACTION_OF_AMMO_DROPPED);
	PickableBuffSS* ammoPack = new PickableBuffSS(ammo, GetPosition(), 50.0f);

	ServerEngine::GetInstance()->GetGameWorld()->GetLevel()->AddPickableObject(ammoPack);
}

void CharacterEntity::Respawn()
{
	m_stats->HP		 = m_stats->MaxHP;
	m_stats->Stamina = m_stats->MaxStamina;
	m_stats->Armour	 = m_stats->MaxArmour;
	m_shooting		 = false;
	m_aiming		 = false;
	m_sprinting		 = false;

	for (auto weapon : m_weapons) {
		weapon.second->Reset();
	}

	m_staminaRecharging = false;
}

//
// applies different types of buffs to entity's stats
//
void CharacterEntity::ApplyEffect(const Effect& effect,  CharacterEntity* const author) {
	
	// TODO: handle all types of buffs
	switch (effect.Type) {

	case merrymen::EffectType::ExtraAmmo:

		if (m_activeWeapon) {
			m_activeWeapon->GetWeaponStats().ApplyEffect(effect, author);
		}

		break;

	default:
		break;
	}

	// send a proper message to all clients
	GameWorld* gw = ServerEngine::GetInstance()->GetGameWorld();
	gw->SendBuffApplied(static_cast<Player*>(this->GetOwner()), effect);
}

//
// updates player's stats: stamina. hp, etc.
//
bool CharacterEntity::UpdateStats(float dt) {

	ServerEntity::Update(dt);

	bool statsChanged = m_stats->Update(dt);

	// player has used all of the character's stamina and has to be punished
	if (m_stats->Stamina < 0.1f) {

		m_sprinting = false;
		m_staminaRecharging = true;
	}
	else if (IsStaminaRecharging() && m_stats->Stamina > m_stats->MaxStamina / 2.0f) {
		m_staminaRecharging = false;
	}
	
	// check if the character is still alive
	if (m_stats->HP <= 0.0f) {
		Die();
	}

	// decrease sprint interruption timer - when it goes to zero the character can shoot
	if (m_sprintInterruptionTimer > 0.0f) {
		m_sprintInterruptionTimer -= dt;
	}
	
	if (m_sprintInterruptionTimer <= 0.0f && m_sprintInterruptionTimer > -1.0f) {

		m_sprintInterruptionTimer = -1.0f;
		SetSprintInterrupted(false);

		SetShooting(true);
	}

	return statsChanged;
}

//
// aiming flag setter
//
void CharacterEntity::SetAiming(const bool value) {

	if (value != m_aiming && !GetActiveWeapon()->IsReloading() && !IsAimingInterrupted()) {
		m_aiming = value;
	}
}

//
// sets the active weapon and interrupts reloading (if needed)
//
void CharacterEntity::SetActiveWeapon(WeaponID wid) {

	if (m_activeWeapon != m_weapons[wid] && !IsAiming()) {

		// stop reloading
		if (m_activeWeapon && m_activeWeapon->IsReloading()) {
			m_activeWeapon->StopReloading();
		}

		m_activeWeapon = m_weapons[wid];
	}
}

WeaponID CharacterEntity::GetActiveWeaponID() const
{
	for (const auto& w : m_weapons)
	{
		if (w.second == m_activeWeapon)
		{
			return w.first;
		}
	}
	return m_weapons.begin()->first;
}

//
// deals with the sprint command coming from the client
//
void CharacterEntity::HandleSprintCommand() {

	// make sure the character is moving and not already sprinting
	if (IsMoving() && !IsSprinting()) {

		// check if sprint was not interrupted (by shooting, aiming, etc.) and the character has enough stamina
		if (!IsSprintInterrupted() && !IsStaminaRecharging() && m_stats->Stamina > 0.0f) {

			// finally make sure that the character is moving forward
			if (glm::dot(GetForward(), GetDirection()) < (1.0f - MOVEMENT_ANGLE_INTERVAL)) {
				return;
			}
			else {
				SetSprinting(true);
			}

			// stop reloading
			if (m_activeWeapon->IsReloading()) {
				m_activeWeapon->StopReloading();
			}

			// stop aiming
			if (IsAiming()) {

				SetAiming(false);
				SetAimingInterrupted(true);
			}
		}
	}
}

//
// deals with the shoot command coming from the client
//
void CharacterEntity::HandleShootCommand() {

	if (GetActiveWeapon()) {

		// interrupt the sprint if a shooting command was received
		if (IsSprinting()) {
			
			SetSprinting(false);
			SetSprintInterrupted(true);

			m_sprintInterruptionTimer = SHOOT_AFTER_SPRINT_DELAY;
		}
		else if (m_sprintInterruptionTimer <= 0.0f) {
			SetShooting(true);
		}
	}
}

