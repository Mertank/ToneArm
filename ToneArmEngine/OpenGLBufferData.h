/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLBufferData

		Stores data to bind meshes.
		Will delete itself once no one is referencing it anymore.

		Created by: Karl Merten
		Created on: 27/05/2014

========================
*/

#ifndef __OPENGLBUFFERDATA_H__
#define __OPENGLBUFFERDATA_H__

#include <GL\glew.h>
namespace vgs {

class OpenGLBufferData {
public:
								~OpenGLBufferData( void );		
	
	static OpenGLBufferData*	CreateForNode( unsigned int nodeID, unsigned int meshIndex );

	OpenGLBufferData*			Duplicate( void );
	void						Destroy( void );

	inline unsigned int			GetOriginalNodeID( void ) const { return m_originalNode; }
	inline unsigned int			GetMeshIndex( void ) const { return m_meshIndex; }

	GLuint						vbo;
	GLuint						ibo;
	GLuint						vao;
	GLuint						elementCount;
	GLenum						indexType;
private:
								OpenGLBufferData( void );

	unsigned int				m_references;
	unsigned int				m_originalNode;
	unsigned int				m_meshIndex;

};

}

#endif //__OPENGLBUFFERDATA_H__