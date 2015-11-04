/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	10/7/2014 5:45:48 PM
------------------------------------------------------------------------------------------
*/

#include "Stats.h"
#include "Effect.h"

using namespace merrymen;

//
// handles application of the effect
//
void Stats::ApplyEffect(const Effect& effect, vgs::GameObject* const author) {

	Effect* appliedEffect = new Effect(effect);

	// add the effect to the list of applied effects
	AppliedEffects.insert(std::pair<Effect*, vgs::GameObject*>(appliedEffect, author));
}

//
// cancels one of the applied effects
//
void Stats::UnapplyEffect(Effect* const effect, vgs::GameObject* const author) {

	// remove the effect from the list of applied effects
	AppliedEffects.erase(effect);

	delete effect;
}

//
// returns all of the effects applied to the stats which match the passed type
//
std::vector<Effect*> Stats::GetAppliedEffectsOfType(const SpecificEffectType& type) {

	std::vector<Effect*> result;

	for (auto effect : AppliedEffects) {

		SpecificEffectType effectType = SpecificEffectType(effect.first->Type, effect.first->Reason);

		if (effectType == type) {
			result.push_back(effect.first);
		}
	}

	return result;
}

//
// returns all of the effects of the same type as the passed effect, which are applied to the stats 
//
std::vector<Effect*> Stats::GetSimilarAppliedEffects(const Effect& effect) {

	std::vector<Effect*> result;

	SpecificEffectType effectType = SpecificEffectType(effect.Type, effect.Reason);

	for (auto appliedEffect : AppliedEffects) {

		SpecificEffectType appliedEfectType = SpecificEffectType(appliedEffect.first->Type, appliedEffect.first->Reason);

		if (effectType == appliedEfectType) {
			result.push_back(appliedEffect.first);
		}
	}

	return result;
}

//
// cound the number of effects of the same type as the passed effect, which are applied to the stats
//
int Stats::CountSimilarAppliedEffects(const Effect& effect) {

	return GetSimilarAppliedEffects(effect).size();
}