/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/15/2014 12:19:18 PM
------------------------------------------------------------------------------------------
*/

#include "ShootingHelperMP.h"
#include "CharacterEntity.h"
#include "GameMode.h"
#include "ServerEngine.h"
#include "Level.h"
#include "GameWorld.h"

#include "../ToneArmEngine/SpacePartitionTree.h"
#include "../ToneArmEngine/PhysicsModule.h"

using namespace merrymen;
using namespace vgs;

//
// calculates the direction of a particular shot
//
glm::vec3 ShootingHelperMP::CalculateShotDirection(CharacterEntity* const shooter, const WeaponStats<WeaponSS>& weaponStats) {

	return ShootingHelper::CalculateShotDirection<CharacterEntity, WeaponSS>(shooter, weaponStats);
}

//
// fill the passed lists with data relevant for ray casting
//
void ShootingHelperMP::PreapareGeometryData(std::vector<ColliderComponent*>& geometryColliders, const std::vector<CharacterEntity*>& targets) {

	// create a bounding box around all enemies to exclude irrelevant colliders from the ray cacting check
	std::vector<TreeNode<ColliderComponent>*> treeNodes;

	std::shared_ptr<BoxShape> boxPtr = MathHelper::CreateBoundingBox<CharacterEntity>(targets);
	BoxShape* boundingBox = boxPtr.get();

	// get geometry colliders from the level
	ServerEngine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS)->GetSpacePartitionTree()->CullBox(*boundingBox, treeNodes);

	// transform into shapes
	for(std::vector<TreeNode<ColliderComponent>* const>::const_iterator iter = treeNodes.begin(); iter != treeNodes.end(); ++iter)
	{
		(*iter)->GetObjects(geometryColliders);
	}

	treeNodes.clear();
}