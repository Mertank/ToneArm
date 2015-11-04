/*
========================

Copyright (c) 2014 Vinyl Games Studio

	FreeLookCamera.h

		Created by: Vladyslav Dolhopolov
		Created on: 4/25/2014 3:37:55 PM

========================
*/
#ifndef __FREELOOKCAMERA_H__
#define __FREELOOKCAMERA_H__

#include "Camera.h"
#include "MathHelper.h"
#include "VGSMacro.h"
#include "Engine.h"
#include "RenderModule.h"

namespace vgs
{
	/*
	========================

		FreeLookCamera

			Can move and look at any direction.

			Created by: Vladyslav Dolhopolov
			Created on: 4/25/2014 3:37:55 PM

	========================
	*/
	class FreeLookCamera : public Camera
	{
		CREATE_RENDERABLE_NODE_METH(FreeLookCamera, Engine::GetInstance()->GetModuleByType<RenderModule>(EngineModuleType::RENDERER));
		DECLARE_RTTI
	public:
								FreeLookCamera();
		virtual					~FreeLookCamera() {}

		void					UpdateOrientation();

		void					SetYaw(float radians);
		void					SetPitch(float radians);
		void					Yaw(float radians);
		void					Pitch(float radians);

		float					GetSpeed() const						{ return m_Speed; }
		void					SetSpeed(float speed)					{ m_Speed = speed; }

		virtual glm::mat4		GetViewMatrix() const override;

		virtual bool			Init()				override;
		virtual void			Update(float dt)	override;

	private:
		float					m_Speed;
		float					m_MouseSpeed;

	};

}

#endif __FREELOOKCAMERA_H__