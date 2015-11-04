/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/23/2014 12:53:11 PM
------------------------------------------------------------------------------------------
*/

#include "FixedCamera.h"
#include "Scene.h"

using namespace vgs;
using namespace merrymen;

INITIALIZE_DEBUG_COMMAND_CALLBACK( vgs::FixedCamera, freezeCamera )
IMPLEMENT_RTTI( FixedCamera, Camera )

//
// default constructor
//
FixedCamera::FixedCamera() :
	m_renderableModule(nullptr),
	m_target(nullptr),
	m_lookAtPoint(glm::vec3()),
	m_offset(glm::vec3()),
	m_targetPositions(std::deque<glm::vec3>(POSITIONS_FOR_INTERPOLATION)),
	m_closeEnough(false),
	m_frozen(false)
{
	UpdateOrientation();
}
//
// initializes the camera and sets its target to the specified Character instance, placing the camera at the specified offsets from this object
//
bool FixedCamera::InitWithTarget(Node* const target, const float zOffset, const float yOffset) {

	SetProjection(Projection(1280.0f, 720.0f, 1.0f, 15000.0f, 30, CameraProjection::PERSPECTIVE));
	m_frustum.UpdateFrustumInternals( m_currentProjection.FOV, m_currentProjection.width / m_currentProjection.height, m_currentProjection.nearClipping, m_currentProjection.farClipping );

	m_target = target;
	m_lookAtPoint = m_target->GetAbsoluteWorldPosition();

	SetPosition(target->GetPosition().x, target->GetPosition().y + yOffset, target->GetPosition().z + zOffset);

	m_offset.y = yOffset;
	m_offset.z = zOffset;

	UpdateOrientation();

	CameraTransformMessage* msg = m_renderableModule->GetRenderMessage<CameraTransformMessage>();
	msg->Init( 0, GetViewMatrix(), GetProjectionMatrix(), GetPosition() );
	m_renderableModule->SendRenderMessage( msg );

	return true;
}

//
// camera updates it's state
//
void FixedCamera::Update(float dt)
{
	bool targetMoved = false;

	if (m_target && !m_frozen) {

		// if the target object has moved the camera needs to be moved accordingly
		if (m_target->GetPosition().x != m_transform.GetPosition().x ||
			m_target->GetPosition().z + m_offset.z != m_transform.GetPosition().z ||
			m_target->GetPosition().y + m_offset.y != m_transform.GetPosition().y) {

			targetMoved = true;
		}

		// follow the target object
		if (m_target->GetRTTI().DerivesFrom(Character::rtti)) {

			Character* player = static_cast<Character*>(m_target);
			
			if (player->IsAiming()) {
				FollowAimingPlayer(player);
			}
			else {
				FollowObject();
			}
		}
		else if (targetMoved) {
			FollowObject();
		}

		CameraTransformMessage* msg = m_renderableModule->GetRenderMessage<CameraTransformMessage>();
		msg->Init( 0, GetViewMatrix(), GetProjectionMatrix(), GetPosition() );
		m_renderableModule->SendRenderMessage( msg );
	}

	Camera::Update(dt);
}

//
// simply follows m_target object
//
void FixedCamera::FollowObject() {

	// update camera's position by lerping between most recent target positions
	SetPosition(MathHelper::Lerp(m_targetPositions) + m_offset);

	// update the point that the camera tracks
	m_lookAtPoint = GetPosition() - m_offset;

	// add another position to the list of points to interpolate between
	m_targetPositions.push_front(m_target->GetAbsoluteWorldPosition());

	if (m_closeEnough) {
		m_closeEnough = false;
	}
}

//
// follows the character, who is in aiming at the moment
//
void FixedCamera::FollowAimingPlayer(const merrymen::Character* const player) {

	WeaponCS* activeWeapon = player->GetActiveWeapon();

	glm::vec3 newOffset = glm::vec3(0.0f, activeWeapon->GetWeaponStats().ZoomOutFactor * m_offset.y, activeWeapon->GetWeaponStats().ZoomOutFactor * m_offset.z);

	glm::vec3 finalPoint = player->GetPosition() + player->GetTransform().GetForward() * activeWeapon->GetWeaponStats().AimingDistance + newOffset;

	if (!m_closeEnough) {

		// use hysteresis to calculate next camera's position
		glm::vec3 nextPoint = GetPosition() + (finalPoint - GetPosition()) * CAMERA_ZOOM_SPEED;
		SetPosition(nextPoint);

		// stop the "aiming"
		if (glm::distance(finalPoint, GetPosition()) < CLOSE_ENOUGH_DISTANCE) {
			m_closeEnough = true;
		}
	}
	else {
		SetPosition(finalPoint);
	}

	// update the point that the camera tracks
	m_lookAtPoint = GetPosition() - newOffset;

	// add another position to the list of points to interpolate between
	m_targetPositions.push_front(m_lookAtPoint);

	// and remove one
	m_targetPositions.pop_back();
}

//
// updates camera's orientation according to it's pitch and yaw values
//
void FixedCamera::UpdateOrientation()
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

	m_OrientationChanged = false;
}

//
// returns camera's view matrix
//
glm::mat4 FixedCamera::GetViewMatrix() const
{
	if ( m_target ) {
		return glm::lookAt( m_transform.GetPosition(), m_lookAtPoint, m_transform.GetUp() );
	} else {
		return glm::lookAt( m_transform.GetPosition(), m_transform.GetPosition() + m_transform.GetForward(), m_transform.GetUp() );
	}
}

BEGIN_CALLBACK_FUNCTION( FixedCamera, freezeCamera )
	if ( args.size() > 0 ) 
	{
		char* enable	= args[0];
		int value		= StringUtils::IsEqual(enable, "on") ? 1 : StringUtils::IsEqual(enable, "off") ? 0 : -1;

		if (value > -1)
		{
			Scene* currentScene = Engine::GetRunningScene();
			Camera* currentCamera = currentScene->GetCurrentCamera();

			if (currentCamera->GetRTTI() == FixedCamera::rtti)
			{
				static_cast<FixedCamera*>(currentCamera)->SetFrozen( value == 1 );
			}
		}
	}
END_CALLBACK_FUNCTION