 /*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/15/2014 3:22:41 PM
------------------------------------------------------------------------------------------
*/

#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "../ToneArmEngine/RTTI.h"

#include "ClientEntity.h"
#include "WeaponCS.h"

#include <CharacterStats.h>

#include <map>

namespace merrymen
{

struct Effect;

/*
------------------------------------------------------------------------------------------
	Character

	Base class for all playable characters in the game (client side).
------------------------------------------------------------------------------------------
*/

using namespace vgs;

enum class TeamTag : char
{
	NONE = -1,
	TEAM_A,
	TEAM_B
};

enum class CharacterAnimationTypes : unsigned char {

	Idle,
	WalkForwardPrimary,
	WalkForwardSecondary,
	WalkForwardAimPrimary,
	WalkForwardAimSecondary,
	WalkBackwardPrimary,
	WalkBackwardSecondary,
	WalkBackwardAimPrimary,
	WalkBackwardAimSecondary,
	WalkRightPrimary,
	WalkRightSecondary,
	WalkRightAimPrimary,
	WalkRightAimSecondary,
	WalkLeftPrimary,
	WalkLeftSecondary,
	WalkLeftAimPrimary,
	WalkLeftAimSecondary,
	AimPrimary,
	AimSecondary,
	ShootPrimaryHip,
	ShootPrimaryAim,
	ShootSecondaryHip,
	ShootSecondaryAim,
	ReloadPrimary,
	ReloadSecondary,
	CockPrimary,
	CockSecondary,
	ChangeWeapon,
	RunPrimary,
	RunSecondary,
	Die
};

typedef std::map<CharacterAnimationTypes, unsigned long> AnimationMap;

class Character : public ClientEntity {

	DECLARE_RTTI;

	typedef std::unordered_map<WeaponID, WeaponCS*> WeaponMap;

public:
	static Character*			Create(const char* modelFile, const char* iniFile, const char* categoryName, bool online = true);

	virtual						~Character();

	virtual bool				Init(const char* modelFile, const char* iniFile, const char* categoryName, bool online);

	virtual void				Update(float dt);
	void						UpdateStats(float dt);

	// actions
	void						Move(float dt) override;
	void						TakeDamage(const Damage& damage, ProjectileType projectileType, GameObject* shooter = nullptr) override;
	void						Die(GameObject* killer = nullptr) override;
	void						Respawn();
	void						Shoot();
	void						Reload();

	// input handling
	virtual void				HandleKeyboardInput(float dt);
	virtual void				HandleMouseInput();

	void						ApplyEffect(const Effect& effect, Character* const author = nullptr);

	// visual effects
	void						UpdateVisuallEffects(float dt);
	void						PlayTakingDamageEffects();
	void						FOVCheck();
	void						SetVisible(bool value);
	void						ShowEffectApplied(EffectType effect, float duration) {};

	// animations
	void						CacheAnimationRelatedValues();
	void						PlayMovementAnimation();
	void						PlayShootAnimation();
	void						PlayReloadAnimation(int loopCount);
	void						PlayCockAnimation();
	void						PlayDeathAnimation();
	void						PlayChangeWeaponAnimation();
	
	// accessors
	// weapons
	WeaponMap&					GetWeapons()												{ return m_weapons; }

	void						AddWeapon(WeaponID wid, WeaponCS* const weapon);
	void						RemoveWeapon(WeaponCS* weapon);					

	WeaponCS*					GetActiveWeapon() const										{ return m_activeWeapon; }
	virtual void				SetActiveWeapon(WeaponID wid);

	// stats
	CharacterStats<Character>&	GetCharacterStats()											{ return *m_stats; }
	void						SetCharacterStats(const CharacterStats<Character>& stats)	{ *m_stats = stats; }

	const bool					StatsChanged()												{ return m_statsChanged; }
	void						SetStatsChanged(bool value)									{ m_statsChanged = value; }

	float						GetViewHeight()												{ return m_viewHeight; }

	// network
	void						SetTeamTag(char val)										{ m_teamTag = val; }
	char						GetTeamTag() const											{ return m_teamTag; }

	void						SetKills(unsigned char val)									{ m_kills = val; }
	unsigned char&				GetKills()													{ return m_kills; }

	void						SetDeaths(unsigned char val)								{ m_deaths = val; }
	unsigned char&				GetDeaths()													{ return m_deaths; }

	// movement
	void						SetPosition(const glm::vec3& position) override;

	const bool					IsMoving() const											{ return m_moving; }
	void						SetMoving(const bool moving)								{ m_moving = moving; }

	const bool					WasMoving() const											{ return m_wasMoving; }
	void						SetWasMoving(const bool moving)								{ m_wasMoving = moving; }

	const glm::vec3&			GetMovementDirection() const								{ return m_movementDirection; }
	void						SetMovementDirection(const glm::vec3& direction)			{ m_movementDirection = direction; }

	const glm::vec3&			GetPreviousMovementDirection() const						{ return m_previousMovementDirection; }
	void						SetPreviousMovementDirection(const glm::vec3& direction)	{ m_previousMovementDirection = direction; }

	const glm::vec3&			GetPreviousForward() const									{ return m_previousForward; }
	void						SetPreviousForward(const glm::vec3& forward)				{ m_previousForward = forward; } 

	const bool					IsAiming() const											{ return m_aiming; }
	virtual void				SetAiming(const bool aiming);

	const bool					IsAimingInterrupted() const									{ return m_aimingInterrupted; }
	void						SetAimingInterrupted(const bool value)						{ m_aimingInterrupted = value; }

	const bool					WasAiming() const											{ return m_wasAiming; }
	void						SetWasAiming(const bool aiming)								{ m_wasAiming = aiming; }

	const bool					IsSprinting() const											{ return m_sprinting; }
	void						SetSprinting(const bool sprinting);

	const bool					IsSprintInterrupted() const									{ return m_sprintInterrupted; }
	void						SetSprintInterrupted(const bool value)						{ m_sprintInterrupted = value; }

	const bool					WasSprinting() const										{ return m_wasSprinting; }
	void						SetWasSprinting(const bool sprinting)						{ m_wasSprinting = sprinting; }

	const bool					IsStaminaRecharging() const									{ return m_staminaRecharging; }
	void						SetStaminaRecharging(const bool recharging)					{ m_staminaRecharging = recharging; }

	// other
	const bool					IsDead() const												{ return m_dead; }
	void						SetDead(const bool dead)									{ m_dead = dead; }

	const bool					InFOV() const												{ return m_inFOV; }
	void						SetInFOV(const bool value);

public:
	static AnimationMap			CharacterAnimations;

protected:
								Character();

	WeaponMap					m_weapons;
	WeaponCS*					m_activeWeapon;

	CharacterStats<Character>*	m_stats;
	bool						m_statsChanged;

	// movement
	glm::vec3					m_movementDirection;
	glm::vec3					m_previousMovementDirection;
	glm::vec3					m_previousForward;

	bool						m_moving;
	bool						m_wasMoving;
	bool						m_sprinting;
	bool						m_wasSprinting;
	bool						m_sprintInterrupted;
	float						m_sprintInterruptionTimer;
	bool						m_aiming;
	bool						m_wasAiming;
	bool						m_aimingInterrupted;
	bool						m_staminaRecharging;

	bool						m_dead;
	float						m_deathTimer;

	float						m_viewHeight;
	bool						m_inFOV;

	// network stuff
	char						m_teamTag;
	unsigned char				m_kills;
	unsigned char				m_deaths;

	ParticleEmitterNodePool*	m_emitterPool;
};

} // namespace vgs

#endif __CHARACTER_H__