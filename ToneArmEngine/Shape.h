/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Shape

		Base class for shapes.

		Created by: Karl Merten
		Created on: 05/06/2014

========================
*/

#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "RTTI.h"
#include "Transform.h"

namespace vgs {

class Shape {
	DECLARE_RTTI

	friend class ColliderComponent;

public:
									Shape( void );
									Shape( const Shape& other );
									~Shape( void );

	const Transform&				GetTransform( void ) const { return m_transform; }
	
	const glm::vec3&				GetPosition( void ) const { return m_transform.GetPosition(); }
	void							SetPosition( const glm::vec3& pos ) { m_transform.SetPosition( pos ); }
	
	const glm::vec3&				GetRotation() const { return m_transform.GetRotation(); }
	void							SetRotation( const glm::vec3& rot );

	virtual const glm::vec3&		GetBoundingDimensions( void ) const { return m_boundingDimensions; }
	void							SetBoundingDimensions(const glm::vec3 dimensions) { m_boundingDimensions = dimensions; }
	
	virtual glm::vec4* 				GetPoints( void ) { return m_points; }
	unsigned int					GetPointCount( void ) const { return m_pointCount; }

	friend bool						operator<( const Shape& a, const Shape& b );
	friend bool						operator==( const Shape& a, const Shape& b );

protected:
	virtual void					UpdateBoundingDimensions( void ) = 0;

	glm::vec3						m_boundingDimensions;
	glm::vec4*						m_points;
	unsigned int					m_pointCount;
	Transform						m_transform;
};

}

#endif //__SHAPE_H__