/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLTexture

		OpenGL implementation of Texture object.

		Created by: Karl Merten
		Created on: 19/05/2014 

========================
*/

#ifndef __OPENGLTEXTURE_H__
#define __OPENGLTEXTURE_H__

#include "Texture.h"
#include <GL\glew.h>

namespace vgs {

struct CreateTextureMessage;

class OpenGLTexture : public Texture {
public:
	virtual					~OpenGLTexture( void );
	static OpenGLTexture*	CreateTextureFromMessage( CreateTextureMessage* msg );
private:
							OpenGLTexture( void );

	bool					InitializeTexture( CreateTextureMessage* msg );
	bool					UploadTexture( char* data, char bytesPerPixel );

	virtual void			BindTexture( void );
	virtual	void			UnbindTexture( void );

	GLuint					m_textureHandle;
	unsigned int			m_textureWidth;
	unsigned int			m_textureHeight;
};

}

#endif //__OPENGLTEXTURE_H__