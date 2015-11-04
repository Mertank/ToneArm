/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	OpenGLBufferData

	Created by: Karl Merten
	Created on: 27/05/2014

========================
*/
#include "OpenGLBufferData.h"

namespace vgs {
/*
=========
OpenGLBufferData::OpenGLBufferData

	OpenGLBufferData default constructor.
=========
*/
OpenGLBufferData::OpenGLBufferData( void ) :
	vbo( 0 ),
	vao( 0 ),
	ibo( 0 ),
	elementCount( 0 ),
	indexType( GL_UNSIGNED_INT ),
	m_references( 1 ),
	m_originalNode( 0 )
{}
/*
=========
OpenGLBufferData::~OpenGLBufferData

	OpenGLBufferData destructor.
	Destorys the buffers on the GPU.
=========
*/
OpenGLBufferData::~OpenGLBufferData( void ) {
	glDeleteBuffers( 1, &vbo );
	glDeleteBuffers( 1, &ibo );
	glDeleteVertexArrays( 1, &vao );
}
/*
=========
OpenGLBufferData::CreateForNode

	OpenGLBufferData create.
=========
*/
OpenGLBufferData* OpenGLBufferData::CreateForNode( unsigned int nodeID, unsigned int meshIndex ) {
	OpenGLBufferData* retPtr = new OpenGLBufferData();	
	if ( retPtr ) {
		retPtr->m_originalNode	= nodeID;		
		retPtr->m_meshIndex		= meshIndex;		
	}

	return retPtr;
}
/*
=========
OpenGLBufferData::Duplicate

	Returns a pointer to itself, and increases the counter.
=========
*/
OpenGLBufferData* OpenGLBufferData::Duplicate( void ) {
	++m_references;
	return this;
}
/*
=========
OpenGLBufferData::Destroy

	Reduces reference counter.
	Deletes if it is 0.
=========
*/
void OpenGLBufferData::Destroy( void ) {
	--m_references;
	if ( m_references == 0 ) {
		delete this;
	}
}
}