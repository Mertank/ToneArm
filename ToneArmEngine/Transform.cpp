/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Transform.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 4/28/2014 11:04:15 AM

========================
*/
#include "Transform.h"
#include "MathHelper.h"

using namespace vgs;

Transform::Transform()
	: m_position(glm::vec3(0.0f))
	, m_rotation(glm::vec3(0.0f))
	, m_scale(glm::vec3(1.0f))
	, m_model(glm::mat4(1.0f))
{
}

void Transform::SetPosition(const v3& position)
{
	m_position = position;
	m_model[3] = glm::vec4(position, 1.0f);
}

void Transform::SetRotation(const v3& rotation)
{
	if ( rotation != m_rotation ) {		
		m_rotation = rotation;	

		m_model = glm::scale( glm::mat4( 1.0 ), m_scale );
		m_model = glm::rotate(m_model, m_rotation.z, v3(0, 0, 1));
		m_model = glm::rotate(m_model, m_rotation.y, v3(0, 1, 0));
		m_model = glm::rotate(m_model, m_rotation.x, v3(1, 0, 0));
		m_model[3] = glm::vec4(m_position, 1.0f);
	}
}

void Transform::SetRotation(const glm::quat& rotation)
{
	m_model = glm::scale( glm::mat4( 1.0 ), m_scale );
	m_model = glm::toMat4( rotation ) * m_model;
	m_model[3] = glm::vec4(m_position, 1.0f);
}

void Transform::SetScale(const v3& scale)
{
	m_scale = scale;
	m_model = glm::scale(m_model, scale);
}

void Transform::Translate(const v3& value) {

	m_position += value;
	m_model[3] += glm::vec4(value, 0.0f);
}

void Transform::Rotate(const v3& axis, float angle) {

	m_rotation += axis * angle;
	m_model = glm::rotate(m_model, angle, axis);
}

void Transform::Scale(const v3& factor) {

	m_scale *= factor;
	m_model = glm::scale(m_model, factor);
}

void Transform::SetLocalAxes( const v3& forward, const v3& right, const v3& up ) {
	m_model[0].x = right.x;
	m_model[0].y = right.y;
	m_model[0].z = right.z;

	m_model[1].x = up.x;
	m_model[1].y = up.y;
	m_model[1].z = up.z;

	m_model[2].x = forward.x;
	m_model[2].y = forward.y;
	m_model[2].z = forward.z;
}