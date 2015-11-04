/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/3/2014 1:41:27 PM
------------------------------------------------------------------------------------------
*/

#ifndef __FSM_H__
#define __FSM_H__

#include "Component.h"
#include "FSMState.h"

#include <map>

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	FSM

	Base class for finite state machines.
------------------------------------------------------------------------------------------
*/

class GameObject;

class FSMComponent : public Component {

	DECLARE_RTTI;

public:
	template <class T>
	static FSMComponent*				Create(GameObject* const owner);

	template <class T>
	bool								Init(GameObject* const owner);

										~FSMComponent();

	void								Update(float dt) override;

	template <class T>
	void								ChangeState();

private:
										FSMComponent();

	std::map<const RTTI*, FSMState*>	m_states;
	FSMState*							m_currentState;
};

//
// creates FSM
//
template <class T>
FSMComponent* FSMComponent::Create(GameObject* const owner) {

	FSMComponent* component = new FSMComponent();

	if (component && component->Init<T>(owner)) {
		return component;
	}

	// initialization failed
	delete component;
	return nullptr;
}

//
// initializes fsm with proper values
//
template <class T>
bool FSMComponent::Init(GameObject* const owner) {

	if (Component::Init(owner)) {

		m_currentState = new T();
		m_states.insert(std::pair<const RTTI*, FSMState*>(&(T::rtti), m_currentState));
		return true;
	}

	// initialization failed
	return false;
}

//
// changes the current state of the FSM
//

template <class T>
void FSMComponent::ChangeState() {

	// exit the current state properly
	m_currentState->OnExit();
	
	// reassing the current state to a different state
	std::map<const RTTI*, FSMState*>::iterator statesItr = m_states.find(&T::rtti);

	if (statesItr != m_states.end()) {
		m_currentState = statesItr->second;
	}
	else {
		m_currentState = new T();
		m_states.insert(std::pair<const RTTI*, FSMState*>(&T::rtti, m_currentState));
	}

	// enter the new state properly
	m_currentState->OnEnter();
}

}

#endif __FSM_H__