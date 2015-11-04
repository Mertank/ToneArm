/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/18/2014 2:38:20 PM
------------------------------------------------------------------------------------------
*/

#include "PhysicsModule.h"
#include "MathHelper.h"
#include "SpacePartitionTree.h"
#include "CollisionHelper.h"
#include "CylinderCollider.h"
#include "BoxCollider.h"
#include "Trigger.h"

#include <GameObject.h>
#include <PickableItem.h>
#include <memory>

using namespace vgs;

//
// sets up the module
//
void PhysicsModule::Startup() {

	m_staticColliders.reserve(400);
	m_dynamicColliders.reserve(10);
}

//
// updates in-game physics
//
void PhysicsModule::Update(float dt) {

	std::vector<ColliderComponent*>::iterator collidersItr = m_dynamicColliders.begin();

	for (; collidersItr != m_dynamicColliders.end(); ++collidersItr) {

		// check collisions with static colliders
		float speed = glm::length((*collidersItr)->GetPositionDelta());

		if (glm::length((*collidersItr)->GetPositionDelta()) > 0.0f) {

			glm::vec3 moveBackDirection;

			// check for collisions with static colliders
			CheckCollisions((*collidersItr), m_staticColliders, speed, moveBackDirection);

			// check for collisions with triggers
			CheckTriggers((*collidersItr), m_triggers);

			// check pickable objects
			CheckPickableObjects((*collidersItr), m_pickableObjects);
		}
	}
}

//
// cleanup
//
void PhysicsModule::Shutdown() {
	
	DeleteSpacePartiotionTree();

	m_dynamicColliders.clear();
	m_staticColliders.clear();
	m_triggers.clear();
	m_pickableObjects.clear();
}

//
// creates a SpacePartitionTree used for collision detection
//
void PhysicsModule::CreateSpacePartitionTree() {

	m_spacePartitionTree = SpacePartitionTree<ColliderComponent>::CreateSpacePartitionTree(m_staticColliders, nullptr);
}

//
// deletes the SpacePartitionTree
//
void PhysicsModule::DeleteSpacePartiotionTree() {

	delete m_spacePartitionTree;
	m_spacePartitionTree = nullptr;
}

//
// adds a collider to the physics module
//
void PhysicsModule::AddCollider(ColliderComponent* const collider) {

	if (collider->GetRTTI() == Trigger::rtti) {
		m_triggers.push_back(static_cast<Trigger*>(collider));
	}
	else if (collider->IsDynamic()) {
		m_dynamicColliders.push_back(collider);
	}
	else {
		m_staticColliders.push_back(collider);
	}
}

//
// removes a collider from the physics module
//
void PhysicsModule::RemoveCollider(ColliderComponent* const collider) {

	if (collider->GetRTTI() == Trigger::rtti) {
		m_triggers.erase(remove(m_triggers.begin(), m_triggers.end(), collider), m_triggers.end());
	}
	else if (collider->IsDynamic()) {
		m_dynamicColliders.erase(remove(m_dynamicColliders.begin(), m_dynamicColliders.end(), collider), m_dynamicColliders.end());
	}
	else {
		m_staticColliders.erase(remove(m_staticColliders.begin(), m_staticColliders.end(), collider), m_staticColliders.end());
	}
}

//
// removes a pickable object from the physics module
//
void PhysicsModule::RemovePickableItem(PickableItem* const item) {

	m_pickableObjects.erase(remove(m_pickableObjects.begin(), m_pickableObjects.end(), item), m_pickableObjects.end());
}

//
// checks collisions between a movingObject and triggers
//
void PhysicsModule::CheckTriggers(ColliderComponent* movingObject, std::vector<Trigger*> triggers) {

	// prepare moving object's shape data
	BoxShape* movingShape = nullptr;

	if (movingObject->GetShape()->GetRTTI() == BoxShape::rtti) {
		movingShape = static_cast<BoxShape*>(movingObject->GetShape());
	}
	else if (movingObject->GetShape()->GetRTTI() == CylinderShape::rtti) {
		movingShape = BoxShape::CreateFromCylinder(*static_cast<CylinderShape*>(movingObject->GetShape()));
	}

	// check for collisions with triggers
	for (auto trigger : m_triggers) {
		
		BoxShape* triggerShape = nullptr;

		if (trigger->GetShape()->GetRTTI() == BoxShape::rtti) {
			triggerShape = static_cast<BoxShape*>(trigger->GetShape());
		}
		else if (trigger->GetShape()->GetRTTI() == CylinderShape::rtti) {
			triggerShape = BoxShape::CreateFromCylinder(*static_cast<CylinderShape*>(trigger->GetShape()));
		}

		// collision happened - a callback function must be called
		if (movingShape && triggerShape&&  CollisionHelper::BoxBoxCollisionTest(*movingShape, *triggerShape)) {
			trigger->CollisionStarted(movingObject->GetOwner());
		}
	}
}

//
// checks for collisions between ColliderComponents
//
void PhysicsModule::CheckCollisions(ColliderComponent* movingObject, std::vector<ColliderComponent*> objectToCheck, float movementSpeed,
									glm::vec3& slidingDirection) {

	std::set<Intersection> intersections;
	Intersection ptr = nullptr;

	// get all colliders from the octree
	std::vector<TreeNode<ColliderComponent>*> colliderTreeNodes;
	m_spacePartitionTree->CullCylinder(*(static_cast<CylinderShape*>(movingObject->GetShape())), colliderTreeNodes);

	std::set<ColliderComponent*> colliders;

	for(std::vector<TreeNode<ColliderComponent>* const>::const_iterator iter = colliderTreeNodes.begin(); iter != colliderTreeNodes.end(); ++iter) {
		(*iter)->GetObjects(colliders);
	}

	glm::vec3 movementDirection = MathHelper::Normalize(movingObject->GetPositionDelta());

	// check collisions with all of them
	for (auto collider : colliders) {		

		if (collider->GetRTTI() == CylinderCollider::rtti) {

			// FIXME: slide along the circle
			if (CollisionHelper::CylinderCylinderCollisionTest(*(static_cast<CylinderShape*>(movingObject->GetShape())),
															   *(static_cast<CylinderShape*>(collider->GetShape())))) {

				slidingDirection = movementDirection * -1.0f * movementSpeed;
				movingObject->CollisionStarted(slidingDirection);
				return;
			}
		}
		else if (collider->GetRTTI() == BoxCollider::rtti) {

			for (auto intersection : CollisionHelper::CylinderBoxCollisionTest(*(static_cast<CylinderShape*>(movingObject->GetShape())),
																			   *(static_cast<BoxShape*>(collider->GetShape())))) {
				intersections.insert(intersection);
			}
		}
	}

	// slide or stop the player if neccessary
	for (auto segment : intersections) {

		// if one of the surfaces of the geometry collider that the player is colliding with is perpendicular to player's movement direction,
		// do not let the player move

		if (glm::dot(movementDirection, MathHelper::Normalize(segment->EndPoint - segment->StartPoint)) == 0.0f) {

			slidingDirection = movementDirection * -1.0f * movementSpeed;
			movingObject->CollisionStarted(slidingDirection);
			return;
		}

		// find out which one of the two possible directions for player to slide along is more collinear with player's current movement direction
		if (glm::dot(movementDirection, segment->EndPoint - segment->StartPoint) < 0.0f) {
			segment->Revert();
		}
	}

	// player is colliding with one of the corners of the geometry collider (2 surfaces)
	if (intersections.size() > 1) {

		float movementSpeed = glm::length(movingObject->GetPositionDelta());
		int counter = 0;

		// make sure that sliding does not make player get stuck in level geometry
		for (auto outerIntersection : intersections) {
						
			glm::vec3 newPosition = movingObject->GetPosition() - glm::normalize(outerIntersection->EndPoint - outerIntersection->StartPoint) * movementSpeed;

			for (auto innerIntersection : intersections) {

				if (MathHelper::LineSegmentCircleIntersectionCheck2D(*innerIntersection.get(), MathHelper::Circle(newPosition,
																								static_cast<CylinderShape*>(movingObject->GetShape())->GetRadius()))) {
					counter++;
					ptr = outerIntersection;
					break;
				}
			}
		}

		if (counter == intersections.size()) {

			slidingDirection = movementDirection * -1.0f * movementSpeed;
			movingObject->CollisionStarted(slidingDirection);
			return;
		}
	}
	// player is colliding with only one of the surfaces of the geometry collider
	else if (intersections.size() > 0) {
		ptr = *intersections.begin();
	}

	// adjust the movement direction if the player has collided with any geometry
	if (ptr) {

		MathHelper::LineSegment* ls = ptr.get();

		slidingDirection = (MathHelper::Normalize(ls->EndPoint - ls->StartPoint) - movementDirection) * movementSpeed;

		movingObject->CollisionStarted(slidingDirection);
		return;
	}
}

//
// check if a moving object is close enough to any of the pickable items in the game world to pick that item
//
void PhysicsModule::CheckPickableObjects(ColliderComponent* movingObject, std::vector<PickableItem*> objects) {

	for (auto pickableItem : objects) {

		// if the moving object is close enough, the item must be picked up
		if (glm::distance(movingObject->GetOwner()->GetPosition(), pickableItem->GetPosition()) < pickableItem->GetPickUpRadius()) {
			pickableItem->HandlePickup(movingObject->GetOwner());
		}
	}
}

Trigger* const PhysicsModule::GetTriggerByTag(const std::string& tag)
{
	std::vector<Trigger*>::const_iterator it = std::find_if(m_triggers.begin(), m_triggers.end(), 
		[&tag](Trigger const* tr)->bool
		{
			return tag.compare(tr->GetTag()) == 0;
		});
	return it != m_triggers.end() ? *it : nullptr;
}