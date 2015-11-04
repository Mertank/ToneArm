/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Shape

	Created by: Karl Merten
	Created on: 05/06/2014

========================
*/
#include "Shape.h"

namespace vgs {

IMPLEMENT_BASE_RTTI( Shape )
/*
============
Shape::Shape

	Shape default constructor
============
*/
Shape::Shape( void ) :
	m_points( NULL ),
	m_pointCount( 0 )
{}
/*
============
Shape::Shape

	Shape copy constructor
============
*/
Shape::Shape( const Shape& other ) {
	m_pointCount = other.m_pointCount;
	m_points = 	new glm::vec4[m_pointCount];
	memcpy( m_points, other.m_points, sizeof( glm::vec4 ) * m_pointCount );

	m_boundingDimensions = other.m_boundingDimensions;
	m_transform = other.m_transform;
}
/*
============
Shape::~Shape

	Shape destructor
============
*/
Shape::~Shape( void ) {
	delete[] m_points;
}
/*
============
Shape::SetRotation

	Sets the shape's rotation and updates it's bounding dimensions.
============
*/
void Shape::SetRotation( const glm::vec3& rot ) {
	if ( rot != m_transform.GetRotation() ) {
		m_transform.SetRotation( rot );
		UpdateBoundingDimensions();
	}
}
/*
============
operator<

	Less than operator for sorting shapes.
============
*/
bool operator<( const Shape& a, const Shape& b ) {
	if ( a.m_transform.GetPosition() != b.m_transform.GetPosition() ) {
		const glm::vec3& aPosition = a.m_transform.GetPosition();
		const glm::vec3& bPosition = b.m_transform.GetPosition();
		if ( aPosition.x != bPosition.x ) {
			return aPosition.x < bPosition.x;
		} else if ( aPosition.y != bPosition.y ) {
			return aPosition.y < bPosition.y;
		} else {
			return aPosition.z < bPosition.z;
		}
	} else if ( a.m_transform.GetRotation() != b.m_transform.GetRotation() ) {
		const glm::vec3& aRotation = a.m_transform.GetRotation();
		const glm::vec3& bRotation = b.m_transform.GetRotation();
		if ( aRotation.x != bRotation.x ) {
			return aRotation.x < bRotation.x;
		} else if ( aRotation.y != bRotation.y ) {
			return aRotation.y < bRotation.y;
		} else {
			return aRotation.z < bRotation.z;
		}
	} else {
		const glm::vec3& aDimensions = a.m_boundingDimensions;
		const glm::vec3& bDimensions = b.m_boundingDimensions;
		if ( aDimensions.x != bDimensions.x ) {
			return aDimensions.x < bDimensions.x;
		} else if ( aDimensions.y != bDimensions.y ) {
			return aDimensions.y < bDimensions.y;
		} else {
			return aDimensions.z < bDimensions.z;
		}
	}
}
/*
============
operator==

	Equals operator.
============
*/
bool operator==( const Shape& a, const Shape& b ) {
	return ( a.m_transform.GetPosition() == b.m_transform.GetPosition() &&
			 a.m_transform.GetRotation() == b.m_transform.GetRotation() &&
			 a.m_boundingDimensions == b.m_boundingDimensions );
}

}