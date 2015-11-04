/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLDynamicMesh

		A mesh that will be updated frequently.

		Created by: Karl Merten
		Created on: 04/06/2014

========================
*/

#ifndef __OPENGLDYNAMICMESH_H__
#define __OPENGLDYNAMICMESH_H__

#include "OpenGLMesh.h"

namespace vgs {

struct UpdateMeshMessage;

class OpenGLDynamicMesh : public OpenGLMesh {
public:
	static OpenGLDynamicMesh*	CreateDynamicMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex );

	void						UpdateData( UpdateMeshMessage* msg );
protected:
	bool						InitializeDynamicMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex );

	virtual void				UploadData( float* verticies, unsigned int vertexCount, void* indicies, unsigned int indexCount );
};

}

#endif