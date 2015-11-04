/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLTransparencyPass

		Pass for transparent objects.

		Created by: Karl Merten
		Created on: 04/08/2014

========================
*/
#ifndef __OPENGLTRANSPARENCYPASS_H__
#define __OPENGLTRANSPARENCYPASS_H__

#include "RenderPass.h"

namespace vgs {

class OpenGLTransparencyPass : public RenderPass {
public:
	static OpenGLTransparencyPass*		CreateWithPriority( unsigned int priority );

	virtual void						BeginPass( void );
	virtual void						FinishPass( void );
	virtual void						ProcessRenderable( Renderable* obj );
private:
	bool								InitializePass( unsigned int priority );
};

}

#endif //__OPENGLTRANSPARENCYPASS_H__