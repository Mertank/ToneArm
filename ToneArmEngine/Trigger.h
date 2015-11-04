/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	10/2/2014 12:01:49 PM
------------------------------------------------------------------------------------------
*/

#ifndef __TRIGGER_H__
#define __TRIGGER_H__

#include "ColliderComponent.h"
#include "Shape.h"

#include <functional>
#include <set>

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	Trigger

	A class that represents triggers in 3D space.
------------------------------------------------------------------------------------------
*/

class Trigger : public ColliderComponent {

									DECLARE_RTTI;
	
typedef std::function<void(GameObject* const charater, Trigger* const trigger)> CallbackFunc;

public:
									Trigger(GameObject* owner, const std::string& tag, Shape* const shape, CallbackFunc onEnterCallback, CallbackFunc onExitCallback);
									~Trigger();

	void							Update(float dt) override;						

	// callback invocation
	void							CollisionStarted(GameObject* const charater);
	void							CollisionEnded(GameObject* const charater);

	// accessors
	void							SetOnEnterCallback(CallbackFunc const callback)		{ m_onEnterCallback = callback; }
	void							SetOnExitCallback(CallbackFunc const callback)		{ m_onExitCallback = callback; }

	const std::string&				GetTag() const										{ return m_tag; }
	void							SetTag(const std::string& tag)						{ m_tag = tag; }

	Shape* const					GetShape() const override							{ return m_shape; }
	void							SetShape(Shape* const shape)						{ m_shape = shape; }

	const glm::vec3&				GetPosition() const override						{ return m_shape->GetPosition(); }
	void							SetPosition(const glm::vec3& position) override		{ m_shape->SetPosition(position); }

	const glm::vec3&				GetRotation() const	override						{ return m_shape->GetRotation(); }
	void							SetRotation(const glm::vec3& position) override		{ m_shape->SetPosition(position); }

	virtual void					SetActive(bool active);

private:
									Trigger();

	Shape*							m_shape;

	std::string						m_tag;

	CallbackFunc					m_onEnterCallback;
	CallbackFunc					m_onExitCallback;

	std::set<GameObject*>			m_collidingObjects;
};

}

#endif __TRIGGER_H__