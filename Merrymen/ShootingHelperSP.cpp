/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/5/2014 5:25:33 PM
------------------------------------------------------------------------------------------
*/

#include "ShootingHelperSP.h"
#include "Character.h"
#include "GameScene.h"

#include "../ToneArmEngine/LevelNode.h"

using namespace merrymen;

//
// calculates the direction of a particular shot
//
glm::vec3 ShootingHelperSP::CalculateShotDirection(Character* const shooter, const WeaponStats<WeaponCS>& weaponStats) {

	return ShootingHelper::CalculateShotDirection<Character, WeaponCS>(shooter, weaponStats);
}

//
// fill the passed lists with data relevant for ray casting
//
void ShootingHelperSP::PreapareGeometryData(Character* const shooter, std::vector<ColliderComponent*>& geometryColliders, bool doFrustumCulling) {

	GameScene* gs = static_cast<GameScene*>(Engine::GetRunningScene());

	if (gs) {

		// get geometry colliders
		std::vector<ColliderComponent*> colliders = gs->GetLevel()->GetGeometryColliders();

		for (auto collider : colliders) {
			geometryColliders.push_back(collider);
		}

		// do frustum culling
		if (doFrustumCulling) {

			Camera* currentCamera = gs->GetCurrentCamera();

			if (currentCamera) {
				currentCamera->GetFrustum().CollidersInFrustum(geometryColliders);
			}
		}
	}
}