/*
========================
	
	Copyright (c) 2014 Vinyl Games Studio

	BoxShape

	Created by: Karl Merten
	Created on: 01/05/2014

========================
*/
#include "BoxShape.h"
#include "CylinderShape.h"

namespace vgs {

IMPLEMENT_RTTI( BoxShape, Shape )
/*
==========
BoxShape::BoxShape

	BoxShape default constructor
==========
*/
BoxShape::BoxShape( void ) :
	m_dimensions( 1.0f, 1.0f, 1.0f )
{}
/*
==========
BoxShape::BoxShape

	BoxShape copy constructor
==========
*/
BoxShape::BoxShape( const BoxShape& other ) :
	Shape( other )
{
	m_dimensions = other.m_dimensions;
	m_rect = other.m_rect;

}
/*
==========
BoxShape::BoxShape

	BoxShape destructor
==========
*/
BoxShape::~BoxShape( void ) 
{}
/*
==========
BoxShape::CreateBoxShape

	Creates a bounding box copy
==========
*/
BoxShape* BoxShape::CreateBoxShape( const glm::vec3& position, const glm::vec3& dimensions ) {
	BoxShape* box = new BoxShape();
	if ( box && box->InitializeBoxShape( position, dimensions ) ) {
		return box;
	} else {
		delete box;
		return NULL;
	}
}
/*
==========
BoxShape::CreateFromCylinder

	Creates a bounding box around the cylinder
==========
*/
BoxShape* BoxShape::CreateFromCylinder( const CylinderShape& cyl ) {
	BoxShape* box = new BoxShape();	
	if ( box && box->InitializeBoxShape( cyl.GetPosition(), glm::vec3( cyl.GetRadius() * 2.0f, cyl.GetHeight(), cyl.GetRadius() * 2.0f ) ) ) {
		box->SetPosition( cyl.GetPosition() );
		box->SetRotation( cyl.GetRotation() );
		return box;
	} else {
		delete box;
		return NULL;
	}
}
/*
==========
BoxShape::InitializeBoxShape

	Initializes the shape
==========
*/
bool BoxShape::InitializeBoxShape( const glm::vec3& position, const glm::vec3& dimensions ) {

	SetPosition(position);
	SetDimensions( dimensions );
	m_pointCount = 8;
	m_points = new glm::vec4[8];

	return true;
}
/*
==========
BoxShape::GetPoints

	Gets the points required to make up the box
==========
*/
glm::vec4* BoxShape::GetPoints( void ) {
	glm::vec4 comparePoint( -m_dimensions * 0.5f, 1.0f );
	const glm::mat4& modelMat = m_transform.GetModelMatrix();
	comparePoint = modelMat * comparePoint;

	//Points may need updating
	if ( m_points[0].x != comparePoint.x || m_points[0].y != comparePoint.y || m_points[0].z != comparePoint.z ) {
		glm::vec4 min( -m_dimensions * 0.5f, 1.0f );
		glm::vec4 max(  m_dimensions * 0.5f, 1.0f );
		
		min.w = max.w = 1.0f;

		m_points[0]	= glm::vec4( min.x, max.y, min.z, 1.0f );
		m_points[1]	= glm::vec4( min.x, max.y, max.z, 1.0f );
		m_points[2]	= glm::vec4( max.x, max.y, min.z, 1.0f );
		m_points[3]	= glm::vec4( max.x, max.y, max.z, 1.0f );

		m_points[4]	= glm::vec4( min.x, min.y, min.z, 1.0f );
		m_points[5]	= glm::vec4( min.x, min.y, max.z, 1.0f );
		m_points[6]	= glm::vec4( max.x, min.y, min.z, 1.0f );
		m_points[7]	= glm::vec4( max.x, min.y, max.z, 1.0f );

		for ( int i = 0; i < 8; ++i ) {
			m_points[i] = modelMat * m_points[i];
		}
	}

	return m_points;
}
/*
==========
BoxShape::SetDimensions

	Sets the dimesnions of the box
==========
*/
void BoxShape::SetDimensions( const glm::vec3& dim ) {
	m_dimensions = dim;
	m_rect.Width  = m_dimensions.x;
	m_rect.Height = m_dimensions.z;
	UpdateBoundingDimensions();
}
/*
==========
BoxShape::GetPositiveVertex

	Returns the most positive vertex based on a normal
==========
*/
glm::vec3 BoxShape::GetPositiveVertex( const glm::vec3& normal ) const {
	const glm::vec3& position = m_transform.GetPosition();
	glm::vec3 positive( position - ( m_boundingDimensions * 0.5f ) );

	if ( normal.x >= 0.0f ) {
		positive.x = position.x + ( m_boundingDimensions.x * 0.5f );
	}
	if ( normal.y >= 0.0f ) {
		positive.y = position.y + ( m_boundingDimensions.y * 0.5f );
	}
	if ( normal.z >= 0.0f ) {
		positive.z = position.z + ( m_boundingDimensions.z * 0.5f );
	}

	return positive;
}
/*
==========
BoxShape::GetNegativeVertex

	Returns the most negative vertex based on a normal
==========
*/
glm::vec3 BoxShape::GetNegativeVertex( const glm::vec3& normal ) const {
	const glm::vec3& position = m_transform.GetPosition();
	glm::vec3 negative( position + ( m_dimensions * 0.5f ) );

	if ( normal.x >= 0.0f ) {
		negative.x = position.x - ( m_dimensions.x * 0.5f );
	}
	if ( normal.y >= 0.0f ) {
		negative.y = position.y - ( m_dimensions.y * 0.5f );
	}
	if ( normal.z >= 0.0f ) {
		negative.z = position.z - ( m_dimensions.z * 0.5f );
	}

	return negative;
}
/*
==========
BoxShape::UpdateBoundingDimensions

	Updates the bounding dimensions of the box,
	( It grows when rotated. )
==========
*/
void BoxShape::UpdateBoundingDimensions( void ) {
	if ( m_transform.GetRotation().x != 0.0f ||
		 m_transform.GetRotation().y != 0.0f ||
		 m_transform.GetRotation().z != 0.0f ) {
		m_boundingDimensions = MathHelper::GetAABBDimensions( *this );
	} else {
		m_boundingDimensions = m_dimensions;
	}
}
/*
==========
BoxShape::GetRect

	Gets the 2D rect representation of the box.
	Mainly for raycasting
==========
*/
MathHelper::RectF& BoxShape::GetRect( void ) {	
	return m_rect;
}


void BoxShape::SetPosition( const glm::vec3& pos ) {
	Shape::SetPosition( pos );
	m_rect.Center = pos;
}

void BoxShape::SetRotation( const glm::vec3& rot ) {
	Shape::SetRotation( rot );
	m_rect.Angle = -rot.y;
}
} //namespace vgs