/*
========================

Copyright (c) 2014 Vinyl Games Studio

	LevelNode

		A node that contains a level in it.

		Created by: Karl Merten
		Created on: 26/05/2014

========================
*/

#ifndef __LEVELNODE_H__
#define __LEVELNODE_H__

#include "Node.h"
#include "PickableItem.h"

#include "../Merrymen/PickableItemCS.h"

#include <vector>
#include <memory>

namespace vgs {

class LevelResource;
class ColliderComponent;

class LevelNode : public Node {
public:
													~LevelNode( void );

	static LevelNode*								CreateFromResource( std::shared_ptr<LevelResource> level );

	void											Update(float dt) override;

	const std::vector<ColliderComponent*>&			GetGeometryColliders() const									{ return m_colliders; }

	const std::vector<merrymen::PickableItemCS*>&	GetPickableObjects() const										{ return m_pickableObjects; }
	const std::vector<glm::vec3>&					GetSpawnPoints() const;

	void											AddPickableObject(merrymen::PickableItemCS* const item);
	void											RemovePickableObject(merrymen::PickableItemCS* const item);

	template<typename T>
	T* const										GetNearestPickableObject(const glm::vec3& position);

#ifdef TONEARM_DEBUG
	void											ShowCollisions( bool show = true );
#endif
private:
													LevelNode( void );

	bool											InitializeFromResource( std::shared_ptr<LevelResource> level );	


	std::vector<ColliderComponent*>					m_colliders;
	std::shared_ptr<LevelResource>					m_level;
	std::vector<merrymen::PickableItemCS*>			m_pickableObjects;
};

//
// returns pickable object, closest to the passed position
//
template<typename T>
T* const LevelNode::GetNearestPickableObject(const glm::vec3& position) {

	T* result = nullptr;
	float minDistance = 5000.0f;

	// simple sort algorithm
	for (auto object : m_pickableObjects) {

		if (object->GetRTTI() == T::rtti) {

			float distance = glm::distance(position, object->GetPosition());

			if (distance < minDistance) {

				minDistance = distance;
				result = static_cast<T*>(object);
			}
		}
	}

	return result;
}

}

#endif //__LEVELNODE_H__