/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	OpenGLParticleEmitter

	Created by: Karl Merten
	Created on: 23/07/2014

========================
*/
#include "OpenGLParticleEmitter.h"
#include "Particle.h"
#include "OpenGLProgramManager.h"
#include "OpenGLProgram.h"
#include "OpenGLRenderModule.h"
#include "CameraProjectionData.h"
#include "MathHelper.h"
#include "CameraProjectionData.h"

namespace vgs {
/*
========
OpenGLParticleEmitter::OpenGLParticleEmitter

	OpenGLParticleEmitter default constructor
========
*/
OpenGLParticleEmitter::OpenGLParticleEmitter( void ) :
	m_particleVBO( 0 )
{}
/*
========
OpenGLParticleEmitter::~OpenGLParticleEmitter

	OpenGLParticleEmitter destructor
========
*/
OpenGLParticleEmitter::~OpenGLParticleEmitter( void ) {
	glDeleteBuffers( 1, &m_particleVBO );
}
/*
========
OpenGLParticleEmitter::CreateOpenGLParticleEmitter

	Creates an emitter for OpenGL.
========
*/
OpenGLParticleEmitter* OpenGLParticleEmitter::CreateOpenGLParticleEmitter( const ParticleEmitterSettings& settings ) {
	OpenGLParticleEmitter* emitter = new OpenGLParticleEmitter();
	if ( emitter && emitter->InitializeEmitterWithSettings( settings ) ) {
		return emitter;
	}

	delete emitter;
	return NULL;
}
/*
========
OpenGLParticleEmitter::InitializeEmitterWithSettings

	Initializes an emitter for OpenGL.
========
*/
bool OpenGLParticleEmitter::InitializeEmitterWithSettings( const ParticleEmitterSettings& settings ) {
	if ( !ParticleEmitter::InitializeEmitterWithSettings( settings ) ) {
		return false;
	}

	glGenBuffers( 1, &m_particleVBO );
	if ( !m_particleVBO ) {
		return false;
	}

	glBindBuffer( GL_ARRAY_BUFFER, m_particleVBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * 3 * settings.particleCount, NULL, GL_DYNAMIC_DRAW );

	glBindVertexArray( 0 );

	return true;
}
/*
========
OpenGLParticleEmitter::UpdateParticleBuffer

	Updates the particle positions on the GPU.
========
*/
void OpenGLParticleEmitter::UpdateParticleBuffer( void ) {
	glm::vec3* particlePosition = m_particles[0].GetParticlePosition();

	for ( unsigned int i = 0; i < m_livingParticles; ++i ) {
		glBufferSubData( GL_ARRAY_BUFFER, ( sizeof( float ) * 3 * i ), sizeof( float ) * 3, particlePosition );
		particlePosition = ( glm::vec3* )( ( char* )particlePosition + sizeof( Particle ) );
	}
}
/*
========
OpenGLParticleEmitter::PreRender

	Setup before render
========
*/
void OpenGLParticleEmitter::PreRender( void ) {
	if ( m_livingParticles == 0 ) { return; }
	
	glBindBuffer( GL_ARRAY_BUFFER, m_particleVBO );
	UpdateParticleBuffer();
	
	OpenGLProgramManager::GetInstance()->UseProgramWithFeatures( ProgramFeatures::PARTICLE_BILLBOARDING );

	( ( OpenGLRenderModule* ) m_renderer )->SendGlobalUniforms();	
	
	OpenGLProgram* prog = OpenGLProgramManager::GetInstance()->GetActiveProgram();

	glm::mat3 test = glm::mat3( m_renderer->GetCameraData().GetViewMatrix() ) * glm::transpose( glm::mat3( m_renderer->GetCameraData().GetViewMatrix() ) );

	prog->SetUniform( "u_BillboardMatrix", glm::transpose( glm::mat3( m_renderer->GetCameraData().GetViewMatrix() ) ) );
	prog->SetUniform( "u_DiffuseColor", m_emitterSettings.particleColor );
	prog->SetUniform( "u_Opacity", GetOpacity() );
	prog->SetUniform( "u_ParticleSize", m_emitterSettings.particleSize );
}
/*
========
OpenGLParticleEmitter::Render

	Emitter render
========
*/
void OpenGLParticleEmitter::Render( void ) {
	if ( m_livingParticles == 0 ) { return; }
	
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 3, ( void* )( NULL ) );

	
	glDrawArrays( GL_POINTS, 0, m_livingParticles );
	
	glDisableVertexAttribArray( 0 );	
}
/*
========
OpenGLParticleEmitter::PostRender

	Emitter post render reset state
========
*/
void OpenGLParticleEmitter::PostRender( void ) {
	if ( m_livingParticles == 0 ) { return; }
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
/*
========
OpenGLParticleEmitter::UpdateSettings

	Updates the settings.
========
*/
void OpenGLParticleEmitter::UpdateSettings( const ParticleEmitterSettings& settings ) {
	if ( settings.particleCount != m_emitterSettings.particleCount ) {
		glBindBuffer( GL_ARRAY_BUFFER, m_particleVBO );
		glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * 3 * settings.particleCount, NULL, GL_DYNAMIC_DRAW );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

	ParticleEmitter::UpdateSettings( settings );
}
}