/*
========================

Copyright (c) 2014 Vinyl Games Studio

	FreeLookCamera.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 4/25/2014 3:37:55 PM

========================
*/
#include <SFML\System.hpp>
#include <SFML\Window.hpp>

#include "FreeLookCamera.h"
#include "MathHelper.h"
#include "RunningInfo.h"

using namespace vgs;

IMPLEMENT_RTTI( FreeLookCamera, Camera )

FreeLookCamera::FreeLookCamera() :
	m_renderableModule(nullptr),
	m_Speed(10),								// world units / second
	m_MouseSpeed(MathHelper::PI / 1000.0f)	// radians / pixel
{
	UpdateOrientation();
}

bool FreeLookCamera::Init()
{
	m_Speed = 250.0f;
	RunningInfo* info = Engine::GetInstance()->GetRunningInfo();
	SetProjection( Projection( info->contextWidth, info->contextHeight, 1.0f, 15000.0f, 30, CameraProjection::PERSPECTIVE ) );	

	return true;
}

void FreeLookCamera::SetYaw( float rad ) {
	m_Yaw = rad;
	
	if (m_Yaw > MathHelper::PI)
	{
		m_Yaw -= MathHelper::TWO_PI;
	}
	else if (m_Yaw < -MathHelper::PI)
	{
		m_Yaw += MathHelper::TWO_PI;
	}

	m_OrientationChanged = true;
}

void FreeLookCamera::SetPitch( float rad ) {
	m_Pitch = rad;
	
	if (m_Pitch > MathHelper::PI)
	{
		m_Pitch -= MathHelper::TWO_PI;
	}
	else if (m_Pitch < -MathHelper::PI)
	{
		m_Pitch += MathHelper::TWO_PI;
	}

	m_OrientationChanged = true;
}

void FreeLookCamera::Yaw(float radians)
{
	m_Yaw += radians;

	// keep angle in standard range
	if (m_Yaw > MathHelper::PI)
	{
		m_Yaw -= MathHelper::TWO_PI;
	}
	else if (m_Yaw < -MathHelper::PI)
	{
		m_Yaw += MathHelper::TWO_PI;
	}

	m_OrientationChanged = true;
}

void FreeLookCamera::Pitch(float radians)
{
	m_Pitch += radians;

	const float maxPitch = glm::radians(85.0f);
	const float minPitch = -glm::radians(85.0f);

	// limit pitch range
	if (m_Pitch > maxPitch)
	{
		m_Pitch = maxPitch;
	}
	else if (m_Pitch < minPitch)
	{
		m_Pitch = minPitch;
	}

	m_OrientationChanged = true;
}

void FreeLookCamera::Update(float dt)
{
	static sf::Vector2i prevMousePos;
	// will send updated view and projection matricies to the render module
	// if position or rotation changed
	static bool transformChanged = true;

	sf::Vector2i mouseDelta = sf::Mouse::getPosition() - prevMousePos;
	
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && (mouseDelta.x != 0 || mouseDelta.y != 0))
	{
		int dx = mouseDelta.x;
		int dy = mouseDelta.y;

		if (dx != 0) {
			Yaw(-dx * m_MouseSpeed);
		}

		if (dy != 0) {
			Pitch(dy * m_MouseSpeed);
		}
		transformChanged = true;
	}

	prevMousePos = sf::Mouse::getPosition();

	// recompute forward, right, and up vectors if needed
	if (m_OrientationChanged)
	{
		UpdateOrientation();
		transformChanged = true;
	}

	// move vector determined from key states
	glm::vec3 localMoveVec(0.0f, 0.0f, 0.0f);

	// move forward and back
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		localMoveVec.z += 1;
		transformChanged = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		localMoveVec.z -= 1;
		transformChanged = true;
	}

	// move left and right
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		localMoveVec.x -= 1;
		transformChanged = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		localMoveVec.x += 1;
		transformChanged = true;
	}

	// move up and down
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		localMoveVec.y += 1;
		transformChanged = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		localMoveVec.y -= 1;
		transformChanged = true;
	}

	float localMoveLen = glm::length(localMoveVec);

	if (localMoveLen > 0)
	{
		// determine motion speed
		float speed = m_Speed;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		{
			speed *= 5;
		}

		// normalize and scale the local move vector
		localMoveVec *= speed * dt / localMoveLen;

		// apply translation in world space (relative to camera orientation)
		SetPosition( m_transform.GetPosition() + ( localMoveVec.z * m_transform.GetForward() + localMoveVec.x * m_transform.GetRight() + localMoveVec.y * m_transform.GetUp() ) );
	}

	if (transformChanged)
	{
		// send new transform to the render module
		CameraTransformMessage* msg = m_renderableModule->GetRenderMessage<CameraTransformMessage>();
		msg->Init( 0, GetViewMatrix(), GetProjectionMatrix(), GetPosition() );
		m_renderableModule->SendRenderMessage( msg );

		transformChanged = false;
	}

	Camera::Update( dt );
}

void FreeLookCamera::UpdateOrientation()
{
	// yaw about world y-axis
	glm::quat yawQuat = MathHelper::CreateQuaternion(m_Yaw, glm::vec3(0.0f, 1.0f, 0.0f));

	// get the rotated local x-axis
	glm::vec3 right = yawQuat * glm::vec3(1.0f, 0.0f, 0.0f);

	// pitch about local x-axis
	glm::quat pitchQuat = MathHelper::CreateQuaternion(m_Pitch, right);

	// combined orientation: world yaw followed by local pitch
	glm::quat Q = pitchQuat * yawQuat;
	m_Orientation = Q;
	 
	Q = glm::normalize( Q );

	glm::vec4 rotForward	= Q * glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f );
	glm::vec4 rotRight		= Q * glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f );
	glm::vec4 rotUp			= Q * glm::vec4( 0.0f, 1.0f, 0.0f, 0.0f );

	m_transform.SetLocalAxes( glm::vec3( rotForward ), glm::vec3( rotRight ), glm::vec3( rotUp ) );

	m_OrientationChanged = false;
}

glm::mat4 FreeLookCamera::GetViewMatrix() const
{
	return glm::lookAt(m_transform.GetPosition(), m_transform.GetPosition() + m_transform.GetForward(), m_transform.GetUp());
}