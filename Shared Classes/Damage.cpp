/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/7/2014 2:21:22 PM
------------------------------------------------------------------------------------------
*/

#include "Damage.h"
#include "ProjectileStats.h"

namespace merrymen {

//
// default constructor
//
Damage::Damage() :
		DamageRange(0),
		SplashRadius(0.0f)
{
	DamageValues[0] = DamageValues[1] = DamageValues[2] = 0;
}

//
// parametrized constructor
//
Damage::Damage(const unsigned short healthDamage,
			   const unsigned short armourDamage,
			   const unsigned short splashDamage,
			   const float			splashRadius,
			   const unsigned short damageRange) :
	DamageRange(damageRange),
	SplashRadius(splashRadius)
{
	DamageValues[0] = healthDamage;
	DamageValues[1] = armourDamage;
	DamageValues[2] = splashDamage;
}

//
// static method that calculates damage of all types based on the passed values
//
Damage Damage::CalculateDamage(const int numberOfProjectiles,
							   const unsigned short weaponDamage,
							   const float shotDistance,
							   const float weaponAimingDistance,
							   const ProjectileStats& ps) {

	Damage result;

	// apply different modifiers to all damage values
	for (int i = 0; i < 4; i++) {
		
		for (int j = 0; j < numberOfProjectiles; j++) {

			if (ps.DamageInfo.DamageValues[i] > 0) {

				unsigned short dmg;

				// randomize the damage amount using DamageRange value
				dmg = std::rand() % (ps.DamageInfo.DamageValues[i] + ps.DamageRange()) + (ps.DamageInfo.DamageValues[i] - ps.DamageRange());

				// reduce damage for long-distance shots
				if (shotDistance > weaponAimingDistance) {
				
					float fraction = weaponAimingDistance / shotDistance;
					dmg = (unsigned short)((float)(dmg) * fraction);
				}

				result.DamageValues[i] += dmg;
			}
		}

		// add basic weapon damage
		if (result.DamageValues[i] > 0) {
			result.DamageValues[i] += weaponDamage;
		}
	}

	result.SplashRadius = ps.SplashRadius();

	return result;
}

}