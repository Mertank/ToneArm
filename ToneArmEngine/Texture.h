/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Texture

		A texture base class.

		Created by: Karl Merten
		Created on: 16/05/2014

========================
*/

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

namespace vgs {

namespace TextureType {
	enum Value : char {
		NONE			= 0x00,
		DIFFUSE			= 0x01,
		DIFFUSE_W_ALPHA = 0x02,
		NORMAL			= 0x04,
		SPECULAR		= 0x08
	};
};

class Texture {
public:
								Texture( void );
	virtual						~Texture( void );

	Texture*					Duplicate( void );
	void						Destroy( void );

	virtual	void				BindTexture( void ) = 0;
	virtual	void				UnbindTexture( void ) = 0;

	inline bool					IsDiffuseMap( void ) const { return m_textureType == TextureType::DIFFUSE || m_textureType == TextureType::DIFFUSE_W_ALPHA; }
	inline bool					HasAlphaChannel( void ) const { return m_textureType == TextureType::DIFFUSE_W_ALPHA; }
	inline bool					IsNormalMap( void ) const { return m_textureType == TextureType::NORMAL; }
	inline bool					IsSpecularMap( void ) const { return m_textureType == TextureType::SPECULAR; }
	inline TextureType::Value	GetTextureType( void ) const { return m_textureType; }
protected:
	TextureType::Value			m_textureType;
private:
	unsigned int				m_referenceCount;
};

}

#endif //__TEXTURE_H__