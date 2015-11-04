/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Camera.cpp

		Base camera class

		Created by: Vladyslav Dolhopolov
		Created on: 4/25/2014 3:25:26 PM

========================
*/
#include "Camera.h"
#include "RenderModule.h"
#include "Engine.h"
#include "DebugConsoleModule.h"
#include "Scene.h"
#include "FreeLookCamera.h"
#include "FixedCamera.h"
#include "../Merrymen/GameScene.h"
#include "../Merrymen/Character.h"
#include "NetworkNode.h"

using namespace vgs;

INITIALIZE_DEBUG_COMMAND_CALLBACK( vgs::Camera, switchCamera )

IMPLEMENT_RTTI( Camera, Node )

const glm::vec3 Camera::LOCAL_FORWARD   (0.0f, 0.0f, -1.0f);
const glm::vec3 Camera::LOCAL_UP        (0.0f, 1.0f,  0.0f);
const glm::vec3 Camera::LOCAL_RIGHT     (1.0f, 0.0f,  0.0f);

Camera::Camera() : 
	m_Orientation(1.0f, 0.0f, 0.0f, 0.0f),		// identity
	m_Yaw(0.0f),
	m_Pitch(0.0f),
	m_OrientationChanged(false)
{
	m_projectionMatrix = glm::ortho( 0.0f, m_currentProjection.width, 0.0f, m_currentProjection.height, m_currentProjection.nearClipping, m_currentProjection.farClipping );
	m_frustum.UpdateFrustumInternals( m_currentProjection.FOV, m_currentProjection.width / m_currentProjection.height, m_currentProjection.nearClipping, m_currentProjection.farClipping );

}

void Camera::SetProjection( const Projection& projection ) {
	m_currentProjection = projection;

	if ( m_currentProjection.projectionType == CameraProjection::ORTHOGRAPHIC ) {
		m_projectionMatrix = glm::ortho( 0.0f, m_currentProjection.width, 0.0f, m_currentProjection.height, m_currentProjection.nearClipping, m_currentProjection.farClipping );
	} else {
		m_projectionMatrix = glm::perspective( m_currentProjection.FOV, ( m_currentProjection.width / m_currentProjection.height ), 
											   m_currentProjection.nearClipping, m_currentProjection.farClipping );
	}
	
	m_frustum.UpdateFrustumInternals( m_currentProjection.FOV, m_currentProjection.width / m_currentProjection.height, m_currentProjection.nearClipping, m_currentProjection.farClipping );

	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );

	UpdateCameraProjectionMessage* projMessage = renderer->GetRenderMessage<UpdateCameraProjectionMessage>();
	projMessage->Init( m_currentProjection );
	renderer->SendRenderMessage( projMessage );

	CameraTransformMessage* msg = renderer->GetRenderMessage<CameraTransformMessage>();
	msg->Init( 0, GetViewMatrix(), m_projectionMatrix, GetPosition() );
	renderer->SendRenderMessage( msg );
}

void Camera::Update( float dt ) {
	m_frustum.UpdateFrustum( GetViewMatrix(), GetProjectionMatrix(), GetPosition() );
}

/*
=========
Camera::switchCameraCallback

	Callback for switchCamera command
=========
*/
BEGIN_CALLBACK_FUNCTION( Camera, switchCamera )
	if ( args.size() > 0 ) {
		char* camType = args[0];
		if ( StringUtils::IsEqual( camType, "free" ) ) {
			Scene* currentScene = Engine::GetRunningScene();
			Camera* currentCamera = currentScene->GetCurrentCamera();
			FreeLookCamera* newCam = FreeLookCamera::Create();
			
			newCam->SetProjection( currentCamera->GetProjection() );
			newCam->SetPosition( currentCamera->GetPosition() );

			currentScene->RemoveChild( currentCamera->GetID() );
			currentScene->SetCurrentCamera( newCam );
			delete currentCamera;
		} else if ( StringUtils::IsEqual( camType, "fixed" ) ) {
			Scene* currentScene = Engine::GetRunningScene();
			Camera* currentCamera = currentScene->GetCurrentCamera();
			FixedCamera* newCam = FixedCamera::Create();

			merrymen::Character* character = NULL;

#ifdef SINGLE_PLAYER
			character = ( ( merrymen::GameScene* )currentScene )->GetPlayers()[0];
#else
			NetworkNode* node = NetworkNode::GetLocalPlayer();
			if ( node ) {
				character = node->GetParentCharacter();
			}
#endif
			newCam->InitWithTarget( character, 1000.0f, 1900.0f );
			newCam->SetProjection( currentCamera->GetProjection() );

			currentScene->RemoveChild( currentCamera->GetID() );
			currentScene->SetCurrentCamera( newCam );
			delete currentCamera;
		}
	}
END_CALLBACK_FUNCTION