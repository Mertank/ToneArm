/*
========================

Copyright (c) 2014 Vinyl Games Studio

	BoxShape

		Class that represents a 3D box.

		Created by: Karl Merten
		Created on: 01/05/2014

========================
*/

#ifndef __BOXSHAPE_H__
#define __BOXSHAPE_H__

#include <glm\glm.hpp>
#include "Shape.h"
#include "MathHelper.h"

namespace vgs {

class CylinderShape;

class BoxShape : public Shape {
	DECLARE_RTTI
public:
									BoxShape( void );
									BoxShape( const BoxShape& other );
	virtual							~BoxShape( void );

	static BoxShape*				CreateBoxShape( const glm::vec3& position, const glm::vec3& dimensions );
	static BoxShape*				CreateFromCylinder( const CylinderShape& cyl );

	virtual const glm::vec3&		GetBoundingDimensions( void ) const { return m_dimensions; }
	const glm::vec3&				GetDimensions( void ) const { return m_dimensions; }
	void							SetDimensions( const glm::vec3& dimensions );

	glm::vec3						GetPositiveVertex( const glm::vec3& normal ) const;
	glm::vec3						GetNegativeVertex( const glm::vec3& normal ) const;

	MathHelper::RectF&				GetRect( void );

	virtual glm::vec4*				GetPoints( void ); 
	virtual void					SetPosition( const glm::vec3& position );
	virtual void					SetRotation( const glm::vec3& rot );

protected:
	virtual void					UpdateBoundingDimensions( void );
private:
	bool							InitializeBoxShape( const glm::vec3& position, const glm::vec3& dimensions );

	glm::vec3						m_dimensions;
	MathHelper::RectF				m_rect;
};

} //namepsace vgs

#endif //__BOXSHAPE_H__