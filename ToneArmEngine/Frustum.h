/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Frustum

		Representation of a frustrum.
		Can check if points, spheres, or boxes are in it. 
		
		http://zach.in.tu-clausthal.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html
		
		Created by: Karl Merten
		Created on: 04/06/2014

========================
*/

#ifndef __FRUSTRUM_H__
#define __FRUSTRUM_H__

#include <glm\glm.hpp>
#include <vector>

namespace vgs {

class Shape;
class BoxShape;
class CylinderShape;
class ColliderComponent;

namespace FrustumResult {
	enum Value {
		OUTSIDE,
		INTERSECT,
		INSIDE
	};
}

namespace PlaneDirection {
	enum Value {
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT,
		NEARP, //WTF!!?!?!??!?!?!?!
		FARP
	};
}

namespace FrusutmPoints {
	enum Value {
		FAR_TOPLEFT  = 0,
		FAR_TOPRIGHT,
		FAR_BOTTOMLEFT,
		FAR_BOTTOMRIGHT,

		NEAR_TOPLEFT,
		NEAR_TOPRIGHT,
		NEAR_BOTTOMLEFT,
		NEAR_BOTTOMRIGHT
	};
}

class Frustum {
public:
							Frustum( void );
	void					UpdateFrustumInternals( float angle, float ratio, float near, float far );
	void					UpdateFrustum( const glm::mat4& viewMat, const glm::mat4& projMat, const glm::vec3& camPosition );

	FrustumResult::Value	PointInFrustum( const glm::vec4& point ) const;
	FrustumResult::Value	BoxInFrustum( const BoxShape& box ) const;
	FrustumResult::Value	BoxInFrustum( const glm::vec3& pos, const glm::vec3& dimensions ) const;
	FrustumResult::Value	CylinderInFrustum( const CylinderShape& cyl ) const;

	void					ShapesInFrustum(std::vector<Shape*>&) const;
	void					CollidersInFrustum(std::vector<ColliderComponent*>&) const;

	glm::vec2				GetFrustumSizeAtDistance( float distance ) const;

	const glm::vec4*		GetPlanes( void ) const { return m_planes; }
	const glm::vec3*		GetPoints( void ) const { return m_points; }
private:
	float					m_near;
	float					m_far;
	float					m_aspectRatio;
	float					m_angle;
	float					m_tang;

	float					m_nearWidth;
	float					m_nearHeight;
	float					m_farWidth;
	float					m_farHeight;

	glm::vec4				m_planes[6];
	glm::vec3				m_points[8];
};

}

#endif //__FRUSTRUM_H__