/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/5/2014 4:37:30 PM
------------------------------------------------------------------------------------------
*/

#ifndef __SHOOTING_HELPER_H__
#define __SHOOTING_HELPER_H__

#include <glm/glm.hpp>
#include <vector>

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	ShootingHelper

	Interface capable of preparing data, relevant for ray casting.
------------------------------------------------------------------------------------------
*/

template <class C>
struct WeaponStats;

class ShootingHelper {

public:
						ShootingHelper() {}
	virtual				~ShootingHelper() {}

	template <class T, class C>
	glm::vec3			CalculateShotDirection(T* const shooter, const WeaponStats<C>& weaponStats);
};

}

//
// randomizes the trajectory of each particular projectile
//
template <class T, class C>
glm::vec3 merrymen::ShootingHelper::CalculateShotDirection(T* const shooter, const WeaponStats<C>& weaponStats) {

	glm::vec3 forward = shooter->GetForward();
	glm::vec3 direction = forward;

	// pick the right fire spread angle
	float spreadAngle = shooter->IsAiming() ? weaponStats.FireSpreadAngleAim : weaponStats.FireSpreadAngleHip;

	if (spreadAngle > 0.0f) {

		// randomize shot direction
		spreadAngle = (std::rand() % (short)(spreadAngle) - spreadAngle / 2);

		float newX = forward.x * cos(glm::radians(spreadAngle)) - forward.z * sin(glm::radians(spreadAngle));
		float newZ = forward.z * cos(glm::radians(spreadAngle)) + forward.x * sin(glm::radians(spreadAngle));

		direction = glm::vec3(newX, forward.y, newZ);
	}

	return direction;
}

#endif __SHOOTING_HELPER_H__