/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ParticleEmitter

		A basic particle emitter.

		Created by: Karl Merten
		Created on: 23/07/2014

========================
*/

#ifndef __PARTICLEEMITTER_H__
#define __PARTICLEEMITTER_H__

#include "Renderable.h"

namespace vgs {

class Particle;

struct ParticleEmitterSettings {
	ParticleEmitterSettings( void ) :
		particleCount( 0 ),
		particleTimeToLive( 0.0f ),
		emissionRate( 0.0f ),
		particleColor( 0.0f ),
		emissionAngle( 0.0f ),
		particleTimeToLiveVariance( 0.0f ),
		loopCount( 0 ),
		particleSize( 5 ),
		emissionSpeed( 0.0f )
	{}

	ParticleEmitterSettings( unsigned int count, float ttl, float rate, const glm::vec3& col, int loop, float size, float speed ) :
		particleCount( count ),
		particleTimeToLive( ttl ),
		emissionRate( rate ),
		particleColor( col ),
		emissionAngle( 0.0f ),
		particleTimeToLiveVariance( 0.0f ),
		loopCount( loop ),
		particleSize( size ),
		emissionSpeed( speed ),
		emissionSpeedVariance( 10.0f )
	{}

	ParticleEmitterSettings( unsigned int count, float ttl, float ttlVariance, float rate, float angle, const glm::vec3& col, int loop, float size, float speed, float speedVariance ) :
		particleCount( count ),
		particleTimeToLive( ttl ),
		emissionRate( rate ),
		particleColor( col ),
		emissionAngle( angle ),
		particleTimeToLiveVariance( ttlVariance ),
		loopCount( loop ),
		particleSize( size ),
		emissionSpeed( speed ),
		emissionSpeedVariance( speedVariance )
	{}

	glm::vec3		particleColor;

	float			emissionAngle;	

	float			emissionRate;

	float			particleTimeToLive;
	float			particleTimeToLiveVariance;

	unsigned int	particleCount;
	int				loopCount;

	float			particleSize;
	float			emissionSpeedVariance;
	float			emissionSpeed;
};

class ParticleEmitter : public Renderable {
public:
								~ParticleEmitter( void );

	virtual void				Update( float dt );

	virtual float				GetOpacity( void ) { return 1.0f; };
	virtual void				UpdateTransform( TransformMessage* msg );
	virtual void				UpdateSettings( const ParticleEmitterSettings& settings );
	
	void						Emit( bool emit = true, int amount = -1 );
	bool						IsEmitting( void ) { return m_emitting && m_livingParticles == 0; }
protected:
								ParticleEmitter( void );
	bool						InitializeEmitterWithSettings( const ParticleEmitterSettings& settings );

	float						GetNextTimeToLive( void );
	glm::vec3					GetNextDirection( void );

	ParticleEmitterSettings		m_emitterSettings;
	Particle*					m_particles;

	unsigned int				m_livingParticles;
	unsigned int				m_spawnedParticles;
	
	bool						m_emitting;
};

}

#endif //__PARTICLEEMITTER_H__