/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Particle

		A particle.

		Created by: Karl Merten
		Created on: 23/07/2014

========================
*/

#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include <glm\glm.hpp>

namespace vgs {

class ParticleEmitter;

class Particle {
public:
						Particle( void );
						
	bool				Update( float dt );
	void				ResetParticle( float timeToLive, const glm::vec3& pos, const glm::vec3& vel );

	void				SetParticlePosition( const glm::vec3& pos ) { m_particlePosition = pos; }
	glm::vec3*			GetParticlePosition( void ) { return &m_particlePosition; }
private:
	float				m_particleTimeToLive;

	glm::vec3			m_particlePosition;
	glm::vec3			m_particleVelocity;
};

}

#endif //__PARTICLE_H__