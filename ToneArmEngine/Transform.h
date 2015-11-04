/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Transform.h

		Created by: Vladyslav Dolhopolov
		Created on: 4/28/2014 11:04:15 AM

========================
*/
#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>

namespace vgs
{
	/*
	========================

		Transform

			Model matrix and a bunch of useful methods.

			Created by: Vladyslav Dolhopolov
			Created on: 4/28/2014 11:04:15 AM

	========================
	*/
	class Transform
	{
		typedef glm::vec3 v3;

	public:
							Transform();

		void				SetPosition(const v3& position);
		void				SetRotation(const v3& rotation);
		void				SetRotation(const glm::quat& rotation);
		void				SetScale(const v3& scale);

		void				SetLocalAxes( const v3& forward, const v3& right, const v3& up );

		void				Translate(const v3& value);
		void				Scale(const v3& factor);
		void				Rotate(const v3& axis, const float angle);

		const v3&			GetPosition() const									{ return m_position; }
		const v3&			GetRotation() const									{ return m_rotation; }
		const v3&			GetScale() const 									{ return m_scale; }

		const glm::mat4&	GetModelMatrix() const								{ return m_model; }

		v3					GetForward() const									{ return v3(glm::normalize(m_model[2])); }
		v3					GetRight() const									{ return -v3(glm::normalize(m_model[0])); }
		v3					GetUp() const										{ return v3(glm::normalize(m_model[1])); }

	private:
		v3					m_position;
		v3					m_rotation;
		v3					m_scale;
		
		glm::mat4			m_model;

	};

}

#endif __TRANSFORM_H__