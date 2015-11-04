/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/23/2014 12:53:11 PM
------------------------------------------------------------------------------------------
*/

#ifndef __FIXED_CAMERA_H__
#define __FIXED_CAMERA_H__

#include "Camera.h"
#include "VGSMacro.h"
#include "Engine.h"
#include "RenderModule.h"
#include "MathHelper.h"

#include "../Merrymen/Character.h"

#define	CAMERA_ZOOM_SPEED				0.05f
#define	CLOSE_ENOUGH_DISTANCE			1.0f
#define POSITIONS_FOR_INTERPOLATION		25

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	FixedCamera

	Main camera for the game.
------------------------------------------------------------------------------------------
*/

class FixedCamera : public Camera {

		CREATE_RENDERABLE_NODE_METH(FixedCamera, Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER));
		DECLARE_RTTI
		public:
									FixedCamera();
									~FixedCamera() {}

		void						UpdateOrientation();

		glm::mat4					GetViewMatrix()		const override;

		bool						InitWithTarget(Node* const target, const float zOffset, const float yOffset);

		void						Update(float dt)	override;
		void						FollowObject();
		void						FollowAimingPlayer(const merrymen::Character* const player);

		const glm::vec3&			GetLookAtPoint( void ) const { return m_lookAtPoint; }

		void						SetFrozen(bool value = true) { m_frozen = value; }

		DECLARE_DEBUG_COMMAND_CALLBACK( vgs::FixedCamera, freezeCamera )

	private:
		Node*						m_target;
		glm::vec3					m_lookAtPoint;
		glm::vec3					m_offset;
		std::deque<glm::vec3>		m_targetPositions;
		bool						m_closeEnough;
		bool						m_frozen;
};

} // namespace vgs

#endif __FIXED_CAMERA_H__