/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	OpenGLMaterial

	Created by: Karl Merten
	Created on: 20/05/2014

========================
*/
#include "OpenGLMaterial.h"
#include "OpenGLProgram.h"

namespace vgs {
/*
========
OpenGLMaterial::OpenGLMaterial

	OpenGLMaterial default constructor
========
*/
OpenGLMaterial::OpenGLMaterial( void ) :
	m_requiredFeatures( ProgramFeatures::NONE )
{}
/*
========
OpenGLMaterial::~OpenGLMaterial

	OpenGLMaterial destructor
========
*/
OpenGLMaterial::~OpenGLMaterial( void )
{}
/*
========
OpenGLMaterial::OpenGLMaterial

	OpenGLMaterial copy constructor
========
*/
OpenGLMaterial::OpenGLMaterial( const OpenGLMaterial& orig ) :
	Material( orig )
{
	m_requiredFeatures = orig.m_requiredFeatures;
}
/*
========
Material::CreateMaterialWithColor

	Creates a material with the passed in diffuse color
========
*/
OpenGLMaterial* OpenGLMaterial::CreateMaterialWithColor( const glm::vec3& diffColor ) {
	OpenGLMaterial* matPtr = new OpenGLMaterial();
	if ( matPtr ) {
		matPtr->m_diffuseColor = diffColor;
		return matPtr;
	} else {
		return NULL;
	}
}

void OpenGLMaterial::AddTexture( Texture* texture ) {
	Material::AddTexture( texture );
	
	if ( texture->IsDiffuseMap() ) {
		m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ProgramFeatures::DIFFUSE_TEXTURE );
	} else if ( texture->IsNormalMap() ) {
		m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ProgramFeatures::NORMAL_MAP );
	} else if ( texture->IsSpecularMap() ) {
		m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ProgramFeatures::SPEC_MAP );
	}
}

void OpenGLMaterial::BindMaterial( void ) {
	Material::BindMaterial();
	OpenGLProgram* prog = OpenGLProgramManager::GetInstance()->GetActiveProgram();
	prog->SetUniform( "u_DiffuseColor", m_diffuseColor );	
}

void OpenGLMaterial::UnbindMaterial( void ) {
	Material::UnbindMaterial();
}


}