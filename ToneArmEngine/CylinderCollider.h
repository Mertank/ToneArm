/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/19/2014 1:16:46 PM
------------------------------------------------------------------------------------------
*/

#ifndef __CYLINDER_COLLIDER_H__
#define __CYLINDER_COLLIDER_H__

#include "ColliderComponent.h"
#include "CylinderShape.h"

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	CylinderCollider

	Class that represents a 3D bounding cylinder.
------------------------------------------------------------------------------------------
*/

#ifdef TONEARM_DEBUG
	class ModelNode;
#endif

class CylinderCollider : public ColliderComponent {

								DECLARE_RTTI;
public:
								CylinderCollider(GameObject* const owner, const glm::vec3& position, float radius, float height, bool dynamic);

	// accessors
	virtual Shape* const		GetShape() const override								{ return m_shape; }

	const glm::vec3&			GetPosition() const override							{ return m_shape->GetPosition(); }
	void						SetPosition(const glm::vec3& position) override			{ m_shape->SetPosition(position); }

	const glm::vec3&			GetRotation() const	override							{ return m_shape->GetRotation(); }
	void						SetRotation(const glm::vec3& rotation) override			{ m_shape->SetRotation(rotation); }

	float						GetRadius() const										{ return m_shape->GetRadius(); }
	void						SetRadius(float radius)									{ m_shape->SetRadius(radius); }

	float						GetHeight() const										{ return m_shape->GetHeight(); }
	void						SetHeight(float height)									{ m_shape->SetHeight(height); }

private:
								CylinderCollider();
	virtual						~CylinderCollider();

	CylinderShape*				m_shape;
};

} // namespace vgs

#endif __CYLINDERCOLLIDER_H__