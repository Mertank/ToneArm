/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/15/2014 4:01:13 PM
------------------------------------------------------------------------------------------
*/

#ifndef __CHARACTER_STATS_H__
#define __CHARACTER_STATS_H__

#include "Stats.h"
#include "Effect.h"

#include "../ToneArmEngine/InitializationCategory.h"

#include <algorithm>

#define SHOOT_AFTER_SPRINT_DELAY		0.75f
#define	RESPAWN_TIMEOUT					2.0f
#define FRACTION_OF_AMMO_DROPPED		0.25f
#define	MOVEMENT_ANGLE_INTERVAL			0.2f
#define CHARACTER_HEIGHT				150.0f
#define FOV_CHANGE_SPEED				175.0f
#define MINIMUM_SPEED_PERCENTAGE		20.0f

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	CharacterStats

	Struct that stores stats of in-game characters.
------------------------------------------------------------------------------------------
*/

enum class CharacterClass : char
{
	NONE = -1,
	TANK,
	SNIPER,
	ASSASSIN,
	ASSAULT,
	TECHNICIAN
};

template <class T>
struct CharacterStats : public Stats {

	float				HP;
	float				MaxHP;
	float				Stamina;
	float				MaxStamina;
	float				Armour;
	float				MaxArmour;
	float				DamageAbsorbtion;
	float				WalkSpeed;
	float				WalkSpeedAiming;
	float				SprintSpeed;
	float				FOVAngle;
	float				DefaultFOVAngle;
	float				HPRegenerationSpeed;
	float				StaminaUsageSpeed;
	float				StaminaRegenerationSpeed;
	float				HeadshotAngle;

	T*					Owner;

	// constructors
						CharacterStats();
						CharacterStats(T* const owner, const vgs::InitializationCategory& category);

	bool				Update(float dt);
	bool				UpdateEffects(float dt) override;

	void				ApplyEffect(const Effect& effect, vgs::GameObject* const author = nullptr) override;
	void				UnapplyEffect(Effect* const effect, vgs::GameObject* const author = nullptr) override;

	CharacterStats&		operator=(const CharacterStats& other);
};

//
// default constructor
//
template <class T>
CharacterStats<T>::CharacterStats() :
	Owner(nullptr),
	HP(0.0f),
	MaxHP(0.0f),
	Stamina(0.0f),
	MaxStamina(0.0f),
	Armour(0.0f),
	MaxArmour(0.0f),
	WalkSpeed(0.0f),
	WalkSpeedAiming(0.0f),
	SprintSpeed(0.0f),
	FOVAngle(0.0f),
	DefaultFOVAngle(0.0f),
	HPRegenerationSpeed(0.0f),
	StaminaUsageSpeed(0.0f),
	StaminaRegenerationSpeed(0.0f),
	HeadshotAngle(0.0f)
{}

//
// parametrized constructor for reading values from a .ini file
//
template <class T>
CharacterStats<T>::CharacterStats(T* const owner, const vgs::InitializationCategory& category) :
	Owner(owner)
{	
	HP = MaxHP					= category.GetFloatValueForKey("hp");
	Stamina	= MaxStamina		= category.GetFloatValueForKey("stamina");
	Armour = MaxArmour			= category.GetFloatValueForKey("armour");
	DamageAbsorbtion			= category.GetFloatValueForKey("damage_absorbtion");
	WalkSpeed					= category.GetFloatValueForKey("walk_speed");
	WalkSpeedAiming				= category.GetFloatValueForKey("walk_speed_aiming");
	SprintSpeed					= category.GetFloatValueForKey("sprint_speed");
	FOVAngle = DefaultFOVAngle	= category.GetFloatValueForKey("fov_angle");
	HPRegenerationSpeed			= category.GetFloatValueForKey("hp_regeneration_speed");
	StaminaUsageSpeed			= category.GetFloatValueForKey("stamina_usage_speed");
	StaminaRegenerationSpeed	= category.GetFloatValueForKey("stamina_regeneration_speed");
	HeadshotAngle				= category.GetFloatValueForKey("head_size");
}

//
// overloaded assignment operator
//
template <class T>
CharacterStats<T>& CharacterStats<T>::operator=(const CharacterStats<T>& other) {

	if (this != &other) {
		
		Owner						= other.Owner;
		HP							= other.HP;
		MaxHP						= other.MaxHP;
		Stamina						= other.Stamina;
		MaxStamina					= other.MaxStamina;
		Armour						= other.Armour;
		MaxArmour					= other.MaxArmour;
		DamageAbsorbtion			= other.DamageAbsorbtion;
		WalkSpeed					= other.WalkSpeed;
		WalkSpeedAiming				= other.WalkSpeedAiming;
		SprintSpeed					= other.SprintSpeed;
		FOVAngle					= other.FOVAngle;
		DefaultFOVAngle				= other.DefaultFOVAngle;
		HPRegenerationSpeed			= other.HPRegenerationSpeed;
		StaminaUsageSpeed			= other.StaminaUsageSpeed;
		StaminaRegenerationSpeed	= other.StaminaRegenerationSpeed;
		HeadshotAngle				= other.HeadshotAngle;
	}

	return *this;
}

//
// updates the stats 
//
template <class T>
bool CharacterStats<T>::Update(float dt) {

	// update effects applied to the stats
	bool statsChanged = UpdateEffects(dt);

	// TODO: implement 3 sec. timer (3 seconds without taking damage to start recovering health)
	// health regeneration
	if (HP < MaxHP) {

		HP = std::min(MaxHP, HP + HPRegenerationSpeed * dt);
		statsChanged = true;
	}

	// stamina usage/regeneration
	if (Owner->IsMoving()) {

		// using stamina
		if (Owner->IsSprinting()) {

			Stamina = std::max(0.0f, Stamina - StaminaUsageSpeed * dt);
			statsChanged = true;
		}
		// recovering stamina
		else if (!Owner->IsSprinting() && Stamina < MaxStamina) {

			Stamina = std::min(MaxStamina, Stamina + StaminaUsageSpeed * dt);
			statsChanged = true;
		}
	}
	// recovering stamina at a maximum rate
	else if (Stamina < MaxStamina) {

		Stamina = std::min(MaxStamina, Stamina + StaminaUsageSpeed * 2.0f * dt);
		statsChanged = true;
	}

	return statsChanged;
}

//
// updates the effects applied to the stats
//
template <class T>
bool CharacterStats<T>::UpdateEffects(float dt) {

	bool statsChanged = false;

	// update applied effects
	for (auto effect : AppliedEffects) {

		// chech if the effect timer has reached zero and this effect needs to be unapplied
		if (effect.first->Update(dt)) {

			UnapplyEffect(effect.first, effect.second);
			continue;
		}

		// handle specific cases
		switch (effect.first->Type) {

		case EffectType::Burning:

			if (Armour >= 0.0f) {
				Armour -= effect.first->Value * dt;
			}
			else {
				HP -= effect.first->Value * dt;
			}

			if (HP <= 0.0f) {
				Owner->Die(effect.second);
			}

			statsChanged = true;

		default:
			break;
		}
	}

	return statsChanged;
}

//
// determines the way a buff/debuff is applied to the stats
//
template <class T>
void CharacterStats<T>::ApplyEffect(const Effect& effect, vgs::GameObject* const author) {

	SpecificEffectType effectType = SpecificEffectType(effect.Type, effect.Reason);

	// check whether an effect of that type can be applied to the stats or not
	if (CountSimilarAppliedEffects(effect) < Effect::EffectLimits[effectType]) {

		Stats::ApplyEffect(effect, author);

		// apply effect
		switch (effect.Type) {

		case EffectType::SpeedBuff:

			// TODO: make sure the new values are not less than 20% of the corresponding basic values
			WalkSpeed *= 1.0f + effect.Value;
			WalkSpeedAiming *= 1.0f + effect.Value;
			SprintSpeed *= 1.0f + effect.Value;

			break;

		default:
			break;
		}
	}
}

//
// determines waht happens when the buff timer expires
//
template <class T>
void CharacterStats<T>::UnapplyEffect(Effect* const effect, vgs::GameObject* const author) {

	// unapply the effect
	switch (effect->Type) {

	case EffectType::SpeedBuff:

		WalkSpeed /= 1.0f - effect->Value;
		WalkSpeedAiming /= 1.0f - effect->Value;
		SprintSpeed /= 1.0f - effect->Value;

		break;

	default:
		break;
	}

	Stats::UnapplyEffect(effect);
}

}
#endif