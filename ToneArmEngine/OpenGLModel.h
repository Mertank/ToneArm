/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLModel

		OpenGL specific implementation of a model.

		Created by: Karl Merten
		Created on: 13/05/2014

========================
*/

#ifndef __OPENGLMODEL_H__
#define __OPENGLMODEL_H__

#include "RenderableModel.h"
#include "OpenGLProgramManager.h"

namespace vgs {

class Animator;
struct DuplicateModelMessage;

class OpenGLModel : public RenderableModel {
public:
							OpenGLModel( void );

	static OpenGLModel*		CreateModelFromMessage( const CreateModelMessage* msg );
	static OpenGLModel*		DuplicateModel( const OpenGLModel* other, DuplicateModelMessage* msg );
	
	virtual void			PreRender( void );
	virtual void			Render( void );
	virtual void			PostRender( void );

	virtual void			AttachTexture( Texture* texture, unsigned int meshIndex );
	virtual void			AttachMesh( RenderableMesh* mesh );

	virtual void			IsLit( bool lit );
	virtual void			SetLightingType( LightingType::Value lightingType );

	virtual void			AttachAnimator( Animator* animator );
	virtual void			AddAnimation( Animation* anim );
private:
	bool					InitializeFromMessage( const CreateModelMessage* msg );	
	bool					InitializeFromModel( const OpenGLModel* other, DuplicateModelMessage* msg );	

	ProgramFeatures::Value	m_requiredFeatures;
};

}

#endif //__OPENGLMODEL_H__