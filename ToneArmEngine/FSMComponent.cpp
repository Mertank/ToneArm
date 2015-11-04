/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/3/2014 1:41:27 PM
------------------------------------------------------------------------------------------
*/

#include "FSMComponent.h"

using namespace vgs;

IMPLEMENT_RTTI(FSMComponent, Component);

//
// default constructor
//
FSMComponent::FSMComponent() :
	m_currentState(nullptr)
{}

//
// default destructor
//
FSMComponent::~FSMComponent() {

	for (auto state : m_states) {
		delete state.second;
	}

	m_states.clear();
}

//
// executes current state logic
//
void FSMComponent::Update(float dt) {

	m_currentState->Execute();
}