/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/5/2014 5:25:33 PM
------------------------------------------------------------------------------------------
*/

#include <ShootingHelper.h>
#include <WeaponStats.h>

#include "../Merrymen/WeaponCS.h"

namespace vgs {
class ColliderComponent;
}

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	ShootingHelperSP

	Preparing data, relevant for ray casting (single-player implementation).
------------------------------------------------------------------------------------------
*/

class Character;

class ShootingHelperSP : public ShootingHelper {

public:
						ShootingHelperSP() {}
						~ShootingHelperSP() {}

	glm::vec3			CalculateShotDirection(Character* const shooter, const WeaponStats<WeaponCS>& weaponStats);
	void				PreapareGeometryData(Character* const shooter, std::vector<vgs::ColliderComponent*>& geometryColliders, bool doFrustumCulling);
};

}