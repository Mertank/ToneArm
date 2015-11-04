/*
========================

Copyright (c) 2014 Vinyl Games Studio

	DynamicModelNode

		A model node with a mesh that updates frequently.

		Created by: Karl Merten
		Created on: 11/06/2014

========================
*/

#ifndef __DYNAMICMODELNODE_H__
#define __DYNAMICMODELNODE_H__

#include "ModelNode.h"

namespace vgs {

class DynamicModelNode : public ModelNode {
								DECLARE_RTTI
public:
	static DynamicModelNode*	CreateWithMesh( float* vertexData, void* indexData, GLenum indexType, unsigned int vertexCount, unsigned int indexCount, VertexComponents::Value vertexComps, const glm::vec3& diffuseColor, const glm::vec3& boxPosition, const glm::vec3& boxSize );
	void						UpdateMesh( float* vertexData, void* indexData, unsigned int vertexCount, unsigned int indexCount, const glm::vec3& boxPosition, const glm::vec3& boxSize );
protected:
	bool						InitWithMesh( float* vertexData, void* indexData, GLenum indexType, unsigned int vertexCount, unsigned int indexCount, VertexComponents::Value vertexComps, const glm::vec3& diffuseColor, const glm::vec3& boxPosition, const glm::vec3& boxSize );

	BoxShape					m_boundingBox;
};

}

#endif //__DYNAMICMODELNODE_H__