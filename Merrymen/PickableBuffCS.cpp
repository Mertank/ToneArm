/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/21/2014 2:15:53 PM
------------------------------------------------------------------------------------------
*/

#include "PickableBuffCS.h"
#include "Character.h"

#include "../ToneArmEngine/ModelNode.h"
#include "../ToneArmEngine/LevelNode.h"

using namespace merrymen;
using namespace vgs;

IMPLEMENT_RTTI(PickableBuffCS, PickableItemCS)

//
// default constructor
//
PickableBuffCS::PickableBuffCS() :
	m_buff(nullptr)
{}

//
// parametrized constructor
//
PickableBuffCS::PickableBuffCS(const char* modelFile, Effect* const buff, const glm::vec3& position, float pickUpRadius, bool active) :
	PickableItemCS(modelFile, position, pickUpRadius, active),
	m_buff(buff)
{}

//
// destructor
//
PickableBuffCS::~PickableBuffCS() {

	if (m_buff) {
		delete m_buff;
	}
}

//
// handles picking that object up by some character
//
void PickableBuffCS::HandlePickup(void* const picker) {

	Character* newOwner = static_cast<Character*>(picker);
	newOwner->ApplyEffect(*m_buff);

	PickableItemCS::HandlePickup(picker);
}

//
// rotates the model show the user that this object is interactive
//
void PickableBuffCS::Update(float dt) {

	m_model->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 90.0f * dt);
}