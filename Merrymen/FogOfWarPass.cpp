/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	FogOfWarPass

	Created by: Karl Merten
	Created on: 01/07/2014

========================
*/
#include "FogOfWarPass.h"
#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/OpenGLRenderModule.h"
#include "../ToneArmEngine/OpenGLProgramManager.h"
#include "../ToneArmEngine/OpenGLProgram.h"
#include "../ToneArmEngine/Frustum.h"
#include "../ToneArmEngine/CameraProjectionData.h"
#include "../ToneArmEngine/MathHelper.h"

#include "CustomRenderPasses.h"

#include <SFML\Window.hpp>

namespace merrymen {

IMPLEMENT_RTTI( FogOfWarPass, vgs::RenderPass )

/*
=========
FogOfWarPass::FogOfWarPass

	FogOfWarPass default constructor
=========
*/
FogOfWarPass::FogOfWarPass( void ) :
	m_frustumMesh( 0 )
{}
/*
=========
FogOfWarPass::~FogOfWarPass

	FogOfWarPass destructor
=========
*/
FogOfWarPass::~FogOfWarPass( void ) {
	glDeleteBuffers( 1, &m_frustumMesh );
}
/*
=========
FogOfWarPass::CreateWithPriority

	FogOfWarPass creation method
====
*/
FogOfWarPass* FogOfWarPass::CreateWithPriority( unsigned int priority ) {
	FogOfWarPass* pass = new FogOfWarPass();
	if ( pass ) {
		pass->m_priority = priority;
		pass->m_passType = ( vgs::PassType::Value )( CustomRenderPasses::FOG_OF_WAR_PASS );
		return pass;
	}

	delete pass;
	return NULL;
}
/*
=========
FogOfWarPass::InitiailizePass

	FogOfWarPass init.
	Creates OpenGL Buffers
====
*/
bool FogOfWarPass::InitializePass( void* passData ) {
	//Make a VBO for the frustum mesh.
	glGenBuffers( 1, &m_frustumMesh );
	glBindBuffer( GL_ARRAY_BUFFER, m_frustumMesh );
	glBufferData( GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	return true;
}
/*
=========
FogOfWarPass::BeginPass

	Binds frame buffer.
	Resizes texture if required
====
*/
void FogOfWarPass::BeginPass( void ) {
	m_cameraData = &vgs::Engine::GetInstance()->GetModuleByType<vgs::OpenGLRenderModule>( vgs::EngineModuleType::RENDERER )->GetCameraData();
	
	glEnable( GL_STENCIL_TEST );
	glEnable( GL_DEPTH_TEST );

	glStencilMask( 0xFF ); //This thing needs to be set before clear.
	//Set everything I draw to 1 in stencil
	glStencilFunc( GL_ALWAYS, 1, 0xFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	//Don't write to color/depth buffer
	glDepthMask( GL_FALSE );
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

	glClear( GL_STENCIL_BUFFER_BIT );
}
/*
=========
FogOfWarPass::ProcessRenderable

	Renders fog.
====
*/
void FogOfWarPass::ProcessRenderable( vgs::Renderable* obj ) {
	obj->PreRender();
	obj->Render();
	obj->PostRender();

	m_fogPosition = glm::vec3( obj->GetModelMatrix()[3] ); //This should only happen once.
}
/*
=========
FogOfWarPass::FinishPass

	Alpha masks rendered texture to screen.
====
*/
void FogOfWarPass::FinishPass( void ) {
	vgs::RenderModule* renderer = vgs::Engine::GetInstance()->GetModuleByType<vgs::RenderModule>( vgs::EngineModuleType::RENDERER );
	vgs::CameraProjectionData& cam = renderer->GetCameraData();
	vgs::Projection& projection = cam.GetProjection();

	const glm::vec3* frustumPoints = renderer->GetCameraFrustum().GetPoints();

	//Find the mesh that intersects the frusutm at the fog's position
	glm::vec4 meshCorners[4];

	vgs::MathHelper::RayPlaneIntersection( frustumPoints[vgs::FrusutmPoints::NEAR_BOTTOMLEFT], 
										   ( frustumPoints[vgs::FrusutmPoints::FAR_BOTTOMLEFT] - frustumPoints[vgs::FrusutmPoints::NEAR_BOTTOMLEFT] ),
										   glm::vec3( 0.0f, -1.0f, 0.0f ),
										   m_fogPosition,
										   meshCorners[0] );

	vgs::MathHelper::RayPlaneIntersection( frustumPoints[vgs::FrusutmPoints::NEAR_BOTTOMRIGHT], 
										   ( frustumPoints[vgs::FrusutmPoints::FAR_BOTTOMRIGHT] - frustumPoints[vgs::FrusutmPoints::NEAR_BOTTOMRIGHT] ),
										   glm::vec3( 0.0f, -1.0f, 0.0f ),
										   m_fogPosition,
										   meshCorners[2] );

	vgs::MathHelper::RayPlaneIntersection( frustumPoints[vgs::FrusutmPoints::NEAR_TOPLEFT], 
										   ( frustumPoints[vgs::FrusutmPoints::FAR_TOPLEFT] - frustumPoints[vgs::FrusutmPoints::NEAR_TOPLEFT] ),
										   glm::vec3( 0.0f, -1.0f, 0.0f ),
										   m_fogPosition,
										   meshCorners[1] );

	vgs::MathHelper::RayPlaneIntersection( frustumPoints[vgs::FrusutmPoints::NEAR_TOPRIGHT], 
										   ( frustumPoints[vgs::FrusutmPoints::FAR_TOPRIGHT] - frustumPoints[vgs::FrusutmPoints::NEAR_TOPRIGHT] ),
										   glm::vec3( 0.0f, -1.0f, 0.0f ),
										   m_fogPosition,
										   meshCorners[3] );

	glEnable( GL_BLEND );
	glDisable( GL_DEPTH_TEST ); //Don't depth test this quad.
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); 
	
	glDepthMask( GL_TRUE );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	
	glStencilFunc( GL_NOTEQUAL, 1, 0xFF );
	glStencilMask( 0x00 );
	
	vgs::OpenGLProgram* const activeProgram = vgs::OpenGLProgramManager::GetInstance()->UseProgramWithFeatures( vgs::ProgramFeatures::NONE );

	activeProgram->SetUniform( "u_DiffuseColor", glm::vec3( 0.0f, 0.0f, 0.0f ) );
	activeProgram->SetUniform( "u_ViewMatrix", cam.GetViewMatrix() );
	activeProgram->SetUniform( "u_ProjectionMatrix", cam.GetProjectionMatrix() );
	activeProgram->SetUniform( "u_ModelMatrix", glm::mat4( 1.0 ) );
	activeProgram->SetUniform( "u_Opacity", 0.4f );

	glBindBuffer( GL_ARRAY_BUFFER, m_frustumMesh );
	glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * 16, &meshCorners[0], GL_DYNAMIC_DRAW );

	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof( float ) * 4, NULL );

	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	glDisableVertexAttribArray( 0 );
		
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glDisable( GL_STENCIL_TEST );
	glDisable( GL_BLEND );
}

}