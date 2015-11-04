/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Frustum

	Created by: Karl Merten
	Created on: 04/06/2014

========================
*/
#include "Frustum.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "BoxShape.h"
#include "CylinderShape.h"
#include "SpacePartitionTree.h"
#include "Scene.h"
#include "Camera.h"

namespace vgs {
/*
==========
Frustum::Frustum

	Frustum default constructor
==========
*/
Frustum::Frustum( void ) :
	m_aspectRatio( 0.0f ),
	m_near( 0.01f ),
	m_far( 100.0f ),
	m_angle( 0.0f ),
	m_tang( 0.0f ),
	m_nearWidth( 0.0f ),
	m_nearHeight( 0.0f ),
	m_farWidth( 0.0f ),
	m_farHeight( 0.0f )
{}
/*
==========
Frustum::UpdateFrustumInternals

	Updates the internals of the frustum.
	Shouldn't be called too often.
==========
*/
void Frustum::UpdateFrustumInternals( float angle, float ratio, float near, float far ) {
	m_aspectRatio = ratio;
	m_angle = angle;
	m_near = near;
	m_far = far;

	m_tang = ( float )tan( glm::radians( angle ) * 0.5 ) * 2.0f;
	
	m_nearHeight = near * m_tang;
	m_nearWidth  = m_nearHeight * ratio;

	m_farHeight = far * m_tang;
	m_farWidth  = m_farHeight * ratio;	
}
/*
==========
Frustum::UpdateFrustum

	Updates position and lookat of the frustum.	
==========
*/
void Frustum::UpdateFrustum( const glm::mat4& viewMat, const glm::mat4& projMat, const glm::vec3& camPosition ) {
	/*
	glm::vec3 direction;
	glm::vec3 nearCenter;
	glm::vec3 farCenter;
	glm::vec3 X;
	glm::vec3 Y;
	glm::vec3 Z;

	Z = position - lookAt; //opposite of look at
	Z = glm::normalize( Z );

	X = glm::cross( up,  Z ); //Right vector
	X = glm::normalize( X );

	Y = glm::cross( Z, X ); //Real "up"

	nearCenter = position - Z * m_near;
	farCenter = position - Z * m_far;

	//4 corners of near plane
	m_nearTopLeft = nearCenter + Y * m_nearHeight - X * m_nearWidth;
	m_nearTopRight = nearCenter + Y * m_nearHeight + X * m_nearWidth;
	
	m_nearBottomLeft = nearCenter - Y * m_nearHeight - X * m_nearWidth;
	m_nearBottomRight = nearCenter - Y * m_nearHeight + X * m_nearWidth;

	//4 corners of far plane
	m_farTopLeft = farCenter + Y * m_farHeight - X * m_farWidth;
	m_farTopRight = farCenter + Y * m_farHeight + X * m_farWidth;
	
	m_farBottomLeft = farCenter - Y * m_farHeight - X * m_farWidth;
	m_farBottomRight = farCenter - Y * m_farHeight + X * m_farWidth;

	m_planes[PlaneDirection::TOP].FromPoints( m_nearTopRight, m_nearBottomLeft, m_farTopLeft );
	m_planes[PlaneDirection::BOTTOM].FromPoints( m_nearBottomLeft, m_nearBottomRight, m_farBottomRight );
	
	m_planes[PlaneDirection::LEFT].FromPoints( m_nearTopLeft, m_nearBottomLeft, m_farBottomLeft );
	m_planes[PlaneDirection::RIGHT].FromPoints( m_nearBottomRight, m_nearTopRight, m_farBottomRight );
	
	m_planes[PlaneDirection::NEARP].FromPoints( m_nearTopLeft, m_nearTopRight, m_nearBottomRight );
	m_planes[PlaneDirection::FARP].FromPoints( m_farTopRight, m_farTopLeft, m_farBottomLeft );
	*/
	
	glm::mat4 viewProjMat = projMat * viewMat;
	viewProjMat = glm::transpose( viewProjMat );

	m_planes[PlaneDirection::LEFT] = viewProjMat[3] + viewProjMat[0];
	m_planes[PlaneDirection::RIGHT] = viewProjMat[3] - viewProjMat[0];
	
	m_planes[PlaneDirection::TOP] = viewProjMat[3] - viewProjMat[1];
	m_planes[PlaneDirection::BOTTOM] = viewProjMat[3] + viewProjMat[1];
	
	m_planes[PlaneDirection::NEARP] = viewProjMat[3] + viewProjMat[2];
	m_planes[PlaneDirection::FARP] = viewProjMat[3] - viewProjMat[2];

	for ( int i = 0; i < 6; ++i ) {
		m_planes[i] /= glm::length( glm::vec3( m_planes[i] ) );
	}

	glm::vec3 forward = -glm::vec3( viewMat[0][2], viewMat[1][2], viewMat[2][2] );
	glm::vec3 up	  = -glm::vec3( viewMat[0][1], viewMat[1][1], viewMat[2][1] );
	glm::vec3 right	  =  glm::vec3( viewMat[0][0], viewMat[1][0], viewMat[2][0] );

	glm::vec3 farClipping	= camPosition + ( forward * m_far );
	glm::vec3 nearClipping	= camPosition + ( forward * m_near );

	m_points[FrusutmPoints::FAR_TOPLEFT]	 = farClipping + ( up * ( m_farHeight * 0.5f ) ) - ( right * ( m_farWidth * 0.5f ) );
	m_points[FrusutmPoints::FAR_TOPRIGHT]	 = farClipping + ( up * ( m_farHeight * 0.5f ) ) + ( right * ( m_farWidth * 0.5f ) );
	m_points[FrusutmPoints::FAR_BOTTOMLEFT]  = farClipping - ( up * ( m_farHeight * 0.5f ) ) - ( right * ( m_farWidth * 0.5f ) );
	m_points[FrusutmPoints::FAR_BOTTOMRIGHT] = farClipping - ( up * ( m_farHeight * 0.5f ) ) + ( right * ( m_farWidth * 0.5f ) );

	m_points[FrusutmPoints::NEAR_TOPLEFT]	  = nearClipping + ( up * ( m_nearHeight * 0.5f ) ) - ( right * ( m_nearWidth * 0.5f ) );
	m_points[FrusutmPoints::NEAR_TOPRIGHT]	  = nearClipping + ( up * ( m_nearHeight * 0.5f ) ) + ( right * ( m_nearWidth * 0.5f ) );
	m_points[FrusutmPoints::NEAR_BOTTOMLEFT]  = nearClipping - ( up * ( m_nearHeight * 0.5f ) ) - ( right * ( m_nearWidth * 0.5f ) );
	m_points[FrusutmPoints::NEAR_BOTTOMRIGHT] = nearClipping - ( up * ( m_nearHeight * 0.5f ) ) + ( right * ( m_nearWidth * 0.5f ) );
}
/*
==========
Frustum::PointInFrustum

	Checks if a point is in the frustum.
==========
*/
FrustumResult::Value Frustum::PointInFrustum( const glm::vec4& point ) const {
	for ( int i = 0; i < 6; ++i ) {
		if ( ( m_planes[i].x * point.x ) + ( m_planes[i].y * point.y ) + ( m_planes[i].z * point.z ) + m_planes[i].w < 0 ) {
			return FrustumResult::OUTSIDE;
		}
	}

	return FrustumResult::INSIDE;
}
/*
==========
Frustum::BoxInFrustum

	Checks if a box is in the frustum.
==========
*/
FrustumResult::Value Frustum::BoxInFrustum( const BoxShape& box ) const {

	glm::vec3 positiveVertex, negativeVertex;

	FrustumResult::Value ret = FrustumResult::INSIDE;

	for ( int i = 0; i < 6; ++i ) {
		negativeVertex = box.GetNegativeVertex( glm::vec3( m_planes[i] ) );
		positiveVertex = box.GetPositiveVertex( glm::vec3( m_planes[i] )  );

		if ( glm::dot( glm::vec3( m_planes[i] ), positiveVertex ) + m_planes[i].w < 0 ) {
	 		return FrustumResult::OUTSIDE;
		} else if ( glm::dot( glm::vec3( m_planes[i] ), negativeVertex ) + m_planes[i].w < 0 ) {
			ret = FrustumResult::INTERSECT;
		}
	}

	return ret;

	/*
	glm::vec3 positiveVertex;
		glm::vec3 negativeVertex;

		const Transform& boxTransform = box.GetImmutableTransform();

		if ( std::abs( boxTransform.GetRotation().y ) < 0.001f ) {
			positiveVertex = box.GetPositiveVertex( m_planes[i].GetNormal() );
			negativeVertex = box.GetNegativeVertex( m_planes[i].GetNormal() );
		} else {
			glm::vec3 normalBox( glm::dot( boxTransform.GetRight(), m_planes[i].GetNormal() ),
								 glm::dot( boxTransform.GetUp(), m_planes[i].GetNormal() ),
								 glm::dot( boxTransform .GetForward(), m_planes[i].GetNormal() ) );

			positiveVertex = box.GetPositiveVertex( normalBox );
			negativeVertex = box.GetNegativeVertex( normalBox );
		}

		if ( m_planes[i].Distance( positiveVertex ) < 0 ) {
			return FrustumResult::OUTSIDE;
		} else if ( m_planes[i].Distance( negativeVertex ) < 0 ) {
			return FrustumResult::INTERSECT;
		}
	}
	*/
}
/*
==========
Frustum::BoxInFrustum

	Checks if a box is in the frustum.
==========
*/
FrustumResult::Value Frustum::BoxInFrustum( const glm::vec3& pos, const glm::vec3& dimensions ) const {
	BoxShape* box = BoxShape::CreateBoxShape( pos, dimensions );

	FrustumResult::Value ret = BoxInFrustum( *box );

	delete box;
	return ret;
}
/*
==========
Frustum::CylinderInFrustum

	Checks if a cylinder is in the frustum.
==========
*/
FrustumResult::Value Frustum::CylinderInFrustum( const CylinderShape& cyl ) const {
	return BoxInFrustum( cyl.GetPosition(), cyl.GetBoundingDimensions() );
}

//
// removes from the passed vector all the shapes which are outside of the frustum
//
void Frustum::ShapesInFrustum(std::vector<Shape*>& shapesToCheck) const {

	/*for ( unsigned int i = 0; i < shapesToCheck.size(); i ++) {

		if((shapesToCheck[i])->GetRTTI() == CylinderShape::rtti) {

			CylinderShape* cylinder = static_cast<CylinderShape*>(shapesToCheck[i]);

			if (!CylinderInFrustum(*cylinder)) {
				shapesToCheck.erase(shapesToCheck.begin() + i);

			}
		}
		else if ((shapesToCheck[i])->GetRTTI() == BoxShape::rtti) {

			BoxShape* box = static_cast<BoxShape*>(shapesToCheck[i]);

			if (!BoxInFrustum(*box)) {
				shapesToCheck.erase(shapesToCheck.begin() + i);
			}
		}
	}*/

	std::vector<Shape*> inFrustum;

	BEGIN_STD_VECTOR_ITERATOR( Shape*, shapesToCheck )
		if( currentItem->GetRTTI() == CylinderShape::rtti ) {
			CylinderShape* cylinder = static_cast<CylinderShape*>( currentItem );

			if ( CylinderInFrustum( *cylinder ) ) {
				inFrustum.push_back( currentItem );	
			}
		} else if ( currentItem->GetRTTI() == BoxShape::rtti ) {
			BoxShape* box = static_cast<BoxShape*>( currentItem );

			if ( BoxInFrustum( *box ) ) {
				inFrustum.push_back( currentItem );					
			}
		}
	END_STD_VECTOR_ITERATOR

	shapesToCheck.swap( inFrustum );
}

//
// removes from the passed vector all the colliders which are outside of the frustum
//
void Frustum::CollidersInFrustum(std::vector<ColliderComponent*>& shapesToCheck) const {

	std::vector<ColliderComponent*> inFrustum;

	BEGIN_STD_VECTOR_ITERATOR( ColliderComponent*, shapesToCheck )
		if( currentItem->GetRTTI() == CylinderCollider::rtti ) {

			CylinderCollider* collider = static_cast<CylinderCollider*>(currentItem);

			CylinderShape* cylinder = static_cast<CylinderShape*>( collider->GetShape() );

			if ( CylinderInFrustum( *cylinder ) ) {
				inFrustum.push_back( collider );	
			}
		} else if ( currentItem->GetRTTI() == BoxCollider::rtti ) {

			BoxCollider* collider = static_cast<BoxCollider*>(currentItem);

			BoxShape* box = static_cast<BoxShape*>( collider->GetShape() );

			if ( BoxInFrustum( *box ) ) {
				inFrustum.push_back( collider );					
			}
		}
	END_STD_VECTOR_ITERATOR

	shapesToCheck.swap( inFrustum );
}
/*
==========
Frustum::GetFrustumSizeAtDistance

	Returns the dimensions of the frustum a set distance from the camera.
==========
*/
glm::vec2 Frustum::GetFrustumSizeAtDistance( float distance ) const {
	glm::vec2 dimensions;
	
	dimensions.y = ( float )( 2.0 * distance * m_tang );
	dimensions.x = dimensions.y * m_aspectRatio;

	return dimensions;
}
}