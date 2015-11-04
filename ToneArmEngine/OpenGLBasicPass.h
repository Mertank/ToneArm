/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLBasicPass

		Basic OpenGL Diffuse Pass.

		Created by: Karl Merten
		Created on: 30/06/2014

========================
*/

#ifndef __OPENGLDIFFUSEPASS_H__
#define __OPENGLDIFFUSEPASS_H__

#include "RenderPass.h"

namespace vgs {

class OpenGLBasicPass : public RenderPass {
								DECLARE_RTTI
public:
	static OpenGLBasicPass*		CreateWithPriority( unsigned int priority );
private:
	virtual void				ProcessRenderable( Renderable* obj );
	virtual void				BeginPass( void );
	virtual void				FinishPass( void );
};

}

#endif //__OPENGLDIFFUSEPASS_H__