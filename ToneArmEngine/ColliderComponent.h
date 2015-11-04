/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/17/2014 2:25:42 PM
------------------------------------------------------------------------------------------
*/

#ifndef __COLLIDER_COMPONENT_H__
#define __COLLIDER_COMPONENT_H__

#include "Component.h"
#include "Transform.h"

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	ColliderComponent

	Base class for colliders.
------------------------------------------------------------------------------------------
*/

class Shape;
class ModelNode;

class ColliderComponent : public Component {

	DECLARE_RTTI;

public:
	virtual void					Update(float dt) override;
	void							CollisionStarted(const glm::vec3& force);

	// accessors
	virtual Shape* const			GetShape() const = 0;
	
	virtual const glm::vec3&		GetPosition() const	= 0;
	virtual void					SetPosition(const glm::vec3& position);

	virtual const glm::vec3&		GetRotation() const	= 0;
	virtual void					SetRotation(const glm::vec3& rotation);

	bool							IsDynamic()	const								{ return m_dynamic; }
	void							SetDynamic(const bool dynamic)					{ m_dynamic = dynamic; }

	const glm::vec3&				GetPositionDelta() const						{ return m_positionDelta; }

	virtual void					SetActive(bool active);

#ifdef TONEARM_DEBUG
#ifndef MERRY_SERVER
	virtual void					ShowCollider( bool show );
#endif
#endif

protected:
	// constructors
									ColliderComponent();
									ColliderComponent(GameObject* const owner, bool dynamic = false);
									ColliderComponent(const ColliderComponent& other);

	virtual							~ColliderComponent();

	bool							m_dynamic;
	glm::vec3						m_previousOwnerPosition;
	glm::vec3						m_positionDelta;

#ifndef MERRY_SERVER
#ifdef TONEARM_DEBUG
	ModelNode*						m_colliderMesh;
	bool							m_drawCollider;
#endif
#endif
};

}

#endif __COLLIDER_COMPONENT_H__