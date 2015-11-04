/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/7/2014 2:21:22 PM
------------------------------------------------------------------------------------------
*/

#ifndef __DAMAGE_H__
#define __DAMAGE_H__

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	Damage

	Struct that stores damage amounts for various types of damage.
------------------------------------------------------------------------------------------
*/

struct ProjectileStats;

struct Damage {

	unsigned short			DamageValues[3];
	unsigned short			DamageRange;
	float					SplashRadius;

							Damage();
							Damage(const unsigned short healthDamage,
								   const unsigned short armourDamage,
								   const unsigned short splashDamage,
								   const float			splashRadius,
								   const unsigned short damageRange);

							~Damage() {}

	static Damage			CalculateDamage(const int numberOfProjectiles,
											const unsigned short weaponDamage,
											const float shotDistance,
											const float weaponAimingDistance,
											const ProjectileStats& ps);

	// accessors
	const unsigned short	HealthDamage()	const	{ return DamageValues[0]; }
	const unsigned short	ArmourDamage()	const	{ return DamageValues[1]; }
	const unsigned short	SplashDamage()	const	{ return DamageValues[2]; }
};

}

#endif