/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/19/2014 12:15:49 PM
------------------------------------------------------------------------------------------
*/

#ifndef _BOX_COLLIDER_H__
#define _BOX_COLLIDER_H__

#include "ColliderComponent.h"
#include "BoxShape.h"

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	BoxCollider

	Class that represents a 3D bounding box.
------------------------------------------------------------------------------------------
*/

#ifdef TONEARM_DEBUG
	class ModelNode;
#endif

class CylinderCollider;

class BoxCollider : public ColliderComponent {

	DECLARE_RTTI;

public:
	// constructors
								BoxCollider(GameObject* const owner, const glm::vec3& position, const glm::vec3& dimensions, bool dynamic);
	
	Shape* const				GetShape() const override							{ return m_shape; }
	const glm::vec3&			GetDimensions() const								{ return m_shape->GetDimensions(); }

	const glm::vec3&			GetPosition() const override						{ return m_shape->GetPosition(); }
	void						SetPosition(const glm::vec3& position) override		{ ColliderComponent::SetPosition(position);
																					  m_shape->SetPosition(position); }

	const glm::vec3&			GetRotation() const override						{ return m_shape->GetRotation(); }
	void						SetRotation(const glm::vec3& rotation) override		{ ColliderComponent::SetRotation(rotation);
																					  m_shape->SetRotation(rotation); }

private:
								BoxCollider();
	virtual						~BoxCollider();

	BoxShape*					m_shape;
};

} //namepsace vgs

#endif