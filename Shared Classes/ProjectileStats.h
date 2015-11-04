/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	6/25/2014 1:24:41 PM
------------------------------------------------------------------------------------------
*/

#ifndef __PROJECTILE_STATS_H__
#define __PROJECTILE_STATS_H__

#include "Damage.h"
#include "Stats.h"

namespace vgs {
class InitializationCategory;
}

namespace merrymen
{

//
// enumeration for storing types of projectiles
//
enum class ProjectileType : unsigned char {

	HollowPointBullet,
	ArmourPiercingBullet,
	Shot,
	IncendiaryBullet
};

/*
------------------------------------------------------------------------------------------
	ProjectileStats

	Struct that stores stats of in-game projectiles.
------------------------------------------------------------------------------------------
*/

struct ProjectileStats : public Stats {

	ProjectileType				Type;
	Damage						DamageInfo;
	unsigned short				ProjectilesPerShot;
	float						MaxDamageDistance;

	// constructors
								ProjectileStats();
								ProjectileStats(const ProjectileType type, const vgs::InitializationCategory& category);

	// accessors
	const unsigned short		HealthDamage()	const	{ return DamageInfo.HealthDamage(); }
	const unsigned short		ArmourDamage()	const	{ return DamageInfo.ArmourDamage(); }
	const unsigned short		SplashDamage()	const	{ return DamageInfo.SplashDamage(); }
	const unsigned short		DamageRange()  	const	{ return DamageInfo.DamageRange; }
	const float					SplashRadius() 	const	{ return DamageInfo.SplashRadius; }
};

}

#endif