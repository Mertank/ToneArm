/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	LevelNode

	Created by: Karl Merten
	Created on: 26/05/2014

========================
*/
#include "LevelNode.h"
#include "LevelResource.h"
#include "ModelNode.h"
#include "ColliderComponent.h"
#include "BoxCollider.h"
#include "CylinderCollider.h"
#include "Trigger.h"
#include "PickableItem.h"
#include "PhysicsModule.h"

namespace vgs {

LevelNode::LevelNode( void )
{}

LevelNode::~LevelNode( void )
{
	// make sure that PhysicsModule knows the colliders that belong to the level do not exist any more
	PhysicsModule* physics = Engine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);

	for (auto collider : m_colliders) {
		physics->RemoveCollider(collider);
	}

	physics->DeleteSpacePartiotionTree();
}

LevelNode* LevelNode::CreateFromResource( std::shared_ptr<LevelResource> level ) {
	LevelNode* retNode = new LevelNode();
	if ( retNode && retNode->InitializeFromResource( level ) ) {
		return retNode;		
	} else {
		delete retNode;
	}
	return NULL;
}

bool LevelNode::InitializeFromResource( std::shared_ptr<LevelResource> level ) {
	if ( !level.get() ) {
		return false;
	}

	m_level = level;

	const std::vector<ModelInfo*>& models = level->GetModels();

	// next 2 loops take ten times longer than parsing of vgsLevel file
	for ( std::vector<ModelInfo*>::const_iterator iter = models.begin();
		  iter != models.end(); ++iter ) {
		std::string path = level->GetModelDirectory();
		path += "/";
		path += ( *iter )->modelName;
		path += ".vgsModel";

		for ( std::vector<Transform>::const_iterator transIter = ( *iter )->transforms.begin();
			  transIter != ( *iter )->transforms.end(); ++transIter ) {			
			ModelNode* node = ModelNode::Create( path.c_str() );
			if ( node ) {
				node->SetPosition( transIter->GetPosition() );
				node->SetScale( transIter->GetScale() );
				node->SetRotation( transIter->GetRotation() );
				AddChild( node );
			}
		}
	}

	const std::vector<Shape*>& collisions = level->GetColliders();
	
	for ( std::vector<Shape*>::const_iterator iter = collisions.begin();
		  iter != collisions.end(); ++iter ) {
		if ( ( *iter )->GetRTTI() == BoxShape::rtti ) {
			BoxCollider* box = new BoxCollider( this, ( *iter )->GetPosition(), ( ( BoxShape* )( *iter ) )->GetDimensions(), false );
			AddComponent(box);
			box->SetRotation( ( *iter )->GetRotation() );			
			m_colliders.push_back( box );
		} else {
			CylinderCollider* cyl = new CylinderCollider( this, ( *iter )->GetPosition(), ( ( CylinderShape* )( *iter ) )->GetRadius(),
														( ( CylinderShape* )( *iter ) )->GetHeight(), false );
			AddComponent(cyl);
			cyl->SetRotation( ( *iter )->GetRotation() );
			m_colliders.push_back( cyl );
		}
	}

	// create triggers
	const std::vector<TriggerInfo*> triggers = level->GetTriggers();

	for (auto trigger : triggers) {
		
		Trigger* triggerObject = nullptr;
		Shape* triggerShape = trigger->TriggerShape;

		if (triggerShape->GetRTTI() == CylinderShape::rtti) {
			triggerObject = new Trigger(this, trigger->TriggerTag, new CylinderShape(*static_cast<CylinderShape*>(triggerShape)), nullptr, nullptr);
		}
		else if (triggerShape->GetRTTI() == BoxShape::rtti) {
			triggerObject = new Trigger(this, trigger->TriggerTag, new BoxShape(*static_cast<BoxShape*>(triggerShape)), nullptr, nullptr);
		}
		
		this->AddComponent(triggerObject);
		m_colliders.push_back(triggerObject);
	}

	PhysicsModule* physics = Engine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
	physics->CreateSpacePartitionTree();

	return true;
}
#ifdef TONEARM_DEBUG
void LevelNode::ShowCollisions( bool show ) {
	BEGIN_STD_VECTOR_ITERATOR( ColliderComponent*, m_colliders )
		currentItem->ShowCollider( show );
	END_STD_VECTOR_ITERATOR
}
#endif

void LevelNode::Update(float dt) {

	Node::Update(dt);

	for (auto object : m_pickableObjects) {
		object->Update(dt);
	}
}

const std::vector<glm::vec3>& LevelNode::GetSpawnPoints( void ) const {
	return m_level->GetSpawnPoints();
}

//
// adds a pickable object to the level
//
void LevelNode::AddPickableObject(merrymen::PickableItemCS* const item) {

	// add the item to the level if it has not already been added
	if (std::count(m_pickableObjects.begin(), m_pickableObjects.end(), item) == 0) {

		m_pickableObjects.push_back(item);
		AddChild(item->GetModel());

		item->SetLevel(this);
	}

	// turn on collision detection for that object
	if (item->IsActive()) {

		PhysicsModule* physics = Engine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
		physics->AddPickableItem(item);
	}
}

//
// removes a pickable object from the level
//
void LevelNode::RemovePickableObject(merrymen::PickableItemCS* const item) {

	// remove object from the level
	for (unsigned int i = 0; i < m_pickableObjects.size(); i++) {

		if (item == m_pickableObjects[i]) {
			
			RemoveChild(m_pickableObjects[i]->GetModel());
			m_pickableObjects.erase(m_pickableObjects.begin() + i);
		}
	}

	// turn off collision detection for that object
	if (item->IsActive()) {

		PhysicsModule* physics = Engine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
		physics->RemovePickableItem(item);
	}

	// deallocate the memory
	delete item;
}

}