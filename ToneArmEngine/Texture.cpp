/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Texture

	Created by: Karl Merten
	Created on: 16/05/2014

========================
*/

#include "Texture.h"

namespace vgs {
/*
========
Texture::Texture

	Texture default constructor
========
*/
Texture::Texture( void ) :
	m_textureType( TextureType::DIFFUSE ),
	m_referenceCount( 1 )
{}
/*
========
Texture::~Texture

	Texture destructor
========
*/
Texture::~Texture( void ) 
{}
/*
========
Texture::Duplicate

	Duplicates the texture.
========
*/
Texture* Texture::Duplicate( void ) {
	++m_referenceCount;
	return this;
}
/*
========
Texture::Destroy

	Destorys the texture
========
*/
void Texture::Destroy( void ) {
	--m_referenceCount;
	if ( m_referenceCount == 0 ) {
		delete this;
	}
}
}