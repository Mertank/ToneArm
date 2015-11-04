/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Material

		A material for a model.

		Created by: Karl Merten
		Created on: 16/05/2014

========================
*/

#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "Texture.h"
#include <glm\glm.hpp>

namespace vgs {

class Material {
public:
							Material( const Material& orig );
	virtual					~Material( void );
	
	virtual void			AddTexture( Texture* texture );
	bool					HasTexture( TextureType::Value type ) const { return ( m_supportedTextures & type ) == type; }

	inline void				SetColor( const glm::vec3& newColor ) { m_diffuseColor = newColor; }
	inline const glm::vec3&	GetColor( void ) const { return m_diffuseColor; }

	inline void				SetOpacity( float alpha ) { m_opacity = alpha; }
	inline float			GetOpacity( void ) { return m_opacity; }

	virtual void			BindMaterial( void );
	virtual void			UnbindMaterial( void );
protected:
							Material( void );

	TextureType::Value		m_supportedTextures;
	
	unsigned int			m_textureCount;
	Texture**				m_textures;

	glm::vec3				m_diffuseColor;

	float					m_opacity;
};

}

#endif //__MATERIAL_H__