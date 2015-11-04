/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	OpenGLDynamicMesh

	Created by: Karl Merten
	Created on: 04/06/2014

========================
*/
#include "OpenGLDynamicMesh.h"
#include "OpenGLRenderModule.h"
#include "Engine.h"
#include "OpenGLMaterial.h"
#include "OpenGLBufferData.h"

namespace vgs {

/*
========
OpenGLDynamicMesh::CreateMeshFromMessage

	Creates a mesh from a message.
========
*/
OpenGLDynamicMesh* OpenGLDynamicMesh::CreateDynamicMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex ) {
	OpenGLDynamicMesh* mesh = new OpenGLDynamicMesh();
	if ( mesh && mesh->InitializeDynamicMeshFromMessage( msg, meshIndex ) ) {
		return mesh;
	} else {
		delete mesh;
		return NULL;
	}
}
/*
========
OpenGLDynamicMesh::InitializeMeshFromMessage

	OpenGLDynamicMesh initializes mesh from a buffer.
	Stores the location it ended reading at in endPosition.
========
*/
bool OpenGLDynamicMesh::InitializeDynamicMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex ) {	
	//The only reason this call is safe is because the renderer has to be OpenGL due to the mesh type.
	OpenGLRenderModule* renderModule = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>( EngineModuleType::RENDERER );
	
	m_bufferData = renderModule->GetBufferForNode( msg->nodeID, meshIndex );
	
	m_bufferData->indexType = msg->indexType;	
	m_vertexComponents = msg->vertexComponents;	
	m_bufferData->elementCount = msg->indexCount;

	m_material = OpenGLMaterial::CreateMaterialWithColor( msg->diffuseColor );

	UploadData( msg->verticies, msg->vertexCount, msg->indicies, msg->indexCount );

	if ( m_vertexComponents & VertexComponents::NORMAL ) {
		m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ProgramFeatures::NORMALS );
	}

	if ( m_vertexComponents & VertexComponents::TEXTURE ) {
		m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ProgramFeatures::TEX_COORDS );
	}

	delete[] msg->verticies;
	delete[] msg->indicies;

	return true;
}
/*
========
OpenGLDynamicMesh::UploadData

	Uploads the vertex/index data to openGL
========
*/
void OpenGLDynamicMesh::UploadData( float* verticies, unsigned int vertexCount, void* indicies, unsigned int indexCount ) {
	GLsizeiptr indexSize = ( m_bufferData->indexType == GL_UNSIGNED_SHORT ) ? 2 * indexCount : 4 * indexCount;

	unsigned int stride = ( m_vertexComponents & VertexComponents::NORMAL ) ? 6 : 3;
	if ( m_vertexComponents & VertexComponents::TEXTURE ) {
		stride += 2;
	}
	unsigned int vertexAmount = stride;
	stride *= sizeof( float );

	glGenVertexArrays( 1, &m_bufferData->vao );
	glGenBuffers( 1, &m_bufferData->vbo );
	glGenBuffers( 1, &m_bufferData->ibo );

	glBindVertexArray( m_bufferData->vao );

	glBindBuffer( GL_ARRAY_BUFFER, m_bufferData->vbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_bufferData->ibo );

	GLuint posAttrib = 0;
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, false, stride, ( void* )( NULL ) );

	unsigned int offset = 3; //Some models may not have normals. This keeps track of that.

	if ( m_vertexComponents & VertexComponents::NORMAL ) {
		GLuint normalAttrib = 1;
		glEnableVertexAttribArray( normalAttrib );
		glVertexAttribPointer( normalAttrib, 3, GL_FLOAT, false, stride, ( void* )( offset * sizeof( float ) ) );
		offset += 3;
	}

	if ( m_vertexComponents & VertexComponents::TEXTURE ) {
		GLuint textureAttrib = 2;
		glEnableVertexAttribArray( textureAttrib );
		glVertexAttribPointer( textureAttrib, 2, GL_FLOAT, false, stride, ( void* )( offset * sizeof( float ) ) );
	}

	glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * vertexAmount * vertexCount, verticies, GL_DYNAMIC_DRAW );	
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexSize, indicies, GL_DYNAMIC_DRAW );

	glBindVertexArray( 0 );
}
/*
========
OpenGLDynamicMesh::UpdateData

	Uploads the vertex/index data to openGL
========
*/
void OpenGLDynamicMesh::UpdateData( UpdateMeshMessage* msg ) {
	GLsizeiptr indexSize = ( m_bufferData->indexType == GL_UNSIGNED_SHORT ) ? 2 * msg->indexCount : 4 * msg->indexCount;

	unsigned int stride = ( m_vertexComponents & VertexComponents::NORMAL ) ? 6 : 3;
	if ( m_vertexComponents & VertexComponents::TEXTURE ) {
		stride += 2;
	}
	unsigned int vertexAmount = stride;
	stride *= sizeof( float );

	glBindVertexArray( m_bufferData->vao );

	glBindBuffer( GL_ARRAY_BUFFER, m_bufferData->vbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_bufferData->ibo );

	GLuint posAttrib = 0;
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, false, stride, ( void* )( NULL ) );

	unsigned int offset = 3; //Some models may not have normals. This keeps track of that.

	if ( m_vertexComponents & VertexComponents::NORMAL ) {
		GLuint normalAttrib = 1;
		glEnableVertexAttribArray( normalAttrib );
		glVertexAttribPointer( normalAttrib, 3, GL_FLOAT, false, stride, ( void* )( offset * sizeof( float ) ) );
		offset += 3;
	}

	if ( m_vertexComponents & VertexComponents::TEXTURE ) {
		GLuint textureAttrib = 2;
		glEnableVertexAttribArray( textureAttrib );
		glVertexAttribPointer( textureAttrib, 2, GL_FLOAT, false, stride, ( void* )( offset * sizeof( float ) ) );
	}

	glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * vertexAmount * msg->vertexCount, msg->verticies, GL_DYNAMIC_DRAW );	
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexSize, msg->indicies, GL_DYNAMIC_DRAW );

	glBindVertexArray( 0 );

	m_bufferData->elementCount = msg->indexCount;
}
}