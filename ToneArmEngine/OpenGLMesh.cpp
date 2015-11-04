/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	OpenGLMesh

	Created by: Karl Merten
	Created on: 01/05/2014

========================
*/
#include "OpenGLMesh.h"
#include "OpenGLProgramManager.h"
#include "OpenGLProgram.h"
#include "CachedResourceLoader.h"
#include "BinaryFileResource.h"
#include "MeshHelper.h"
#include "RenderMessages.h"
#include "OpenGLMaterial.h"
#include "OpenGLRenderModule.h"
#include "Engine.h"
#include "OpenGLBufferData.h"

namespace vgs {
/*
========
OpenGLMesh::OpenGLMesh

	OpenGLMesh default constructor.
========
*/
OpenGLMesh::OpenGLMesh( void ) :
	m_bufferData( NULL ),
	m_drawMode( GL_TRIANGLES ),
	m_requiredFeatures( ProgramFeatures::NONE )
{}
/*
========
OpenGLMesh::~OpenGLMesh

	OpenGLMesh destructor.
========
*/
OpenGLMesh::~OpenGLMesh( void ) {
	delete m_material;
}
/*
========
OpenGLMesh::CreateMeshFromMessage

	Creates a mesh from a message.
========
*/
OpenGLMesh* OpenGLMesh::CreateMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex ) {
	OpenGLMesh* mesh = new OpenGLMesh();
	if ( mesh && mesh->InitializeMeshFromMessage( msg, meshIndex ) ) {
		return mesh;
	} else {
		delete mesh;
		return NULL;
	}
}
/*
========
OpenGLMesh::CreateDuplicate

	Creates a duplicate of a mesh.
========
*/
OpenGLMesh* OpenGLMesh::CreateDuplicate( OpenGLMesh* original, unsigned int originalIndex ) {
	OpenGLMesh* mesh = new OpenGLMesh();
	if ( mesh && mesh->InitializeDuplicate( original, originalIndex ) ) {
		return mesh;
	} else {
		delete mesh;
		return NULL;
	}
}
/*
========
OpenGLMesh::InitializeDuplicate

	Initialize a duplicate of a mesh.
========
*/
bool OpenGLMesh::InitializeDuplicate( OpenGLMesh* original, unsigned int originalIndex ) {
	OpenGLRenderModule* renderer = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>( EngineModuleType::RENDERER );
	m_bufferData = renderer->GetBufferForNode( original->m_nodeID, originalIndex );
	
	m_material = new OpenGLMaterial( *( ( OpenGLMaterial* )original->m_material ) );
	m_drawMode = original->m_drawMode;
	m_drawOutlineOnly = original->m_drawOutlineOnly;
	m_requiredFeatures = original->m_requiredFeatures;
	m_vertexComponents = original->m_vertexComponents;
	
	m_boxOffset = original->m_boxOffset;	
	m_boundingBox.SetDimensions( ( ( BoxShape& )original->GetBoundingBox() ).GetDimensions() );

	return true;
}
/*
========
OpenGLMesh::InitializeMeshFromMessage

	OpenGLMesh initializes mesh from a buffer.
	Stores the location it ended reading at in endPosition.
========
*/
bool OpenGLMesh::InitializeMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex ) {	
	//The only reason this call is safe is because the renderer has to be OpenGL due to the mesh type.
	OpenGLRenderModule* renderModule = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>( EngineModuleType::RENDERER );
	
	m_bufferData = renderModule->GetBufferForNode( msg->nodeID, meshIndex );
	
	m_bufferData->indexType = msg->indexType;	
	m_vertexComponents = msg->vertexComponents;	
	m_bufferData->elementCount = msg->indexCount;

	if ( msg->indexCount == 2 ) {
		m_drawOutlineOnly = true;
	}

	m_material = OpenGLMaterial::CreateMaterialWithColor( msg->diffuseColor );

	UploadData( msg->verticies, msg->vertexCount, msg->indicies, msg->indexCount );

	if ( m_vertexComponents & VertexComponents::NORMAL ) {
		m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ProgramFeatures::NORMALS );
	}

	if ( m_vertexComponents & VertexComponents::TEXTURE ) {
		m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ProgramFeatures::TEX_COORDS );
	}

	if ( ( m_vertexComponents & VertexComponents::MATRIX_INDEX ) && ( m_vertexComponents & VertexComponents::WEIGHTS ) ) {
		m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ProgramFeatures::GPU_SKINNING );
	}

	m_boxOffset = msg->boundingBoxOffset;
	m_boundingBox.SetDimensions( msg->boundingBoxSize );

	delete[] msg->verticies;
	delete[] msg->indicies;

	return true;
}
/*
========
OpenGLMesh::UploadData

	Uploads the vertex/index data to openGL
========
*/
void OpenGLMesh::UploadData( float* verticies, unsigned int vertexCount, void* indicies, unsigned int indexCount ) {
	GLsizeiptr indexSize = ( m_bufferData->indexType == GL_UNSIGNED_SHORT ) ? 2 * indexCount : 4 * indexCount;

	unsigned int stride = ( m_vertexComponents & VertexComponents::NORMAL ) ? 6 : 3;
	if ( m_vertexComponents & VertexComponents::TEXTURE ) {
		stride += 2;
	}

	if ( ( m_vertexComponents & VertexComponents::MATRIX_INDEX ) && ( m_vertexComponents & VertexComponents::WEIGHTS ) ) {
		stride += 8;
	}

	unsigned int vertexAmount = stride;
	stride *= sizeof( float );

	glGenVertexArrays( 1, &m_bufferData->vao );
	glGenBuffers( 1, &m_bufferData->vbo );
	glGenBuffers( 1, &m_bufferData->ibo );

	glBindVertexArray( m_bufferData->vao );

	glBindBuffer( GL_ARRAY_BUFFER, m_bufferData->vbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_bufferData->ibo );

	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, stride, ( void* )( NULL ) );

	unsigned int offset = 3; //Some models may not have normals. This keeps track of that.

	if ( m_vertexComponents & VertexComponents::NORMAL ) {
		glEnableVertexAttribArray( 1 );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, stride, ( void* )( offset * sizeof( float ) ) );
		offset += 3;
	}

	if ( m_vertexComponents & VertexComponents::TEXTURE ) {
		glEnableVertexAttribArray( 2 );
		glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, stride, ( void* )( offset * sizeof( float ) ) );
		offset += 2;
	}

	if ( ( m_vertexComponents & VertexComponents::MATRIX_INDEX ) && ( m_vertexComponents & VertexComponents::WEIGHTS ) ) {
		glEnableVertexAttribArray( 3 );
		glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, stride, ( void* )( offset * sizeof( float ) ) );
		offset += 4;
		
		glEnableVertexAttribArray( 4 );
		glVertexAttribIPointer( 4, 4, GL_INT, stride, ( void* )( offset * sizeof( float ) ) );
		offset += 4;
	}

	glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * vertexAmount * vertexCount, verticies, GL_DYNAMIC_DRAW );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexSize, indicies, GL_STATIC_DRAW );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}
/*
========
OpenGLMesh::AttachTexture

	Attaches a texture to the mesh's material
========
*/
void OpenGLMesh::AttachTexture( Texture* tex ) {
	RenderableMesh::AttachTexture( tex );
	m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ( ( OpenGLMaterial* )m_material )->GetRequiredFeatures() );
}
/*
========
OpenGLMesh::PreRender

	OpenGLMesh GL state preperation
========
*/
void OpenGLMesh::PreRender( void ) {
	m_material->BindMaterial();
}
/*
========
OpenGLMesh::Render

	OpenGLMesh render implementation.
========
*/
void OpenGLMesh::Render( void ) {
	glBindVertexArray( m_bufferData->vao );
	glDrawElements( m_drawOutlineOnly ? GL_LINE_LOOP : m_drawMode, m_bufferData->elementCount, m_bufferData->indexType, NULL );	
	glBindVertexArray( 0 );
}
/*
========
OpenGLMesh::PostRender

	OpenGLMesh GL state cleanup
========
*/
void OpenGLMesh::PostRender( void ) {
	m_material->UnbindMaterial();
}
}