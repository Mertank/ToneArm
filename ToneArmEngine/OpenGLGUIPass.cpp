/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	OpenGLGUIPass

	Created by: Karl Merten
	Created on: 30/06/2014

========================
*/
#include "OpenGLGUIPass.h"
#include "OpenGLProgramManager.h"
#include "GUIManager.h"
#include "OpenGLRenderModule.h"

namespace vgs {
IMPLEMENT_RTTI( OpenGLGUIPass, RenderPass )
/*
========
OpenGLGUIPass::~OpenGLGUIPass

	OpenGLGUIPass destructor
========
*/
OpenGLGUIPass::~OpenGLGUIPass( void ) {
	m_guiMgr->SetApplicationWin( NULL );
	delete m_guiMgr;
}
/*
========
OpenGLGUIPass::CreateGUIPassWithPriority

	Creates the gui pass, and gui manager
========
*/
OpenGLGUIPass* OpenGLGUIPass::CreateGUIPassWithPriority( unsigned int priority ) {
	OpenGLGUIPass* pass = new OpenGLGUIPass();
	if ( pass ) {
		pass->SetPriority( priority );
		pass->SetPassType( PassType::GUI );
		pass->m_guiMgr = new GUIManager();
		pass->m_guiMgr->Init();
	} else {
		delete pass;
		pass = NULL;
	}

	return pass;
}
/*
========
OpenGLGUIPass::DoPass

	Does the GUI pass
========
*/
void OpenGLGUIPass::DoPass( std::unordered_map<unsigned int, Renderable*>* renderObjects, RenderModule* renderer ) {
	BeginPass();
	m_guiMgr->Draw();
	FinishPass();

	( ( OpenGLRenderModule* ) renderer )->GetWindow()->setActive();
}
/*
========
OpenGLGUIPass::BeginPass

	Setups the pass
========
*/
void OpenGLGUIPass::BeginPass( void ) {
	OpenGLProgramManager::GetInstance()->ClearProgram();
	glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}
/*
========
OpenGLGUIPass::FinishPass

	Finishes the pass
========
*/
void OpenGLGUIPass::FinishPass( void ) {
	
}
}