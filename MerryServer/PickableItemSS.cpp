/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	10/7/2014 10:51:55 AM
------------------------------------------------------------------------------------------
*/

#include "PickableItemSS.h"
#include "Level.h"
#include "ServerEngine.h"
#include "GameWorld.h"

using namespace merrymen;

IMPLEMENT_RTTI(PickableItemSS, PickableItem)

//
// constructor that allows specifying the position and pick up radius of the pickable item
//
PickableItemSS::PickableItemSS(const glm::vec3& position, float pickUpRadius, bool active) :
	PickableItem(position, pickUpRadius, active)
{}

//
// handles picking that object up by some game entity
//
void PickableItemSS::HandlePickup(void* const picker) {

	m_level->RemovePickableObject(this);

	delete this;
}