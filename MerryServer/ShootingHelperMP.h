/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/15/2014 12:19:18 PM
------------------------------------------------------------------------------------------
*/

#ifndef __SHOOTING_HELPER_MP_H__
#define __SHOOTING_HELPER_MP_H__

#include <ShootingHelper.h>
#include <WeaponStats.h>

namespace vgs {
class CharacterEntity;
class ColliderComponent;
class WeaponSS;
}

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	ShootingHelperMP

	Multiplayer implementation of ShootingHelper.
	Prepares data for ray casting.
------------------------------------------------------------------------------------------
*/
class ShootingHelperMP : public ShootingHelper {

public:
					ShootingHelperMP() {}
					~ShootingHelperMP() {}

	glm::vec3		CalculateShotDirection(vgs::CharacterEntity* const shooter, const WeaponStats<vgs::WeaponSS>& weaponStats);
	void			PreapareGeometryData(std::vector<vgs::ColliderComponent*>& geometryColliders, const std::vector<vgs::CharacterEntity*>& targets);
};

}

#endif __SHOOTING_HELPER_MP_H__