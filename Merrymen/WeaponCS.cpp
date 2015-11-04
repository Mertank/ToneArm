/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/15/2014 4:14:48 PM
------------------------------------------------------------------------------------------
*/

#include "WeaponCS.h"
#include "Character.h"
#include "GameScene.h"
#include "ShootingHelperSP.h"

#include "../ToneArmEngine/RayCasting.h"
#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/EngineModule.h"
#include "../ToneArmEngine/CachedResourceLoader.h"
#include "../ToneArmEngine/InitializationFileResource.h"
#include "../ToneArmEngine/SoundModule.h"
#include "../ToneArmEngine/ParticleEmitterNode.h"
#include "../ToneArmEngine/ParticleEmitterNodePool.h"

using namespace merrymen;

//
// allocates a Weapon object and calls Init() on it
//
WeaponCS* WeaponCS::Create(const WeaponID& id, Character* const owner, const char* modelFile, char** const soundFiles, const char* iniFile, const char* category) {

	WeaponCS* weapon = new WeaponCS();

	if (weapon && weapon->Init(id, owner, modelFile, soundFiles, iniFile, category)) {
		return weapon;
	}

	delete weapon;

	return nullptr;
}

//
// default constructor
//
WeaponCS::WeaponCS() :
	m_owner(nullptr),
	m_model(nullptr),
	m_emitter(nullptr),
	m_stats(nullptr),
	m_projectileStats(nullptr),
	m_cooldownTimer(1.0f),
	m_reloadTimer(-1.0f),
	m_active(false),
	m_shooting(false),
	m_reloading(false),
	m_emitterNodePool( NULL )
{}

//
// destructor
//
WeaponCS::~WeaponCS() {

	delete m_stats;
	delete m_projectileStats;
}

//
// initializes the weapon data with the passed values
//
bool WeaponCS::Init(const WeaponID& id, Character* const owner, const char* modelFile, char** soundFiles, const char* iniFile, const char* category) {

	if (Node::Init()) {
		
		m_id = id;
		m_owner = owner;
		
		// initialize weapon's stats
		CachedResourceLoader* loader = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>(EngineModuleType::RESOURCELOADER);
		std::shared_ptr<InitializationFileResource> ini = loader->LoadResource<InitializationFileResource>(iniFile);

		m_stats = new WeaponStats<WeaponCS>(this, *ini->GetCategory(category));

		m_projectileStats = new ProjectileStats();

		// initialize sounds
		m_sounds.insert(std::pair<WeaponSoundTypes, const char*>(WeaponSoundTypes::ShotSound, soundFiles[0]));
		m_sounds.insert(std::pair<WeaponSoundTypes, const char*>(WeaponSoundTypes::LoadSound, soundFiles[1]));
		m_sounds.insert(std::pair<WeaponSoundTypes, const char*>(WeaponSoundTypes::CockSound, soundFiles[2]));
		m_sounds.insert(std::pair<WeaponSoundTypes, const char*>(WeaponSoundTypes::TriggerClickSound, soundFiles[3]));

		// create model
		m_model = ModelNode::Create( modelFile );
		m_model->SetVisible(false);
		m_model->SetPosition(glm::vec3(0.0f, 15.0f, 50.0f));
		AddChild(m_model);

		// TODO: initialize the emitter with respect to the type of the weapon
		m_emitter = vgs::ParticleEmitterNode::CreateWithSettings(ParticleEmitterSettings(75, 0.1f, 0.05f, 750, 90.0f, glm::vec3(0.85f, 0.75f, 0.0f), 0, 5.0f, 300.0f, 100.0f));

		if (id == WeaponID::Pistol) {
			m_emitter->SetPosition( glm::vec3( -0.898f, 0.3085f, -0.3122f ) * 35.0f);
		}
		else if (id == WeaponID::Shotgun) {
			m_emitter->SetPosition( glm::vec3( -0.898f, 0.3085f, -0.3122f ) * 70.0f);
		}
		else if (id == WeaponID::SniperRifle) {
			m_emitter->SetPosition( glm::vec3( -0.898f, 0.3085f, -0.3122f ) * 85.0f);
		}

		m_emitter->SetRotation( glm::vec3( 67.0f, -116.0f, -83.0f ) );

		m_model->AddChild(m_emitter);

		m_emitterNodePool = ParticleEmitterNodePool::CreateEmitterPool( 10 );
		AddChild( m_emitterNodePool );

		return true;
	}

	return false;
}

//
// Weapon object updates it's state
//
void WeaponCS::Update(float dt) {

	if (!m_owner->IsOnline() && IsActive()) {
		UpdateStats(dt);
	}

	// cooldown goes on
	if (m_cooldownTimer > 0.0f) {
		m_cooldownTimer -= dt;
	}

	// reloading timer
	if (IsReloading()) {
		m_reloadTimer -= dt;
	}

	// auto-reload?
	if (m_stats->Ammo <= 0 && m_reloadTimer <= -1.0f && !m_owner->IsSprinting()) {
		Reload();
	}

	Node::Update(dt);
}

//
// weapon updates it's timers and stats
//
void WeaponCS::UpdateStats(float dt) {

	if (m_stats->Update(dt)) {
		m_owner->SetStatsChanged(true);
	}
}

//
// casts rays to check which objects were hit
//
void WeaponCS::DoRayCasting(std::multimap<ClientEntity*, float>& hitEntities, std::vector<glm::vec3>& impactPoints) {

	RayCasting::ColliderResult shotResultPtr;
	glm::vec3 shotImpactPoint;

	// prepare data for ray casting
	std::vector<ColliderComponent*> geometryColliders;

	ShootingHelperSP helper = ShootingHelperSP();
	helper.PreapareGeometryData(this->GetOwner(), geometryColliders, false);

	GameScene* gs = static_cast<GameScene*>(Engine::GetRunningScene());
	std::vector<Character*> targets = gs->GetPlayers();

	glm::vec3 shotOrigin = this->GetAbsoluteWorldPosition() + m_owner->GetForward() * WEAPON_LENGTH, shotDirection;

	// cast as many rays as there are projectiles in one shot
	for (int i = 0; i < m_projectileStats->ProjectilesPerShot; i++) {

		// calculate shot direction for that particular projectile
		glm::vec3 shotDirection = helper.CalculateShotDirection(this->GetOwner(), this->GetWeaponStats());

		if (targets.size() > 1) {
			// cast a ray to each target
			for (auto target : targets) {

				bool hitSomething = false;
				bool hitTheTarget = false;

				// check what the ray cast from the weapon's position actually hits (target, geometry or nothing)
				if (target != this->GetOwner() && !target->IsDead()) {

					ColliderComponent* collider = target->GetComponentOfType<ColliderComponent>();
					shotResultPtr = RayCasting::RayCastShot(shotOrigin, shotDirection, geometryColliders, collider);

					if (shotResultPtr) {

						ColliderComponent* collider = shotResultPtr.get()->Object;

						// hit someone
						if (collider->GetOwner()->GetRTTI().DerivesFrom(ClientEntity::rtti)) {
							hitEntities.insert(std::pair<ClientEntity*, float>(target, glm::distance(shotOrigin, shotImpactPoint)));
						}
						// hit a wall
						else {
							impactPoints.push_back(shotResultPtr.get()->Intersection);
						}
					}
				}
			}
		}
		else {
			std::shared_ptr<RayCasting::RayCastResult<ColliderComponent>> hitObject = RayCasting::RayCastFirst(geometryColliders, shotOrigin, shotDirection);

			if (hitObject.get()) {
				impactPoints.push_back(hitObject.get()->Intersection);
			}
		}
	}

	geometryColliders.clear();
	targets.clear();
}

//
// method that causes damage
//
void WeaponCS::Shoot() {

	std::multimap<ClientEntity*, float> hitEntities;
	std::vector<glm::vec3> impactPoints;

	// cast rays to check which objects were hit
	DoRayCasting(hitEntities, impactPoints);

	// deal damage to the characters which were hit
	if (hitEntities.size() > 0) {

		for (std::multimap<ClientEntity*, float>::iterator hitEntitiesItr = hitEntities.begin(); hitEntitiesItr != hitEntities.end(); ++hitEntitiesItr) {

			int count = hitEntities.count(hitEntitiesItr->first);

			Damage dmg = Damage::CalculateDamage(count, this->GetWeaponStats().Damage, hitEntitiesItr->second, this->GetWeaponStats().AimingDistance, this->GetProjectileStats());
			hitEntitiesItr->first->TakeDamage(dmg, this->GetProjectileStats().Type);

			std::multimap<ClientEntity*, float>::iterator currentItr = hitEntitiesItr;
			std::advance(hitEntitiesItr, count - 1);
			hitEntities.erase(currentItr, hitEntitiesItr);
		}
	}

	// play shot impact effects on the geometry hit by the shot
	for (auto impactPoint : impactPoints) {
		PlayShotImpactEffects(impactPoint);
	}

	hitEntities.clear();
	impactPoints.clear();
}

//
// handles a shooting command received from the owner
//
void WeaponCS::HandleShootCommand() {

	bool canShoot = false;

	if (m_stats->ShootingType == ShootingType::Automatic) {

		if (m_stats->Ammo > 0 && m_cooldownTimer <= 0.0f && (!IsReloading() || m_stats->LoadingUnit == LoadingUnit::Bullet)) {
			canShoot = true;
		}
	}
	// if a weapon is semi-automatic we need to make sure that the fire button was released at least once since the moment of the most recent shot
	else if (m_stats->ShootingType == ShootingType::Semi_automatic) {

		if (!IsShooting() && m_stats->Ammo > 0 && m_cooldownTimer <= 0.0f && (!IsReloading() || m_stats->LoadingUnit == LoadingUnit::Bullet)) {
			canShoot = true;
		}
	}

	// if we have enough ammo, the cooldown has passed and the weapon is not being reloaded a new shot can be performed
	if (canShoot) {

		Shoot();

		// stop reloading
		if (IsReloading()) {
			StopReloading();
		}

		m_stats->Ammo--;
		m_owner->SetStatsChanged(true);

		// start cooldown
		m_cooldownTimer = m_stats->Cooldown;

		PlayShootingEffects();

		m_owner->PlayShootAnimation();
	}
	else if (m_stats->Ammo == 0 && m_cooldownTimer <= 0.0f && !IsShooting()) {

		m_cooldownTimer = m_stats->Cooldown;

		if (!IsReloading()) {
			PlayWeaponSound(WeaponSoundTypes::TriggerClickSound);
		}
	}
}

//
// method that handles reloading commands from the owner
//
void WeaponCS::Reload() {

	if (!IsReloading() && m_stats->Ammo < m_stats->ClipSize && m_stats->TotalAmmo > 0) {
		
		int bulletsToLoad = 1;

		// set the reload timer to a proper value
		if (m_stats->LoadingUnit == LoadingUnit::Clip) {
			m_reloadTimer = m_stats->ReloadTime;
		}
		else if (m_stats->LoadingUnit == LoadingUnit::Bullet) {

			bulletsToLoad = m_stats->TotalAmmo >= m_stats->ClipSize - m_stats->Ammo ? m_stats->ClipSize - m_stats->Ammo : m_stats->TotalAmmo;
			m_reloadTimer = (m_stats->ReloadTime / m_stats->ClipSize) * bulletsToLoad;
		}

		// stop aiming
		if (m_owner->IsAiming()) {
			m_owner->SetAiming(false);
		}

		// give the player some feedback
		PlayWeaponSound(WeaponSoundTypes::LoadSound);

		m_owner->PlayReloadAnimation(bulletsToLoad);

		m_reloading = true;
	}
}

//
// describes what needs to happen once the reloading process is finished
//
void WeaponCS::FinishReloading() {

	m_reloadTimer = -1.0f;
	m_reloading = false;
	m_owner->SetStatsChanged(true);

	// let the player sprint again if the sprint button was held all the time during reloading
	if (m_owner->IsSprintInterrupted()) {
		m_owner->SetSprintInterrupted(false);
	}

	// play sound
	PlayWeaponSound(WeaponSoundTypes::CockSound);

	// play animation
	m_owner->PlayCockAnimation();
}

//
// interrupts reloading
//
void WeaponCS::StopReloading() {

	m_reloadTimer = -1.0f;

	// play some sounds
	SoundModule* sMod = Engine::GetInstance()->GetModuleByType<SoundModule>(EngineModuleType::SOUNDER);
	if (sMod)
	{
		sMod->StopUniqueSound(m_sounds.at(WeaponSoundTypes::LoadSound));
	}

	SetReloading(false);
}

//
// stops/plays reloading sound depending on whether or not hte weapon is still active (sets the m_active value too of course)
//
void WeaponCS::SetActive(const bool active) {

	if (IsReloading()) {

		// stop reloading sound and restart the timer
		if (!active) {
			StopReloading();
		}
		// play the reload sound, the reloading will start automatically in the UpdateStats
		else {
			PlayWeaponSound(WeaponSoundTypes::LoadSound);
		}
	}

	m_active = active;
}

//
// plays particle effects on the weapon
//
void WeaponCS::PlayShootingEffects() {

	if(m_model->IsVisible()) {
		m_emitter->Emit(true, 0);
	}

	PlayWeaponSound(WeaponSoundTypes::ShotSound);
}

//
// plays particle effects on the projectile impact point
//
void WeaponCS::PlayShotImpactEffects() {

	std::multimap<ClientEntity*, float> hitEntities;
	std::vector<glm::vec3> impactPoints;

	// cast rays to check which objects were hit
	DoRayCasting(hitEntities, impactPoints);

	// play shot impact effects on the geometry hit by the shot
	for (auto impactPoint : impactPoints) {
		PlayShotImpactEffects(impactPoint);
	}

	hitEntities.clear();
	impactPoints.clear();
}

//
// plays particle effects on the projectile impact point
//
void WeaponCS::PlayShotImpactEffects(const glm::vec3& impactPoint) {

	// get an emitter
	ParticleEmitterNode* emitter = m_emitterNodePool->GetEmitterNode();

	// set its parent properly
	GameScene* gs = static_cast<GameScene*>(Engine::GetRunningScene());

	if (gs) {

		Node* parent = emitter->GetParent();

		if (parent && parent != gs) {
			emitter->RemoveFromParent();
		}

		gs->AddChild(emitter);
	}

	// make the emitter look "pretty"
	emitter->SetEmitterSettings(ParticleEmitterSettings(300, 0.1f, 0.05f, 3000, 180.0f, glm::vec3(0.33f, 0.33f, 0.33f), 0, 4.0f, 200.0f, 50.0f));

	// set position of the emitter
	emitter->SetPosition(impactPoint - m_owner->GetForward());

	// rotate the emitter
	glm::vec3 direction = m_owner->GetAbsoluteWorldPosition() - impactPoint;
	direction.y = 0.0f;
	
	emitter->SetDirection( glm::normalize( direction ) );

	emitter->Emit(true, 0);
}

//
// plays the sound of the shot or the sound of a trigger click if the clip is empty
//
void WeaponCS::PlayWeaponSound(const WeaponSoundTypes& soundType) {

	// play sound
	GameScene* gs = static_cast<GameScene*>(Engine::GetRunningScene());

	if (gs) {

		Character* localCharacter = gs->GetLocalCharacter();
		if (!localCharacter)
		{
			return;
		}

		float distance = glm::distance(GetAbsoluteWorldPosition(), localCharacter->GetAbsoluteWorldPosition());

		float volume = SoundModule::CalculateVolume(distance);

		SoundModule* sMod = Engine::GetInstance()->GetModuleByType<SoundModule>(EngineModuleType::SOUNDER);

		if (!sMod)
		{
			std::cout << "Couldn't find sound module in Weapon::PlayWeaponSound" << std::endl;
			return;
		}

		if (soundType == WeaponSoundTypes::ShotSound) {
			sMod->PlaySound(m_sounds.at(soundType), volume);
		}
		else if (GetOwner() == localCharacter) {

			// if reloading sound needs to be played the duration might need to be adjusted
			if (soundType == WeaponSoundTypes::LoadSound) {

				if (m_stats->LoadingUnit == LoadingUnit::Clip) {
					sMod->PlaySound(m_sounds.at(soundType), volume);
				}
				else if (m_stats->LoadingUnit == LoadingUnit::Bullet) {

					// adjust the duration of the sound with respect to the number of projectiles that need to be loaded
					unsigned short bulletsToLoad = m_stats->TotalAmmo >= m_stats->ClipSize - m_stats->Ammo ? m_stats->ClipSize - m_stats->Ammo : m_stats->TotalAmmo;
					float duration = (m_stats->ReloadTime / m_stats->ClipSize) * bulletsToLoad;

					sMod->PlaySound(m_sounds.at(soundType), volume, duration);
				}
			}
			// there's no need in playing more than one sound of this type at the same time, so PlayUniqueSound()
			else if (soundType == WeaponSoundTypes::TriggerClickSound) {
				sMod->PlayUniqueSound(m_sounds.at(soundType), volume);
			}
			else if (soundType == WeaponSoundTypes::CockSound) {

				sMod->StopUniqueSound(m_sounds.at(WeaponSoundTypes::LoadSound));
				sMod->PlayUniqueSound(m_sounds.at(soundType), volume);
			}
		}
	}
}

//
// changes visibility of the weapon's model
//
void WeaponCS::SetVisible(const bool value) {

	if (m_model) {
		m_model->SetVisible(value);
	}
}

//
// reinitializes some member variables
//
void WeaponCS::Reset() {

	m_stats->Ammo = m_stats->ClipSize;
	m_stats->TotalAmmo = m_stats->MaxAmmo;
	m_cooldownTimer = -1.0f;
	m_reloadTimer = -1.0f;
	
	m_shooting = false;
	m_reloading = false;

	m_owner->SetStatsChanged(true);
}