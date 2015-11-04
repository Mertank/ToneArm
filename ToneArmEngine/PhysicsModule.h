/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/18/2014 2:38:20 PM
------------------------------------------------------------------------------------------
*/

#ifndef __PHYSICS_MODULE_H__
#define __PHYSICS_MODULE_H__

#include "EngineModule.h"
#include "MathHelper.h"
#include "SpacePartitionTree.h"

#include <vector>

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	PhysicsModule

	Engine module in charge of in-game physics.
------------------------------------------------------------------------------------------
*/

class ColliderComponent;
class GameObject;
class CylinderCollider;
class BoxCollider;
class Trigger;
class PickableItem;

class PhysicsModule : public EngineModule {

public:
	virtual void									Startup()											override;
	virtual void									Update(float dt)									override;
	virtual void									Shutdown()											override;

	static PhysicsModule*							Create()											{ return new PhysicsModule(); }	
	void											CreateSpacePartitionTree();
	void											DeleteSpacePartiotionTree();

	SpacePartitionTree<ColliderComponent>* const	GetSpacePartitionTree() const						{ return m_spacePartitionTree; }		

	void											AddCollider(ColliderComponent* const collider);
	void											RemoveCollider(ColliderComponent* const collider);

	void											AddPickableItem(PickableItem* const item)			{ m_pickableObjects.push_back(item); }
	void											RemovePickableItem(PickableItem* const item);

	void											CheckCollisions(ColliderComponent* movingObject, std::vector<ColliderComponent*> objectsToCheck,
																	float movementSpeed, glm::vec3& slidingDirection);

	void											CheckTriggers(ColliderComponent* movingObject, std::vector<Trigger*> triggers);

	void											CheckPickableObjects(ColliderComponent* movingObject, std::vector<PickableItem*> objects);

	std::vector<Trigger*>&							GetAllTriggers()									{ return m_triggers; } 
	Trigger* const									GetTriggerByTag(const std::string& tag);

private:
	std::vector<ColliderComponent*>					m_staticColliders;
	std::vector<ColliderComponent*>					m_dynamicColliders;
	std::vector<Trigger*>							m_triggers;
	std::vector<PickableItem*>						m_pickableObjects;

	SpacePartitionTree<ColliderComponent>*			m_spacePartitionTree;
};

}

#endif __PHYSICS_MODULE_H__