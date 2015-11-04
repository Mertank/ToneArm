/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Particle

	Created by: Karl Merten
	Created on: 23/07/2014

========================
*/
#include "Particle.h"

namespace vgs {
/*
========
Particle::Particle

	Particle default constructor.
========
*/
Particle::Particle( void ) :
	m_particleTimeToLive( 0.0f )
{}
/*
========
Particle::Update

	Particle Update
========
*/
bool Particle::Update( float dt ) {
	m_particleTimeToLive -= dt;
	
	if ( m_particleTimeToLive < 0.0f ) {
		return false;
	}

	m_particlePosition += ( m_particleVelocity * dt );
	return true;
}
/*
========
Particle::ResetParticle

	Particle Reset
========
*/
void Particle::ResetParticle( float timeToLive, const glm::vec3& pos, const glm::vec3& vel ) {
	m_particleTimeToLive = timeToLive;
	m_particlePosition = pos;
	m_particleVelocity = vel;
}
}