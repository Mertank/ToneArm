/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	OpenGLTexture

	Created by: Karl Merten
	Created on: 19/05/2014 

========================
*/

#include "OpenGLTexture.h"
#include "RenderMessages.h"

#include <cstdlib>

namespace vgs {

OpenGLTexture::OpenGLTexture( void ) :
	m_textureHandle( 0 ),
	m_textureHeight( 0 ),
	m_textureWidth( 0 )
{}

OpenGLTexture::~OpenGLTexture( void ) {
	glDeleteTextures( 1, &m_textureHandle );
}

OpenGLTexture* OpenGLTexture::CreateTextureFromMessage( CreateTextureMessage* msg ) {
	OpenGLTexture* retPtr = new OpenGLTexture();
	if ( retPtr && retPtr->InitializeTexture( msg ) ) {
		return retPtr;
	} else {
		delete retPtr;
		return NULL;
	}
}

bool OpenGLTexture::InitializeTexture( CreateTextureMessage* msg ) {
	m_textureHeight = msg->textureHeight;
	m_textureWidth = msg->textureWidth;

	m_textureType = msg->textureType;

	bool ret = UploadTexture( msg->data, msg->bytesPerPixel );

	delete[] msg->data;

	return ret;
}

bool OpenGLTexture::UploadTexture( char* data, char bytesPerPixel ) {
	glGenTextures( 1, &m_textureHandle );
	if ( m_textureHandle == 0 ) { return false; }

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_textureHandle );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	GLenum format = ( bytesPerPixel == 3 ) ? GL_RGB : GL_RGBA;
	glTexImage2D( GL_TEXTURE_2D, 0, format, m_textureWidth, m_textureHeight, 0, format, GL_UNSIGNED_BYTE, data );

	glGenerateMipmap( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );

	return true;
}

void OpenGLTexture::BindTexture( void ) {
	if ( IsDiffuseMap() ) {
		glActiveTexture( GL_TEXTURE0 );
	} else if ( IsNormalMap() ) {
		glActiveTexture( GL_TEXTURE1 );
	} else if ( IsSpecularMap() ) {
		glActiveTexture( GL_TEXTURE2 );
	}

	glBindTexture( GL_TEXTURE_2D, m_textureHandle );
}

void OpenGLTexture::UnbindTexture( void ) {
	if ( IsDiffuseMap() ) {
		glActiveTexture( GL_TEXTURE0 );
	} else if ( IsNormalMap() ) {
		glActiveTexture( GL_TEXTURE1 );
	} else if ( IsSpecularMap() ) {
		glActiveTexture( GL_TEXTURE2 );
	}

	glBindTexture( GL_TEXTURE_2D, 0 );
}

}