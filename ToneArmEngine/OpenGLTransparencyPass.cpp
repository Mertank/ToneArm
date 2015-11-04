/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	OpenGLTransparencyPass

	Created by: Karl Merten
	Created on: 04/08/2014

========================
*/
#include "OpenGLTransparencyPass.h"
#include "Renderable.h"
#include <GL\glew.h>

namespace vgs {

OpenGLTransparencyPass* OpenGLTransparencyPass::CreateWithPriority( unsigned int priority ) {
	OpenGLTransparencyPass* pass = new OpenGLTransparencyPass();
	if ( pass && pass->InitializePass( priority ) ) {
		return pass;
	}

	delete pass;
	return NULL;
}

bool OpenGLTransparencyPass::InitializePass( unsigned int priority ) {
	RenderPass::InitializePass( NULL );

	m_passType = PassType::TRANSPARENCY;
	m_priority = priority;

	return true;
}

void OpenGLTransparencyPass::BeginPass( void ) {
	glEnable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );
	glAlphaFunc( GL_GREATER, 0.05f );
	glEnable( GL_ALPHA_TEST );
}

void OpenGLTransparencyPass::FinishPass( void ) {
	glDisable( GL_BLEND );
	glDisable( GL_ALPHA_TEST );
	glDisable( GL_DEPTH_TEST );
}

void OpenGLTransparencyPass::ProcessRenderable( Renderable* obj ) {
	obj->PreRender();
	obj->Render();
	obj->PostRender();
}

}