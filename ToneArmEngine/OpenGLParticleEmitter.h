/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLParticleEmitter

		OpenGL implementation of a particle emitter.

		Created by: Karl Merten
		Created on: 23/07/2014

========================
*/

#ifndef __OPENGLPARTICLEEMITTER_H__
#define __OPENGLPARTICLEEMITTER_H__

#include "ParticleEmitter.h"
#include <GL\glew.h>

namespace vgs {

class OpenGLParticleEmitter : public ParticleEmitter {
public:
									~OpenGLParticleEmitter( void );

	static OpenGLParticleEmitter*	CreateOpenGLParticleEmitter( const ParticleEmitterSettings& settings );

	virtual void					UpdateSettings( const ParticleEmitterSettings& settings );

	void							PreRender( void );
	void							Render( void );
	void							PostRender( void );
private:
									OpenGLParticleEmitter( void );
	bool							InitializeEmitterWithSettings( const ParticleEmitterSettings& settings );

	void							UpdateParticleBuffer( void );

	GLuint							m_particleVBO;
};

}

#endif //__OPENGLPARTICLEEMITTER_H__