/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	CylinderShape

	Created by: Karl Merten
	Created on: 29/05/2014

========================
*/

#include "CylinderShape.h"
#include "BoxShape.h"

namespace vgs {

IMPLEMENT_RTTI( CylinderShape, Shape )
/*
==========
CylinderShape::CylinderShape

	CylinderShape default constructor
==========
*/
CylinderShape::CylinderShape( void ) :
	m_height( 0.0f ),
	m_radius( 0.0f ),
	m_subdivisions( 0 ),
	m_prevSubdivisions( 0 )
{}
/*
==========
CylinderShape::~CylinderShape

	CylinderShape destructor
==========
*/
CylinderShape::~CylinderShape( void ) 
{}
/*
==========
CylinderShape::CreateCylinderShape

	Creates a cylinder shape
==========
*/
CylinderShape* CylinderShape::CreateCylinderShape( const glm::vec3& position, float radius, float height, unsigned int subdivisions ) {
	CylinderShape* cylinder = new CylinderShape();
	if ( cylinder && cylinder->InitializeCylinderShape( position, radius, height, subdivisions ) ) {
		return cylinder;
	} else {
		delete cylinder;
		return NULL;
	}
}
/*
==========
CylinderShape::InitializeCylinderShape

	Initializes the shape
==========
*/
bool CylinderShape::InitializeCylinderShape( const glm::vec3& position, float radius, float height, unsigned int subdivisions ) {

	SetPosition(position);

	m_subdivisions	= subdivisions;
	m_radius		= radius;
	m_height		= height;
	m_points		= new glm::vec4[m_subdivisions * 2];
	m_pointCount	= m_subdivisions * 2;

	m_boundingDimensions.x = m_boundingDimensions.z = radius * 2.0f;
	m_boundingDimensions.y = height;

	return true;
}
/*
==========
CylinderShape::GetPoints

	Returns the points on the cylinder
==========
*/
glm::vec4* CylinderShape::GetPoints( void ) {
	if ( m_prevSubdivisions != m_subdivisions ) {
		delete[] m_points;
		m_points = new glm::vec4[m_subdivisions * 2];
		m_prevSubdivisions = m_subdivisions;
		BuildPoints();
	} else {
		glm::vec4 comparePoint( m_radius, -m_height * 0.5f, 0.0f, 1.0f );
		if ( m_points[0].x != comparePoint.x || m_points[0].y != comparePoint.y || m_points[0].z != comparePoint.z ) {
			BuildPoints();
		}
	}

	return m_points;
}
/*
==========
CylinderShape::BuildPoints

	Generates the points for the cylinder.
==========
*/
void CylinderShape::BuildPoints( void ) {
	float angleIncrements = 360.0f / m_subdivisions;
	float currentAngle = 0.0f;
	glm::vec3 direction( 0.0f );
	const glm::mat4& modelMat = m_transform.GetModelMatrix();
	
	for ( unsigned int i = 0; i < m_subdivisions; ++i ) {
		direction.x = cos( glm::radians( currentAngle ) );
		direction.z = sin( glm::radians( currentAngle ) );

		direction	= glm::normalize( direction );
		direction   = direction * m_radius;

		m_points[i] = glm::vec4( direction.x, m_height * 0.5f, direction.z, 1.0f );
		m_points[i + m_subdivisions] = glm::vec4( direction.x, -m_height * 0.5f, direction.z, 1.0f );

		m_points[i] = modelMat * m_points[i];
		m_points[i + m_subdivisions] = modelMat * m_points[i + m_subdivisions];

		currentAngle += angleIncrements;
	}
}
/*
==========
CylinderShape::UpdateBoundingDimensions

	Updates the bounding dimensions of the cylinder.
==========
*/
void CylinderShape::UpdateBoundingDimensions( void ) {
	//Convert it to a box and use that
	BoxShape* box = BoxShape::CreateFromCylinder( *this );
	m_boundingDimensions = box->GetBoundingDimensions();
	delete box;
}
} //namespace vgs