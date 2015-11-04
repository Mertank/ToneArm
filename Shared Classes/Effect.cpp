/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/18/2014 4:51:20 PM
------------------------------------------------------------------------------------------
*/

#include "Effect.h"

#define DEFAULT_EFFECT_LIMIT	99

using namespace merrymen;

IMPLEMENT_BASE_RTTI(Effect);

EffectsLimitTable Effect::EffectLimits = Effect::CreateEffectsLimitTable();

//
// default constructor
//
Effect::Effect() :
	Value(0.0f),
	Duration(0.0f)
{}

//
// parametrized constructor
//
Effect::Effect(EffectType type, EffectReason reason, float value, float duration) :
	Type(type),
	Reason(reason),
	Value(value),
	Duration(duration)
{}

//
// initializes the map of effect limits
//
EffectsLimitTable Effect::CreateEffectsLimitTable() {

	EffectsLimitTable limits;

	// ammo crate
	SpecificEffectType ammoCrate = std::pair<EffectType, EffectReason>(EffectType::ExtraAmmo, EffectReason::PickableItem);
	limits.insert(std::pair<SpecificEffectType, unsigned short>(ammoCrate, DEFAULT_EFFECT_LIMIT));

	// slowdown by a projectile
	SpecificEffectType slowdown = std::pair<EffectType, EffectReason>(EffectType::SpeedBuff, EffectReason::RangeAttack);
	limits.insert(std::pair<SpecificEffectType, unsigned short>(slowdown, 4));

	return limits;
}

//
// overloaded less than operator
//
bool Effect::operator<(const Effect* const other) {

	return (Type == other->Type && Reason == other->Reason);
}

//
// updates the state of the effect
//
bool Effect::Update(float dt) {

	Duration -= dt;

	return Duration <= 0.0f;
}