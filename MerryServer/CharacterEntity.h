/*
========================

Copyright (c) 2014 Vinyl Games Studio

	CharacterEntity.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/28/2014 3:18:13 PM

========================
*/
#ifndef __CHARACTER_ENTITY_H__
#define __CHARACTER_ENTITY_H__

#include "ServerEntity.h"
#include "WeaponSS.h"

#include <CharacterStats.h>

#include <map>

using namespace merrymen;

namespace merrymen {
struct Effect;
}

namespace vgs
{
	class Player;
	class CharacterController;

	/*
	========================

		CharacterEntity

			Character analog on a client.

			Created by: Vladyslav Dolhopolov
			Created on: 5/28/2014 3:18:13 PM

	========================
	*/
	class CharacterEntity : public ServerEntity
	{
		typedef std::map<WeaponID, WeaponSS*> WeaponMap;

		DECLARE_RTTI;

	public:
											CharacterEntity() {};
											CharacterEntity(unsigned char characterClass, CharacterController* owner = nullptr,
															const char* iniFile = "Data/Characters.ini");

		virtual								~CharacterEntity()	override;

		// actions
		bool								UpdateStats(float dt);
		void								Move(float dt) override {};
		void								TakeDamage(const merrymen::Damage& damage, merrymen::ProjectileType projectileType, GameObject* shooter = nullptr) override;
		void								Die(GameObject* killer = nullptr) override;
		void								Respawn();

		// weapons
		WeaponSS*							GetActiveWeapon()													{ return m_activeWeapon; }
		WeaponID							GetActiveWeaponID() const;
		void								SetActiveWeapon(WeaponID wid);

		const WeaponMap&					GetWeapons() const													{ return m_weapons; }

		// stats
		CharacterStats<CharacterEntity>&	GetCharacterStats()													{ return *m_stats; }
		void								SetCharacterStats(const CharacterStats<CharacterEntity>& stats)		{ *m_stats = stats; }

		const bool							StatsChanged()														{ return m_statsChanged; }
		void								SetStatsChanged(bool value)											{ m_statsChanged = value; }

		void								ApplyEffect(const Effect& effect, CharacterEntity* const author = nullptr);

		// movement
		const bool							IsMoving() const													{ return m_moving; }
		void								SetMoving(const bool moving)										{ m_moving = moving; }

		const bool							IsSprinting() const													{ return m_sprinting; }
		void								SetSprinting(const bool sprinting)									{ m_sprinting = sprinting; }
		void								HandleSprintCommand();

		const bool							IsSprintInterrupted() const											{ return m_sprintInterrupted; }
		void								SetSprintInterrupted(const bool value)								{ m_sprintInterrupted = value; }

		const float							GetSprintInterruptionTimer() const									{ return m_sprintInterruptionTimer; }
		void								SetSprintInterruptionTimer(const float timer)						{ m_sprintInterruptionTimer = timer; }

		const bool							IsStaminaRecharging() const											{ return m_staminaRecharging; }
		void								SetStaminaRecharging(const bool recharging)							{ m_staminaRecharging = recharging; }

		void								SetAiming(bool val);
		bool								IsAiming() const													{ return m_aiming; }

		const bool							IsAimingInterrupted() const											{ return m_aimingInterrupted; }
		void								SetAimingInterrupted(const bool value)								{ m_aimingInterrupted = value; }

		glm::vec3&							GetDirection()														{ return m_direction; }
		void								SetDirection(const glm::vec3& val)									{ m_direction = val; }

		void								SetShooting(bool val)												{ m_shooting = val; }
		bool								IsShooting() const													{ return m_shooting; }
		void								HandleShootCommand();

		CharacterController* const			GetOwner() const													{ return m_owner; }
		void								SetOwner(CharacterController* player)								{ m_owner = player; }

	private:
		glm::vec3							m_direction;

		CharacterStats<CharacterEntity>*	m_stats;
		bool								m_statsChanged;

		WeaponMap							m_weapons;
		WeaponSS*							m_activeWeapon;

		bool								m_shooting;
		bool								m_moving;
		bool								m_sprinting;
		bool								m_sprintInterrupted;
		float								m_sprintInterruptionTimer;
		bool								m_aiming;
		bool								m_aimingInterrupted;
		bool								m_staminaRecharging;

		CharacterController*				m_owner;
	};

} // namespace vgs

#endif __CHARACTER_ENTITY_H__