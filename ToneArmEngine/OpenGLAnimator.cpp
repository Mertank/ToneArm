/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	OpenGLAnimator

	Created by: Karl Merten
	Created on: 18/07/2014

========================
*/
#include "OpenGLAnimator.h"
#include "Animation.h"
#include "Log.h"
#include "OpenGLProgramManager.h"
#include "OpenGLProgram.h"

namespace vgs {
/*
========
OpenGLAnimator::OpenGLAnimator

	OpenGLAnimator default constructor.
========
*/
OpenGLAnimator::OpenGLAnimator( void ) :
	m_matrixDataBuffer( 0 ),
	m_matrixTextureBuffer( 0 )
{}
/*
========
OpenGLAnimator::OpenGLAnimator

	OpenGLAnimator copy constructor.
========
*/
OpenGLAnimator::OpenGLAnimator( const OpenGLAnimator& orig ) :
	Animator( orig ),
	m_matrixDataBuffer( 0 ),
	m_matrixTextureBuffer( 0 )
{}
/*
========
OpenGLAnimator::~OpenGLAnimator

	OpenGLAnimator destructor.
========
*/
OpenGLAnimator::~OpenGLAnimator( void ) {
	glDeleteBuffers( 1, &m_matrixDataBuffer );
	glDeleteTextures( 1, &m_matrixTextureBuffer );
}
/*
========
OpenGLAnimator::CreateAnimatorWithJointCount

	OpenGLAnimator create.
========
*/
OpenGLAnimator* OpenGLAnimator::CreateAnimatorWithJointCount( unsigned int joints ) {
	OpenGLAnimator* animator = new OpenGLAnimator();
	if ( animator && animator->InitializeAnimator( joints ) ) {
		return animator;
	}

	delete animator;
	return NULL;
}
/*
========
OpenGLAnimator::CreateAnimator

	OpenGLAnimator create.
========
*/
OpenGLAnimator* OpenGLAnimator::CreateFromAnimator( OpenGLAnimator* other ) {
	OpenGLAnimator* animator = new OpenGLAnimator( *other );
	if ( animator && animator->InitializeAnimator( other->GetJointCount() ) ) {
		return animator;
	}

	delete animator;
	return NULL;
}
/*
========
OpenGLAnimator::InitializeAnimator

	Initializes the animator.
========
*/
bool OpenGLAnimator::InitializeAnimator( unsigned int joints ) {
	glGenBuffers( 1, &m_matrixDataBuffer );
	if ( !m_matrixDataBuffer ) {
		Log::GetInstance()->WriteToLog( "OpenGLAnimator", "Could not create a data buffer for the matrix" );
		return false;
	}

	glGenTextures( 1, &m_matrixTextureBuffer );
	if ( !m_matrixTextureBuffer ) {
		glDeleteBuffers( 1, &m_matrixDataBuffer );
		Log::GetInstance()->WriteToLog( "OpenGLAnimator", "Could not create a texture buffer for the matrix" );
		return false;
	}

	glm::mat4 matrix( 1.0f ); //Init to bind pose
	float* matrixBuffer = new float[joints * 4 * 4];
	for ( unsigned i = 0; i < joints; ++i ) {
		memcpy( &matrixBuffer[i * 16], &matrix[0], sizeof( float ) * 16 );
	}

	glBindBuffer( GL_TEXTURE_BUFFER, m_matrixDataBuffer );
	glBufferData( GL_TEXTURE_BUFFER, sizeof( float ) * joints * 16, matrixBuffer, GL_DYNAMIC_DRAW );
	glBindBuffer( GL_TEXTURE_BUFFER, 0 );

	SetJointCount( joints );

	delete[] matrixBuffer;

	return true;
}
/*
========
OpenGLAnimator::UpdateDataBuffer

	Update the data buffers on the GPU.
========
*/
void OpenGLAnimator::UpdateDataBuffer( void ) {
	if ( m_currentSkeleton ) {
		unsigned index = 0;
		for ( SkeletonMatricies::const_iterator currentMatrix = m_currentSkeleton->jointMatricies.begin();
			  currentMatrix != m_currentSkeleton->jointMatricies.end(); ++currentMatrix, ++index ) {
			glBufferSubData( GL_TEXTURE_BUFFER, index * 16 * sizeof( float ), sizeof( float ) * 16, &( *currentMatrix )[0] );        
		}
	}
}
/*
========
OpenGLAnimator::PreRender

	Called before it's rendered.
========
*/
void OpenGLAnimator::PreRender( void ) {
	glBindBuffer( GL_TEXTURE_BUFFER, m_matrixDataBuffer );
	
	UpdateDataBuffer();

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, m_matrixTextureBuffer );
	glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, m_matrixDataBuffer );

	OpenGLProgramManager::GetInstance()->GetActiveProgram()->SetUniform( "uMatriciesBuffer", 0 );
}
/*
========
OpenGLAnimator::PostRender

	Called after it's rendered.
========
*/
void OpenGLAnimator::PostRender( void ) {
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindBuffer( GL_TEXTURE_BUFFER, 0 );
	glActiveTexture( GL_TEXTURE0 );
}

}