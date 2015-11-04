/*
========================

Copyright (c) 2014 Vinyl Games Studio

	CameraProjectionData

		Stores to information about a camera for the renderer to use.

		Created by: Karl Merten
		Created on: 5/14/2014

========================
*/

#ifndef __CAMERAPROJECTIONDATA_H__
#define __CAMERAPROJECTIONDATA_H__

#include "Camera.h"
#include "MathHelper.h"

namespace vgs {

	class CameraProjectionData
	{
	public:
							CameraProjectionData( void );
							~CameraProjectionData( void );

		const glm::mat4&	GetViewMatrix()	const								{ return m_viewMatrix; }
		const glm::mat4&	GetProjectionMatrix() const							{ return m_projectionMatrix; }

		Projection&			GetProjection( void )								{ return m_projectionStruct; }
	
		void				SetViewMatrix(const glm::mat4& mat)					{ m_viewMatrix = mat; }
		void				SetProjectionMatrix(const glm::mat4& mat)			{ m_projectionMatrix = mat; }

		const glm::vec3&	GetPosition( void ) const							{ return m_position; }
		void				SetPosition( const glm::vec3& pos )					{ m_position = pos; }
	private:
		Projection			m_projectionStruct;

		glm::mat4			m_viewMatrix;
		glm::mat4			m_projectionMatrix;
		glm::vec3			m_position;

	}; //class CameraProjectionData

} //namespace vgs

#endif //__CAMERAPROJECTIONDATA_H__