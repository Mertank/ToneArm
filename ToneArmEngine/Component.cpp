/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/3/2014 2:39:05 PM
------------------------------------------------------------------------------------------
*/

#include "Component.h"

#include <GameObject.h>

using namespace vgs;

IMPLEMENT_BASE_RTTI(Component);

//
// default constructor
//
Component::Component() :
	m_owner(nullptr),
	m_active(false)
{}

//
// sets the owner of the component to the passed value (pointer)
//
Component::Component(GameObject* const owner) :
	m_owner(owner),
	m_active(true)
{}

//
// initializes a component with passed values
//
bool Component::Init(GameObject* const owner) {

	SetOwner(owner);

	return true;
}

//
// destructor
//
Component::~Component() {}