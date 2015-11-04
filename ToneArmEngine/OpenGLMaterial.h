/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLMaterial

		Material with OpenGL specific implementation.

		Created by: Karl Merten
		Created on: 20/05/2014

========================
*/

#ifndef __OPENGLMATERIAL_H__
#define __OPENGLMATERIAL_H__

#include "Material.h"
#include "OpenGLProgramManager.h"

namespace vgs {

class OpenGLMaterial : public Material {
public:
									OpenGLMaterial( void );
									OpenGLMaterial( const OpenGLMaterial& orig );
	virtual							~OpenGLMaterial( void );
	
	static OpenGLMaterial*			CreateMaterialWithColor( const glm::vec3& diffColor );

	virtual void					AddTexture( Texture* texture );
	virtual void					BindMaterial( void );
	virtual void					UnbindMaterial( void );

	inline ProgramFeatures::Value	GetRequiredFeatures( void ) const { return m_requiredFeatures; }
private:
	ProgramFeatures::Value			m_requiredFeatures;
};

}

#endif //__OPENGLMATERIAL_H__