/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/18/2014 4:08:35 PM
------------------------------------------------------------------------------------------
*/

#include "GameObject.h"

#include "../ToneArmEngine/Component.h"

#include <algorithm>

using namespace vgs;

IMPLEMENT_BASE_RTTI(GameObject);

//
// default constructor
//
GameObject::GameObject() :
	m_transform(Transform()),
	m_transformChanged(true)
{}

//
// destructor
//
GameObject::~GameObject() {

	for (auto component : m_components)
	{
		delete component;
	}

	m_components.clear();
}


//
// initialization
//
bool GameObject::Init( void ) {
	return true;
}

//
// updates the state of a game object
//
void GameObject::Update(float dt) {

	for (auto component : m_components) {

		if (component->IsActive()) {
			component->Update(dt);
		}
	}

	if (m_transformChanged) {
		m_transformChanged = false;
	}
}

//
// sets position of a game object
//
void GameObject::SetPosition(const glm::vec3& pos) 
{
	m_transform.SetPosition(pos);
	m_transformChanged = true;
}

//
// sets rotation of a game object
//
void GameObject::SetRotation(const glm::vec3& rot)
{
	m_transform.SetRotation(rot);
	m_transformChanged = true;
}

//
// sets rotation of a game object
//
void GameObject::SetRotation(const glm::quat& rot)
{
	m_transform.SetRotation(rot);
	m_transformChanged = true;
}

//
// sets scale of a game object
//
void GameObject::SetScale(const glm::vec3& scale)
{
	m_transform.SetScale(scale);
	m_transformChanged = true;
}

//
// translates a game object in the game world
//
void GameObject::Translate(const glm::vec3& value)
{
	m_transform.Translate(value);
	m_transformChanged = true;
}

//
// rotates a game object
//
void GameObject::Rotate(const glm::vec3& axis, float angle) {

	m_transform.Rotate(axis, angle);
	m_transformChanged = true;
}

//
// scales a game object
//
void GameObject::Scale(const glm::vec3& factor)
{
	m_transform.Scale(factor);
	m_transformChanged = true;
}

//
// returns a forward vector of a game object
//
glm::vec3 GameObject::GetForward() {

	return glm::normalize(m_transform.GetForward());
}

//
// adds a copmonent to a game object
//
void GameObject::AddComponent(Component* const component) {

	m_components.push_back(component);
}

//
// adds a copmonent from a game object
//
void GameObject::RemoveComponent(Component* const component) {

	m_components.erase(std::remove(m_components.begin(), m_components.end(), component), m_components.end());
}