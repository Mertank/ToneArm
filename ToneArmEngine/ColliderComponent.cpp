/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/17/2014 2:25:42 PM
------------------------------------------------------------------------------------------
*/

#include "ColliderComponent.h"
#include "PhysicsModule.h"

#ifndef MERRY_SERVER
#include "Engine.h"
#else
#include "../MerryServer/ServerEngine.h"
#endif

#ifndef MERRY_SERVER
#ifdef TONEARM_DEBUG
#include "MeshHelper.h"
#include "ModelNode.h"
#endif
#endif

#include <GameObject.h>

using namespace vgs;

IMPLEMENT_RTTI(ColliderComponent, Component);

//
// default constructor
//
ColliderComponent::ColliderComponent() :
	Component(),
	m_positionDelta(glm::vec3()),
	m_previousOwnerPosition(glm::vec3()),
	m_dynamic(false)
{}

//
// constructor that sets collider's attributes to the passed value
//
ColliderComponent::ColliderComponent(GameObject* const owner, bool dynamic) :
	Component(owner),
	m_dynamic(dynamic),
	m_previousOwnerPosition(owner->GetPosition()),
	m_positionDelta(glm::vec3())

#ifndef MERRY_SERVER
#ifdef TONEARM_DEBUG
	,
	m_colliderMesh(nullptr),
	m_drawCollider(false)
#endif
#endif
{}

//
// copy constructor
//
ColliderComponent::ColliderComponent(const ColliderComponent& other) {

	m_dynamic = other.IsDynamic();

	m_previousOwnerPosition = other.GetOwner()->GetPosition();
	m_positionDelta = glm::vec3();

	SetActive(other.IsActive());
}

//
// destructor
//
ColliderComponent::~ColliderComponent() {

	PhysicsModule* physics;

#ifndef MERRY_SERVER
	physics = Engine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
#else
	physics = ServerEngine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
#endif
	
	physics->RemoveCollider(this);
}

//
// updates the state of the component
//
void ColliderComponent::Update(float dt) {

	if (IsDynamic()) {
		
		GameObject* owner = GetOwner();

		if (owner->IsTransformChanged()) {
			
			m_positionDelta = owner->GetPosition() - m_previousOwnerPosition;
			SetPosition(GetPosition() + m_positionDelta);
		}

		m_previousOwnerPosition = owner->GetPosition();
	}
}

//
// handles a collision
//
void ColliderComponent::CollisionStarted(const glm::vec3& force) {

	// update the position of the collider
	SetPosition(GetPosition() + force);

	// update its owner's position
	GetOwner()->SetPosition(GetOwner()->GetPosition() + force);
	
	// update cached value
	m_previousOwnerPosition = GetOwner()->GetPosition();
}

//
// updates the position of the collider
//
void ColliderComponent::SetPosition(const glm::vec3& postion) {

#ifndef MERRY_SERVER
#ifdef TONEARM_DEBUG
	if (m_colliderMesh) {
		m_colliderMesh->SetPosition(postion);
	}
#endif
#endif
}

//
// updates the rotation of the collider
//
void ColliderComponent::SetRotation(const glm::vec3& rotation) {

#ifndef MERRY_SERVER
#ifdef TONEARM_DEBUG
	m_colliderMesh->SetRotation(rotation);
#endif
#endif
}

//
// sets the value and includes the collider into (excludes from) the collision detection process
//
void ColliderComponent::SetActive(bool active) {

	Component::SetActive(active);

	PhysicsModule* physics;

#ifndef MERRY_SERVER
	physics = Engine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
#else
	physics = ServerEngine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
#endif

	if (active) {
		physics->AddCollider(this);
	}
	else {
		physics->RemoveCollider(this);
	}
}

//
// shows the collider
//
#ifndef MERRY_SERVER
#ifdef TONEARM_DEBUG
void ColliderComponent::ShowCollider( bool drawCol ) {
	m_drawCollider = drawCol;
	m_colliderMesh->SetVisible( drawCol );
}
#endif
#endif