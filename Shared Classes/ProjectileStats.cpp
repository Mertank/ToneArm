/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/11/2014 2:31:38 PM
------------------------------------------------------------------------------------------
*/

#include "ProjectileStats.h"

#include "../ToneArmEngine/InitializationCategory.h"

namespace merrymen {
	
//
// default constructor
//
ProjectileStats::ProjectileStats() :
	Type(ProjectileType::HollowPointBullet),
	DamageInfo(Damage()),
	ProjectilesPerShot(0),
	MaxDamageDistance(0.0f)
{}

//
// parametrized constructor for reading values from a .ini file
//
ProjectileStats::ProjectileStats(const ProjectileType type, const vgs::InitializationCategory& category) {
		
	DamageInfo.DamageValues[0]	= category.GetIntValueForKey("health_damage");
	DamageInfo.DamageValues[1]	= category.GetIntValueForKey("armour_damage");
	DamageInfo.DamageValues[3]	= category.GetIntValueForKey("splash_damage");
	DamageInfo.SplashRadius		= category.GetFloatValueForKey("splash_radius");
	DamageInfo.DamageRange		= category.GetIntValueForKey("damage_range");
	ProjectilesPerShot			= category.GetIntValueForKey("projectiles_per_shot");
	MaxDamageDistance			= category.GetFloatValueForKey("max_damage_distance_modifier");
}

}