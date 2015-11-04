/*
========================

Copyright (c) 2014 Vinyl Games Studio

	WeaponEntity.h

		Created by: Vladyslav Dolhopolov
		Created on: 6/16/2014 2:27:04 PM

========================
*/
#ifndef __WEAPON_SS_H__
#define __WEAPON_SS_H__

#include <WeaponStats.h>
#include <ProjectileStats.h>

#include <map>
#include <vector>

using namespace merrymen;

namespace vgs
{
	class ColliderComponent;
	class CharacterEntity;

	//
	// distance to the object that was shot and its id
	//
	typedef std::pair<float, unsigned char> ShotResultMP;

	/*
	========================

		WeaponEntity

			Server side weapon class.

			Created by: Vladyslav Dolhopolov
			Created on: 6/16/2014 2:27:04 PM

	========================
	*/
	class WeaponSS
	{

		friend struct WeaponStats<WeaponSS>;

	public:
		static WeaponSS*			Create(CharacterEntity* const owner, const char* iniFile, const char* category);

									~WeaponSS();

		bool						Init(CharacterEntity* const owner, const char* iniFile, const char* category);
		
		void						Update(float dt);
		void						UpdateStats(float dt);

		void						RunRaycastChecks(const glm::vec3& from, const glm::vec3& direction, const std::vector<ColliderComponent*>& geometryColliders,
													 const std::vector<CharacterEntity*>& targets, std::multimap<CharacterEntity*, float>& targetsHit);

		bool						Shoot();
		void						Reload();
		void						FinishReloading();
		void						StopReloading();

		CharacterEntity* const		GetOwner() const									{ return m_owner; }
		void						SetOwner(CharacterEntity* const owner)				{ m_owner = owner; }

		WeaponStats<WeaponSS>&		GetWeaponStats()									{ return *m_stats; }
		void						SetWeaponStats(const WeaponStats<WeaponSS>& stats)	{ *m_stats = stats; }

		ProjectileStats&			GetProjectileStats()								{ return *m_projectileStats; }
		void						SetProjectileStats(const ProjectileStats& stats)	{ *m_projectileStats = stats; }

		void						SetTotalAmmo(const unsigned short ammo);

		const bool					IsShooting() const									{ return m_shooting; }
		void						SetShooting(const bool shooting)					{ m_shooting = shooting; }

		const bool					IsReloading() const									{ return m_reloading; }
		void						SetReloading(const bool reloading)					{ m_reloading = reloading; }

		bool						IsDirty()											{ return m_prevAmmo != m_stats->Ammo; }
		//void						ResetDirty()										{ m_prevAmmo = m_stats->Ammo; }

		void						Reset();

	private:
									WeaponSS();

	protected:
		CharacterEntity*			m_owner;

		WeaponStats<WeaponSS>*		m_stats;
		ProjectileStats*			m_projectileStats;
		unsigned short				m_prevAmmo;
		float						m_cooldownTimer;
		float						m_reloadTimer;

		bool						m_shooting;
		bool						m_reloading;
	};

} // namespace vgs

#endif __WEAPON_SS_H__