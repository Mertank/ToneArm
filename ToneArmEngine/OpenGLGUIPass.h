/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLGUIPass

		OpenGL GUI pass.

		Created by: Karl Merten
		Created on: 30/06/2014

========================
*/

#ifndef __OPENGLGUIPASS_H__
#define __OPENGLGUIPASS_H__

#include "RenderPass.h"

namespace vgs {

class GUIManager;

class OpenGLGUIPass : public RenderPass {
							DECLARE_RTTI
public:
	virtual					~OpenGLGUIPass( void );

	static OpenGLGUIPass*	CreateGUIPassWithPriority( unsigned int priority );
	GUIManager*				GetGUIManager( void ) const { return m_guiMgr; }

	virtual void			ProcessRenderable( Renderable* obj ) {} //This should never get called anyway.
	virtual void			BeginPass( void );
	virtual void			FinishPass( void );

	virtual void			DoPass( std::unordered_map<unsigned int, Renderable*>* renderObjects, RenderModule* renderer );
private:
	GUIManager*				m_guiMgr;

};

}

#endif // __OPENGLGUIPASS_H__