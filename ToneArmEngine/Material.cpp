/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Material

	Created by: Karl Merten
	Created on: 16/05/2014

========================
*/

#include "Material.h"
#include "Log.h"

namespace vgs {
/*
========
Material::Material

	Material default constructor
========
*/
Material::Material( void ) :
	m_textureCount( 0 ),
	m_textures( NULL ),
	m_supportedTextures( TextureType::NONE ),
	m_opacity( 1.0f )
{}
/*
========
Material::~Material

	Material destructor
========
*/
Material::~Material( void ) {
	for ( unsigned int i = 0; i < m_textureCount; ++i ) {
		m_textures[i]->Destroy();
	}

	delete[] m_textures;
}
/*
========
Material::Material

	Material copy constructor
========
*/
Material::Material( const Material& orig ) {
	m_textureCount		= orig.m_textureCount;
	m_supportedTextures = orig.m_supportedTextures;
	m_diffuseColor		= orig.m_diffuseColor;
	m_opacity			= orig.m_opacity;

	m_textures = new Texture*[m_textureCount];

	for ( unsigned int i = 0; i < m_textureCount; ++i ) {
		m_textures[i] = orig.m_textures[i]->Duplicate();
	}
}
/*
========
Material::AddTexture

	Adds a texture to the material
========
*/
void Material::AddTexture( Texture* texture ) {
	if ( texture->GetTextureType() & m_supportedTextures ) {
		Log::GetInstance()->WriteToLog( "Material", "Texture of type has already been added" );
		return;
	}

	Texture** newTextures = new Texture*[m_textureCount + 1];
	for ( unsigned int i = 0; i < m_textureCount; ++i ) {
		newTextures[i] = m_textures[i];
	}

	newTextures[m_textureCount] = texture;
	++m_textureCount;
	m_textures = newTextures;
	m_supportedTextures = ( TextureType::Value )( m_supportedTextures | texture->GetTextureType() );
}
/*
========
Material::BindMaterial

	Binds the material for use
========
*/
void Material::BindMaterial( void ) {
	for ( unsigned int i = 0; i < m_textureCount; ++i ) {
		if ( m_textures[i] ) {
			m_textures[i]->BindTexture();
		}
	}
}
/*
========
Material::UnbindMaterial

	Unbinds the material to reset
========
*/
void Material::UnbindMaterial( void ) {
	for ( unsigned int i = 0; i < m_textureCount; ++i ) {
		if ( m_textures[i] ) {
			m_textures[i]->UnbindTexture();
		}
	}
}
}