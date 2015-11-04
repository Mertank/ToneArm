/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/10/2014 10:06:05 AM
------------------------------------------------------------------------------------------
*/

#include "ClientEntity.h"
#include "CharacterStats.h"

#include "../ToneArmEngine/CylinderCollider.h"

using namespace merrymen;

IMPLEMENT_RTTI(ClientEntity, Node);

//
// destructor
//
ClientEntity::~ClientEntity()
{}

//
// creates a model and a collider for the GameEntity object
//
bool ClientEntity::Init(const char* modelFile, bool online) {

	if (Node::Init()) {

		m_online = online;

		// create a model
		m_model = ModelNode::Create(modelFile);
		AddChild(m_model);
		
		// TODO: use actual size of the model to initialize collider dimensions
		// create a collider
		ColliderComponent* collider = new CylinderCollider(this, GetPosition() + glm::vec3(0.0f, CHARACTER_HEIGHT / 2, 0.0f), 35.0f, 150.0f, true);

		if (collider) {
			AddComponent(collider);

#ifdef TONEARM_DEBUG
			collider->ShowCollider(false);
#endif
		}

		return true;
	}

	return false;
}

//
// updates the state of a game entity
//
void ClientEntity::Update(float dt) {

	Node::Update(dt);
}