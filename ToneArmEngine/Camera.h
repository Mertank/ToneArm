/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Camera.h

		Base camera class

		Created by: Vladyslav Dolhopolov
		Created on: 4/25/2014 3:25:26 PM

========================
*/
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "MathHelper.h"
#include "Node.h"
#include <vector>
#include "Frustum.h"

#include "DebugCommand.h"

#define CAMERA_NODE_ID UINT_MAX

namespace vgs
{
	namespace CameraProjection {
		enum Value {
			PERSPECTIVE,
			ORTHOGRAPHIC
		};
	}

	struct Projection {
		float					width;
		float					height;
		float					nearClipping; 
		float					farClipping;
		float					FOV;
		CameraProjection::Value projectionType;

		Projection( void ) :
			width( 0.0f ),
			height( 0.0f ),
			nearClipping(  0.0f ),
			farClipping( 0.0f ),
			FOV( 30 ),
			projectionType( CameraProjection::ORTHOGRAPHIC )
		{}

		Projection( float w, float h, float n, float f, float fov, CameraProjection::Value p ) :
			width( w ),
			height( h ),
			nearClipping( n ),
			farClipping( f ),
			FOV( fov ),
			projectionType( p )
		{}
	};

	class Camera : public Node
	{
								DECLARE_RTTI
	public:
								Camera();
		virtual                 ~Camera() {}

		void                    SetViewportSize(int width, int height);

		glm::vec3				GetPosition() const { return m_transform.GetPosition(); }
		glm::quat				GetOrientation() const { return m_Orientation; }

		void					SetPosition(float x, float y, float z)	{ SetPosition( glm::vec3(x, y, z) ); }
		void					SetPosition(const glm::vec3& position)	{ m_transform.SetPosition( position ); }

		virtual glm::vec3       GetForward() const  { return this->GetOrientation() * LOCAL_FORWARD; }
		virtual glm::vec3       GetUp() const       { return this->GetOrientation() * LOCAL_UP; }
		virtual glm::vec3       GetRight() const    { return this->GetOrientation() * LOCAL_RIGHT; }

		const Projection&		GetProjection( void ) const { return m_currentProjection; }
		const glm::mat4&		GetProjectionMatrix() const { return m_projectionMatrix; };
		virtual glm::mat4       GetViewMatrix() const = 0;

		const Frustum&			GetFrustum( void ) { return m_frustum; }

		void					SetProjection( const Projection& projection );
		virtual void			Update( float dt ) override;

		// the camera's local basis vectors (default orientation)
		static const glm::vec3  LOCAL_FORWARD;
		static const glm::vec3  LOCAL_UP;
		static const glm::vec3  LOCAL_RIGHT;

		DECLARE_DEBUG_COMMAND_CALLBACK( vgs::Camera, switchCamera )
	protected:
		glm::quat				m_Orientation;       // HMMM... replace by a 3x4 matrix transform?

		// orientation as Euler angles
		float					m_Yaw;       // rotation about world y-axis
		float					m_Pitch;     // rotation about local x-axis

		bool					m_OrientationChanged;

		Frustum					m_frustum;
		Projection				m_currentProjection;
		glm::mat4				m_projectionMatrix;
	};

}

#endif __CAMERA_H__