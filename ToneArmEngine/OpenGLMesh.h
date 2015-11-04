/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLMesh

		An OpenGL Mesh.

		Created by: Karl Merten
		Created on: 01/05/2014

========================
*/

#ifndef __OPENGLMESH_H__
#define __OPENGLMESH_H__

#include "RenderableMesh.h"
#include "OpenGLProgram.h"

namespace vgs {

struct CreateMeshMessage;
class OpenGLBufferData;

class OpenGLMesh : public RenderableMesh {
public:
									OpenGLMesh( void );
	virtual							~OpenGLMesh( void );

	static OpenGLMesh*				CreateMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex );
	static OpenGLMesh*				CreateDuplicate( OpenGLMesh* original, unsigned int originalIndex );

	inline ProgramFeatures::Value	GetRequiredFeatures( void ) const { return m_requiredFeatures; }

	virtual void					PreRender( void ) override;
	virtual void					Render( void ) override;
	virtual void					PostRender( void ) override;
	virtual void					AttachTexture( Texture* tex );
protected:
	bool							InitializeMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex );
	bool							InitializeDuplicate( OpenGLMesh* original, unsigned int originalIndex );

	virtual void					UploadData( float* verticies, unsigned int vertexCount, void* indicies, unsigned int indexCount );

	ProgramFeatures::Value			m_requiredFeatures;

	OpenGLBufferData*				m_bufferData;
	GLenum							m_drawMode;	
};

}

#endif //__OPENGLMESH_H__