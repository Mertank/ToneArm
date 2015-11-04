/*
========================

Copyright (c) 2014 Vinyl Games Studio

	WeaponEntity.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 6/16/2014 2:27:04 PM

========================
*/
#include "WeaponSS.h"
#include "CharacterEntity.h"
#include "Player.h"
#include "ServerEngine.h"
#include "GameWorld.h"

#include "../ToneArmEngine/CachedResourceLoader.h"
#include "../ToneArmEngine/InitializationFileResource.h"
#include "../ToneArmEngine/RayCasting.h"
#include "../ToneArmEngine/ColliderComponent.h"

using namespace vgs;
using namespace merrymen;

WeaponSS::WeaponSS() 
	: m_stats(new WeaponStats<WeaponSS>())
	, m_projectileStats(new ProjectileStats())
	, m_prevAmmo(0)
	, m_cooldownTimer(0.0f)
	, m_reloadTimer(-1.0f)
	, m_shooting(false)
	, m_reloading(false)
{
}

//
// creates a Weapon object
//
WeaponSS* WeaponSS::Create(CharacterEntity* const owner, const char* iniFile, const char* categoryName) {

	WeaponSS* weapon = new WeaponSS();

	if (weapon && weapon->Init(owner, iniFile, categoryName)) {
		return weapon;
	}

	delete weapon;

	return nullptr;
}

//
// destructor
//
WeaponSS::~WeaponSS() {

	delete m_stats;
	delete m_projectileStats;
}

//
// initializes a weapon with the values from a specified .ini file
//
bool WeaponSS::Init(CharacterEntity* const owner, const char* iniFile, const char* categoryName) {

	m_owner = owner;

	std::shared_ptr<InitializationFileResource> ini = ServerEngine::GetInstance()
		->GetModuleByType<CachedResourceLoader>(EngineModuleType::RESOURCELOADER)->LoadResource<InitializationFileResource>(iniFile);

	m_stats = new WeaponStats<WeaponSS>(this, *ini->GetCategory(categoryName));
	m_prevAmmo = m_stats->Ammo;

	return true;
}

//
// updates the state of the weapon
//
void WeaponSS::Update(float dt)
{
	m_prevAmmo = m_stats->Ammo;

	UpdateStats(dt);

	// cooldown goes on
	if (m_cooldownTimer > 0.0f)  {
		m_cooldownTimer -= dt;
	}

	// reloading timer
	if (IsReloading()) {
		m_reloadTimer -= dt;
	}

	// auto-reload
	if (!m_owner->IsSprinting() && m_stats->Ammo == 0 && m_reloadTimer <= -1.0f) {
		Reload();
	}
}

//
// updates stats of the weapon
//
void WeaponSS::UpdateStats(float dt) {

	if (m_stats->Update(dt)) {
		m_owner->SetStatsChanged(true);
	}
}

void WeaponSS::RunRaycastChecks(const glm::vec3& from, const glm::vec3& direction, const std::vector<ColliderComponent*>& geometryColliders,
									const std::vector<CharacterEntity*>& targets, std::multimap<CharacterEntity*, float>& targetsHit)
{
	RayCasting::ColliderResult shotResultPtr;

	RayCasting::Ray shotRay = RayCasting::Ray(from, direction);

	// cast a ray to each target
	for (auto target : targets) {

		// check what the ray cast from the weapon's position actually hits (target, geometry or nothing)
		shotResultPtr = RayCasting::RayCastShot(from, direction, geometryColliders, target->GetComponentOfType<ColliderComponent>());

		if (shotResultPtr) {

			ColliderComponent* collider = shotResultPtr.get()->Object;

			// hit someone
			if (collider->GetOwner()->GetRTTI().DerivesFrom(ServerEntity::rtti)) {
				targetsHit.insert(std::pair<CharacterEntity*, float>(target, glm::distance(shotRay.Origin, shotResultPtr.get()->Intersection)));
			}
		}
	}
}

bool WeaponSS::Shoot() 
{
	bool canShoot = false;

	// check if this weapon can fire a projectile
	if (m_stats->ShootingType == ShootingType::Automatic) {

		if (m_stats->Ammo > 0 && m_cooldownTimer <= 0.0f && (!IsReloading() || m_stats->LoadingUnit == LoadingUnit::Bullet)) {
			canShoot = true;
		}
	}
	else if (m_stats->ShootingType == ShootingType::Semi_automatic) {

		if (!m_shooting && m_stats->Ammo > 0 && m_cooldownTimer <= 0.0f && (!IsReloading() || m_stats->LoadingUnit == LoadingUnit::Bullet)) {
			canShoot = true;
		}
	}

	// if we have enough ammo, the cooldown has passed and the weapon is not being reloaded a new shot can be performed
	if (canShoot) {

		// start cooldown
		m_cooldownTimer = m_stats->Cooldown;
		m_stats->Ammo--;
		m_shooting = true;

		// stop reloading
		if (IsReloading()) {
			StopReloading();
		}
	}

	return canShoot;
}

//
// handles reload command
//
void WeaponSS::Reload() 
{
	// start reload timer and return true if possible, otherwise return false
	if (!IsReloading() && m_stats->Ammo < m_stats->ClipSize && m_stats->TotalAmmo > 0) {
		
		m_reloading = true;

		if (m_stats->LoadingUnit == LoadingUnit::Clip) {
			m_reloadTimer = m_stats->ReloadTime;
		}
		else if (m_stats->LoadingUnit == LoadingUnit::Bullet) {

			unsigned short bulletsToLoad = m_stats->TotalAmmo >= m_stats->ClipSize - m_stats->Ammo ? m_stats->ClipSize - m_stats->Ammo : m_stats->TotalAmmo;
			m_reloadTimer = (m_stats->ReloadTime / m_stats->ClipSize) * bulletsToLoad;
		}

		// stop aiming
		if (m_owner->IsAiming()) {
			m_owner->SetAiming(false);
		}

		// stop sprinting
		if (m_owner->IsSprinting()) {
		
			m_owner->SetSprinting(false);
			m_owner->SetSprintInterrupted(true);
		}

		// send a proper message to the clients
		GameWorld* gw = ServerEngine::GetInstance()->GetGameWorld();
		gw->SendReload(static_cast<Player*>(this->GetOwner()->GetOwner()));
	}
}

//
// describes what needs to happen once the reloading process is finished
//
void WeaponSS::FinishReloading() {

	m_reloadTimer = -1.0f;
	m_reloading = false;

	// let the player sprint again if the sprint button was held all the time during reloading
	if (m_owner->IsSprintInterrupted()) {
		m_owner->SetSprintInterrupted(false);
	}

	// TODO: find out if I need to send a special message
}

//
// interrupts reloading
//
void WeaponSS::StopReloading() {

	// reset timer and boolean
	//if (m_stats->LoadingUnit == LoadingUnit::Clip) {
	//	m_reloadTimer = m_stats->ReloadTime;
	//}
	//else if (m_stats->LoadingUnit == LoadingUnit::Bullet) {
		m_reloadTimer = -1.0f;
	//}

	SetReloading(false);
}

//
// sets total ammo amount (makes sure it does not exceed maximum value)
//
void WeaponSS::SetTotalAmmo(const unsigned short ammo) {

	if (ammo <= m_stats->MaxAmmo) {
		m_stats->TotalAmmo = ammo;
	}
	else {
		m_stats->TotalAmmo = m_stats->MaxAmmo;
	}
}

//
// reinitializes some member variables
//
void WeaponSS::Reset() {

	m_stats->Ammo = m_stats->ClipSize;
	m_stats->TotalAmmo = m_stats->MaxAmmo;
	m_prevAmmo = m_stats->Ammo;
	m_cooldownTimer = -1.0f;
	m_reloadTimer = -1.0f;
	
	m_shooting = false;
	m_reloading = false;
}