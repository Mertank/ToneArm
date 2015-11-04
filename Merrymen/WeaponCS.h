/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/15/2014 4:14:48 PM
------------------------------------------------------------------------------------------
*/

#ifndef __WEAPON_CS_H__
#define __WEAPON_CS_H__

#include "../ToneArmEngine/ModelNode.h"

#include <WeaponStats.h>
#include <ProjectileStats.h>

namespace vgs {
class ParticleEmitterNode;
class ParticleEmitterNodePool;
}

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	Weapon

	A customizable class for all weapons in the game.
------------------------------------------------------------------------------------------
*/

class Character;
class ClientEntity;

using namespace vgs;

//
// types of sounds that a weapon can make
//
enum class WeaponSoundTypes : unsigned char {
	
	ShotSound,
	LoadSound,
	CockSound,
	TriggerClickSound
};

typedef std::map<WeaponSoundTypes, const char*>	WeaponSoundMap;

class WeaponCS : public Node {

	friend struct WeaponStats<WeaponCS>;

public:

	static WeaponCS*			Create(const WeaponID& id, Character* const owner, const char* modelFile, char** const soundFiles,
									   const char* iniFile, const char* category);

								~WeaponCS();

	bool						Init(const WeaponID& id, Character* const owner, const char* modelFile, char** soundFiles,
									 const char* iniFile, const char* category);

	void						Update(float dt);
	void						UpdateStats(float dt);

	// shooting
	void						HandleShootCommand();
	void						Shoot();
	void						DoRayCasting(std::multimap<ClientEntity*, float>& targetsHit, std::vector<glm::vec3>& impactPoints);

	// reloading
	void						Reload();
	void						FinishReloading();
	void						StopReloading();

	// effects
	void						PlayShootingEffects();
	void						PlayShotImpactEffects();
	void						PlayShotImpactEffects(const glm::vec3& impactPoint);
	void						PlayWeaponSound(const WeaponSoundTypes& soundType);

	void						SetVisible(const bool visible);

	// accessors
	Character*					GetOwner() const									{ return m_owner; }
	void						SetOwner(Character* const owner)					{ m_owner = owner; }

	ModelNode* const			GetModel() const									{ return m_model; }
	void						SetModel(ModelNode* const model)					{ m_model = model;
																					  AddChild(model); }

	WeaponStats<WeaponCS>&		GetWeaponStats()									{ return *m_stats; }
	void						SetWeaponStats(const WeaponStats<WeaponCS>& stats)	{ *m_stats = stats; }

	ProjectileStats&			GetProjectileStats()								{ return *m_projectileStats; }
	void						SetProjectileStats(const ProjectileStats& stats)	{ *m_projectileStats = stats; }

	const bool					IsActive() const									{ return m_active; }
	void						SetActive(const bool active);

	const bool					IsShooting() const									{ return m_shooting; }
	void						SetShooting(const bool shooting)					{ m_shooting = shooting; }

	const bool					IsReloading() const									{ return m_reloading; }
	void						SetReloading(const bool reloading)					{ m_reloading = reloading; }

	const WeaponSoundMap&		GetSounds() const									{ return m_sounds; }

	void						Reset();

private:
								WeaponCS();

	WeaponID					m_id;
	Character*					m_owner;
	ModelNode*					m_model;

	ParticleEmitterNode*		m_emitter;
	ParticleEmitterNodePool*	m_emitterNodePool;

	WeaponStats<WeaponCS>*		m_stats;
	ProjectileStats*			m_projectileStats;
	float						m_cooldownTimer;
	float						m_reloadTimer;

	bool						m_active;
	bool						m_shooting;
	bool						m_reloading;
	WeaponSoundMap				m_sounds;
};

} // namespace merrymen

#endif __WEAPON_CS_H__