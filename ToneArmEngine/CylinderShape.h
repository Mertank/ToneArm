/*
========================

Copyright (c) 2014 Vinyl Games Studio

	CylinderShape

		Class that represents a cylinder shape.

		Created by: Karl Merten
		Created on: 29/05/2014

========================
*/

#ifndef __CYLINDERSHAPE_H__
#define __CYLINDERSHAPE_H__

#include <glm\glm.hpp>
#include "Shape.h"

namespace vgs {

class CylinderShape : public Shape {

	DECLARE_RTTI

public:
									CylinderShape( void );
	virtual							~CylinderShape( void );

	static CylinderShape*			CreateCylinderShape( const glm::vec3& position, float radius, float height, unsigned int subdivisions = 16 );
	
	float 							GetRadius( void ) const { return m_radius; }
	void 							SetRadius( float radius ) { m_radius = radius; m_boundingDimensions.x = m_boundingDimensions.z = radius * 2.0f; }
	
	float 							GetHeight( void ) const { return m_height; }
	void 							SetHeight( float height ) { m_height = height; m_boundingDimensions.y = height; }

	unsigned int					GetSubdivisions( void ) const { return m_subdivisions; }
	void							SetSubdivisions( unsigned int sub ) { m_subdivisions = sub; }

	virtual glm::vec4*				GetPoints( void );	
protected:
	virtual void					UpdateBoundingDimensions( void );
private:
	bool							InitializeCylinderShape( const glm::vec3& position, float radius, float height, unsigned int subdivisions );
	void							BuildPoints( void );

	float							m_radius;
	float							m_height;
	unsigned int					m_subdivisions;
	unsigned int					m_prevSubdivisions;
};

} //namepsace vgs

#endif //__CYLINDERSHAPE_H__