/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/22/2014 1:43:46 PM
------------------------------------------------------------------------------------------
*/

#include "PickableBuffSS.h"
#include "CharacterController.h"
#include "ServerEngine.h"
#include "GameWorld.h"

#include "../MerryServer/CharacterEntity.h"

using namespace merrymen;
using namespace vgs;

IMPLEMENT_RTTI(PickableBuffSS, PickableItem)

//
// default constructor
//
PickableBuffSS::PickableBuffSS() :
	m_buff(nullptr)
{}

//
// parametrized constructor
//
PickableBuffSS::PickableBuffSS(Effect* const buff, const glm::vec3& position, const float pickUpRadius, bool active) :
	PickableItemSS(position, pickUpRadius, active),
	m_buff(buff)
{}

//
// destructor
//
PickableBuffSS::~PickableBuffSS() {

	delete m_buff;
}

//
// handles picking that object up by some character
//
void PickableBuffSS::HandlePickup(void* const picker) {

	// apply buff
	CharacterEntity* newOwner = static_cast<CharacterEntity*>(picker);
	newOwner->ApplyEffect(*m_buff);

	// send a proper message to the clients
	if (m_buff->Type == EffectType::ExtraAmmo) {

		vgs::GameWorld* gw = vgs::ServerEngine::GetInstance()->GetGameWorld();
		gw->SendObjectPicked(static_cast<Player*>(newOwner->GetOwner()), (unsigned char)PickableItemType::PickableBuff);
	}

	PickableItemSS::HandlePickup(picker);
}