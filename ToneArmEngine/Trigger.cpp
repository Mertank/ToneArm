/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	10/2/2014 12:01:49 PM
------------------------------------------------------------------------------------------
*/

#include "Trigger.h"
#include "BoxCollider.h"
#include "CylinderCollider.h"
#include "CollisionHelper.h"

#include <GameObject.h>

#ifndef MERRY_SERVER
#ifdef TONEARM_DEBUG
#include "MeshHelper.h"
#include "ModelNode.h"
#endif
#endif

using namespace vgs;

IMPLEMENT_RTTI(Trigger, ColliderComponent);

//
// default constructor
//
Trigger::Trigger() :
	m_shape(nullptr),
	m_onEnterCallback(nullptr),
	m_onExitCallback(nullptr)
{}

//
// constructor that lets you specify the owner of the trigger, its shape and callback functions
//
Trigger::Trigger(GameObject* owner, const std::string& tag, Shape* const shape, CallbackFunc onEnterCallback, CallbackFunc onExitCallback) :
	ColliderComponent(owner),
	m_tag(tag),
	m_shape(shape),
	m_onEnterCallback(onEnterCallback),
	m_onExitCallback(onExitCallback)
{
	SetActive(false);

#ifndef MERRY_SERVER
#ifdef TONEARM_DEBUG
	if (GetOwner() && GetOwner()->GetRTTI().DerivesFrom(Node::rtti)) {

		float*			vertexData = NULL;
		unsigned short*	indexData = NULL;

		unsigned int	vertexCount = 0;
		unsigned int	indexCount = 0;
	
		if (shape->GetRTTI() == CylinderShape::rtti) {

			CylinderShape* cylinder = static_cast<CylinderShape*>(m_shape);
			MeshHelper::MakeCylinderMesh(cylinder->GetRadius(), cylinder->GetHeight(), 32, VertexComponents::POSITION, &vertexData, &indexData,
										 vertexCount, indexCount);
		}
		else if (shape->GetRTTI() == BoxShape::rtti) {

			BoxShape* box = static_cast<BoxShape*>(m_shape);
			MeshHelper::MakeRectMesh(box->GetDimensions().x, box->GetDimensions().y, box->GetDimensions().z, VertexComponents::POSITION, &vertexData,
									 &indexData, vertexCount, indexCount);

		}

		m_colliderMesh = ModelNode::CreateWithMesh(vertexData, indexData, GL_UNSIGNED_SHORT, vertexCount, indexCount, VertexComponents::POSITION,
												   glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), m_shape->GetBoundingDimensions());	
		m_colliderMesh->SetOutlineOnly( true );
		m_colliderMesh->SetLighting( false );
	
		m_colliderMesh->SetPosition(m_shape->GetPosition());
		m_colliderMesh->SetRotation(m_shape->GetRotation());

		static_cast<Node*>(GetOwner())->AddChild(m_colliderMesh);
	}
#endif
#endif
}

//
// destructor
//
Trigger::~Trigger() {

	delete m_shape;

	m_collidingObjects.clear();
}

//
// invokes a CollisionStarted callback
//
void Trigger::CollisionStarted(GameObject* const character) {

	// make sure the CollisionStarted callback was not already fired
	if (m_collidingObjects.find(character) != m_collidingObjects.end()) {
		return;
	}

	m_collidingObjects.insert(character);

	if (m_onEnterCallback) {
		m_onEnterCallback(character, this);
	}
}

//
// invokes a CollisionEnded callback
//
void Trigger::CollisionEnded(GameObject* const character) {

	if (m_onExitCallback) {
		m_onExitCallback(character, this);
	}
}

//
// updates the state of the trigger
//
void Trigger::Update(float dt) {

	// prepare trigger shape
	BoxShape* triggerShape = nullptr;

	if (m_shape->GetRTTI() == BoxShape::rtti) {
		triggerShape = static_cast<BoxShape*>(m_shape);
	}
	else if (m_shape->GetRTTI() == CylinderShape::rtti) {
		triggerShape =  BoxShape::CreateFromCylinder(*static_cast<CylinderShape*>(m_shape));
	}

	// check which entities have left the collider to call a proper callback function
	for (std::set<GameObject*>::iterator objectItr = m_collidingObjects.begin(); objectItr != m_collidingObjects.end();) {

		ColliderComponent* collider = (*objectItr)->GetComponentOfType<ColliderComponent>();

		BoxShape* boundingShape = nullptr;

		if (collider->GetRTTI() == BoxCollider::rtti) {

			BoxCollider* box = static_cast<BoxCollider*>(collider);
			boundingShape = static_cast<BoxShape*>(box->GetShape());
		}
		else if (collider->GetRTTI() == CylinderCollider::rtti) {

			CylinderCollider* cylinder = static_cast<CylinderCollider*>(collider);
			boundingShape = BoxShape::CreateFromCylinder(*static_cast<CylinderShape*>(cylinder->GetShape()));
		}

		// this entity has left the trigger - callback function must be called
		if (boundingShape && !CollisionHelper::BoxBoxCollisionTest(*triggerShape, *boundingShape)) {
			CollisionEnded((*objectItr));
			m_collidingObjects.erase(objectItr++);
		}
		else {
			++objectItr;
		}
	}
}

//
//
//
void Trigger::SetActive(bool active) {

	ColliderComponent::SetActive(active);
}