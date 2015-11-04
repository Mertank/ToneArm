/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	10/7/2014 5:45:48 PM
------------------------------------------------------------------------------------------
*/

#ifndef __STATS_H__
#define __STATS_H__

#include <map>
#include <vector>

#include "Effect.h"

namespace vgs {
class GameObject;
}

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	Stats

	Base struct for storing various in-game stats.
------------------------------------------------------------------------------------------
*/

struct Stats {

	virtual bool			UpdateEffects(float dt) { return false; }
	virtual void			ApplyEffect(const Effect& effect, vgs::GameObject* const author = nullptr);
	virtual void			UnapplyEffect(Effect* const effect, vgs::GameObject* const author = nullptr);

	std::vector<Effect*>	GetAppliedEffectsOfType(const SpecificEffectType& type);
	std::vector<Effect*>	GetSimilarAppliedEffects(const Effect& effect);
	int						CountSimilarAppliedEffects(const Effect& effect);

protected:
	std::multimap<Effect*, vgs::GameObject*> AppliedEffects;
};

}

#endif __STATS_H__