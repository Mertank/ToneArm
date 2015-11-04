/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	10/6/2014 4:38:12 PM
------------------------------------------------------------------------------------------
*/

#include "PickableItemCS.h"

#include "../ToneArmEngine/ModelNode.h"
#include "../ToneArmEngine/LevelNode.h"

using namespace merrymen;

IMPLEMENT_RTTI(PickableItemCS, PickableItem)

//
// default constructor
//
PickableItemCS::PickableItemCS() :
	m_model(nullptr)
{}

//
//
// constructor that allows specifying the model for the pickable item as well as its position and pick up radius
PickableItemCS::PickableItemCS(const char* modelFile, const glm::vec3& position, float pickUpRadius, bool active) :
	PickableItem(position, pickUpRadius, active),
	m_model(vgs::ModelNode::Create(modelFile))
{
	m_model->SetPosition(position);
}

//
// destructor
//
PickableItemCS::~PickableItemCS() {

	delete m_model;
}

//
// handles picking this object up by some game entity
//
void PickableItemCS::HandlePickup(void* const picker) {

	m_level->RemovePickableObject(this);
}

//
// activates/deactivates collision detection for the item
//
void PickableItemCS::SetActive(bool active) {

	if (active) {
		m_level->AddPickableObject(this);
	}
	else {
		m_level->RemovePickableObject(this);
	}
}