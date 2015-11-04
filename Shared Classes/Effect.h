/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/18/2014 4:51:20 PM
------------------------------------------------------------------------------------------
*/

#ifndef __EFFECT_H__
#define __EFFECT_H__

#include "../ToneArmEngine/RTTI.h"

#include <map>

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	OneTimeEffect

	Class for storing various in-game buffs applied istantly and only once.
------------------------------------------------------------------------------------------
*/

class PickableBuffCS;
class PickableBuffSS;

enum class EffectType : unsigned char {

	ExtraHP,
	ExtraArmour,
	ExtraStamina,
	ExtraAmmo,
	Healing,
	Poisoning,
	Bleeding,
	Burning,
	Stun,
	Immobility,
	Invisibility,
	SpeedBuff,
	RangeDamageBuff,
	MeleeDamageBuff,
	FireRateBuff,
	ReloadTimeBuff,
	AccuracyBuff
};

enum class EffectReason : unsigned char {

	PassiveAbility,
	ActiveAbility,
	DroneAbility,
	WeaponAttachment,
	Gear,
	MeleeAttack,
	RangeAttack,
	ZoneEffectGround,
	ZoneEffectAir,
	PickableItem
};

typedef std::pair<EffectType, EffectReason> SpecificEffectType;
typedef std::map<SpecificEffectType, unsigned short> EffectsLimitTable;

struct Effect {

	DECLARE_RTTI;

	EffectType					Type;
	EffectReason				Reason;
	float						Value;
	float						Duration;

	Effect(EffectType type, EffectReason reason, float value, float duration = 0.0f);
	~Effect() {};

	bool						Update(float dt);

	bool						operator<(const Effect* const other);

	static EffectsLimitTable	EffectLimits;
	static EffectsLimitTable	CreateEffectsLimitTable();

protected:
	Effect();
};

}

#endif __EFFECT_H__