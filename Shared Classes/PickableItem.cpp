/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/18/2014 4:06:44 PM
------------------------------------------------------------------------------------------
*/

#include "PickableItem.h"

using namespace vgs;

IMPLEMENT_RTTI(PickableItem, GameObject);

//
// default constructor
//
PickableItem::PickableItem() :
	m_active(false),
	m_pickUpRadius(0.0f) {
}

//
// parametrized constructor
//
PickableItem::PickableItem(const glm::vec3& position, float pickUpRadius, bool active) :
	m_active(false),
	m_pickUpRadius(pickUpRadius)
{
	SetPosition(position);
}