/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/18/2014 4:08:35 PM
------------------------------------------------------------------------------------------
*/

#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "../ToneArmEngine/Transform.h"
#include "../ToneArmEngine/VGSMacro.h"
#include "../ToneArmEngine/RTTI.h"

#include <vector>

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	GameObject

	Base class for game objects.
------------------------------------------------------------------------------------------
*/

class Component;

class GameObject {
								DECLARE_RTTI;

public:
	virtual						~GameObject();

								CREATE_METH(GameObject);

	virtual bool				Init();

	virtual void				Update(float dt);

	// components
	template <class T>
	T*							GetComponentOfType();

	template <class T>
	std::vector<T*>				GetAllComponentsOfType();

	void						AddComponent(Component* const component);
	void						RemoveComponent(Component* const component);

	// transform 
	const Transform&			GetTransform() const						{ return m_transform; }

	const glm::mat4&			GetLocalModelMatrix() const					{ return m_transform.GetModelMatrix(); }

	virtual void				Translate(const glm::vec3& value);
	virtual void				Rotate(const glm::vec3& axis, const float angle);
	virtual void				Scale(const glm::vec3& factor);

	const glm::vec3&			GetPosition() const							{ return m_transform.GetPosition(); }
	virtual void				SetPosition(const glm::vec3& pos);

	const glm::vec3&			GetRotation() const							{ return m_transform.GetRotation(); }
	virtual void				SetRotation(const glm::vec3& rot);
	virtual void				SetRotation(const glm::quat& rot);

	const glm::vec3&			GetScale() const							{ return m_transform.GetScale(); }
	virtual void				SetScale(const glm::vec3& scale);

	glm::vec3					GetForward();

	void						SetTransformChanged(const bool value)		{ m_transformChanged = value; }
	virtual const bool			IsTransformChanged() const					{ return m_transformChanged; }

protected:
								GameObject();

	Transform					m_transform;
	bool						m_transformChanged;

private:
	std::vector<Component*>		m_components;
};

//
// returns the first component of the specified type that it finds in that GameObject
//
template <class T>
T* GameObject::GetComponentOfType() {

	for (auto component : m_components) {

		if (component->GetRTTI().DerivesFrom(T::rtti)) {
			return static_cast<T*>(component);
		}
	}

	return nullptr;	
}

//
// returns all components of the specified type which it finds in that GameObject
//
template <class T>
std::vector<T*>	GameObject::GetAllComponentsOfType() {

	std::vector<T*> result;

	for (auto component : m_components) {

		if (component->GetRTTI().DerivesFrom(T::rtti)) {
			result.push_back(static_cast<T*>(component));
		}
	}

	return result;	
}

}

#endif __GAME_OBJECT_H__