/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLAnimator

		An OpenGL animator that's specific to OpenGL.

		Created by: Karl Merten
		Created on: 18/07/2014

========================
*/
#ifndef __OPENGLANIMATOR_H__
#define __OPENGLANIMATOR_H__

#include "Animator.h"
#include <GL\glew.h>

namespace vgs {

struct Skeleton;

class OpenGLAnimator : public Animator {
public:
	virtual					~OpenGLAnimator( void );							

	static OpenGLAnimator*	CreateAnimatorWithJointCount( unsigned int joints );
	static OpenGLAnimator*	CreateFromAnimator( OpenGLAnimator* other );

	virtual void			PreRender( void );
	virtual void			PostRender( void );
private:
							OpenGLAnimator( void );
							OpenGLAnimator( const OpenGLAnimator& other );

	bool					InitializeAnimator( unsigned int joints );
	void					UpdateDataBuffer( void );

	GLuint					m_matrixDataBuffer;
	GLuint					m_matrixTextureBuffer;
};

}

#endif //__OPENGLANIMATOR_H__