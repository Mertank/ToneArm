/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	ParticleEmitter

	Created by: Karl Merten
	Created on: 23/07/2014

========================
*/
#include "ParticleEmitter.h"
#include "Particle.h"

#include <algorithm>

#define RANDOM_MINUS_1_TO_1 ( ( ( ( double )rand() / RAND_MAX ) * 2.0 ) - 1.0 )
#define RANDOM_0_TO_1 ( ( ( double )rand() / RAND_MAX ) )

namespace vgs {
/*
========
ParticleEmitter::ParticleEmitter

	ParticleEmitter default constructor.
========
*/
ParticleEmitter::ParticleEmitter( void ) :
	m_particles( NULL ),
	m_livingParticles( 0 ),
	m_emitting( false ),
	m_spawnedParticles( 0 )
{}
/*
========
ParticleEmitter::~ParticleEmitter

	ParticleEmitter destructor.
========
*/
ParticleEmitter::~ParticleEmitter( void ) {
	delete[] m_particles;
}
/*
========
ParticleEmitter::InitializeEmitterWithSettings

	Initializes the emitter.
========
*/
bool ParticleEmitter::InitializeEmitterWithSettings( const ParticleEmitterSettings& settings ) {
	m_emitterSettings = settings;
	if ( settings.particleCount > 0 ) {
		m_particles = new Particle[settings.particleCount];
	}

	return true;
}
/*
========
ParticleEmitter::Update

	Updates all particles in the emitter.
========
*/
void ParticleEmitter::Update( float dt ) {
	Renderable::Update( dt );

	if ( m_emitting ) {
		unsigned int toEmit = std::max( ( unsigned int )( m_emitterSettings.emissionRate * dt ), 1U );
		for ( unsigned int i = 0; i < toEmit; ++i ) {
			if ( m_livingParticles < m_emitterSettings.particleCount ) { //Particles left.
				m_particles[m_livingParticles].ResetParticle( GetNextTimeToLive(), glm::vec3( GetCompositeMatrix()[3] ), 
															  GetNextDirection() * ( float )( m_emitterSettings.emissionSpeed + ( RANDOM_MINUS_1_TO_1 * m_emitterSettings.emissionSpeedVariance ) ) );
				++m_livingParticles;
				if ( m_emitterSettings.loopCount >= 0 ) {
					++m_spawnedParticles;
					if ( m_spawnedParticles > m_emitterSettings.particleCount - 1 ) {
						if ( m_emitterSettings.loopCount > 0 ) {
							--m_emitterSettings.loopCount;
						}
						m_spawnedParticles = 0;
						if ( m_emitterSettings.loopCount == 0 ) {
							m_emitting = false;
						}
					}
				}
			}
		}
	}

	if ( m_livingParticles > 0 ) {
		for ( unsigned int i = 0; i < m_livingParticles; ) {
			if ( !m_particles[i].Update( dt ) ) {
				memcpy( &m_particles[i], &m_particles[m_livingParticles - 1], sizeof( Particle ) );
				--m_livingParticles;
			} else {
				++i;
			}
		}
	}
}
/*
========
ParticleEmitter::Emit

	Resets all particles.
========
*/
void ParticleEmitter::Emit( bool emit, int amount ) {
	m_emitting = emit;
	m_emitterSettings.loopCount = amount;

	m_livingParticles = 0;
	m_spawnedParticles = 0;
}
/*
========
ParticleEmitter::GetNextTimeToLive

	Gets a random amount of time to live
========
*/
float ParticleEmitter::GetNextTimeToLive( void ) {
	return ( float )( RANDOM_MINUS_1_TO_1 * m_emitterSettings.particleTimeToLiveVariance ) + m_emitterSettings.particleTimeToLive;
}
/*
========
ParticleEmitter::GetNextDirection

	Gets a random direction
========
*/
glm::vec3 ParticleEmitter::GetNextDirection( void ) {	
	const glm::mat4& compMatrix = GetCompositeMatrix();
	if ( m_emitterSettings.emissionAngle == 0.0f ) {
		return glm::vec3( compMatrix[2] );
	} else {
		float rotationAmount = glm::radians( ( float )( RANDOM_MINUS_1_TO_1 * ( m_emitterSettings.emissionAngle * 0.5f ) ) );
		return glm::vec3( compMatrix[2][0] * cos( rotationAmount ) - compMatrix[2][2] * sin( rotationAmount ),
						  0.0f,
						  compMatrix[2][0] * sin( rotationAmount ) + compMatrix[2][2] * cos( rotationAmount ) );
	}	
}
/*
========
ParticleEmitter::UpdateTransform

	Extracts the position only for the emitter.
========
*/
void ParticleEmitter::UpdateTransform( TransformMessage* msg ) {
	Renderable::UpdateTransform( msg );
}
/*
========
ParticleEmitter::UpdateSettings

	Change the settings.
========
*/
void ParticleEmitter::UpdateSettings( const ParticleEmitterSettings& settings ) {
	if ( settings.particleCount != m_emitterSettings.particleCount ) {
		delete[] m_particles;
		m_particles = new Particle[settings.particleCount];
	}

	m_emitterSettings = settings;
}

}