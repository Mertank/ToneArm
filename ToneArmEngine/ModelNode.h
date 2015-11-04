#ifndef __ModelNode_H__
#define __ModelNode_H__

#include "RenderableNode.h"
#include "Engine.h"
#include "RenderModule.h"
#include "ModelResource.h"

#include <memory>

namespace vgs
{
	/*
	========================

		Copyright (c) 2014 Vinyl Games Studio

		ModelNode

			Node that watches transform changes and sends messages to renderer.

			Created by: Vladyslav D.
			Created on: 5/8/2014 4:41:26 PM

	========================
	*/
	class ModelNode : public RenderableNode
	{
		DECLARE_RTTI
		CREATE_MODEL_NODE_METH(ModelNode);		
	public:
										ModelNode();
										~ModelNode()											override;
		
		static ModelNode*				CreateWithMesh( float* vertexData, void* indexData, GLenum indexType, unsigned int vertexCount, unsigned int indexCount, VertexComponents::Value vertexComps, const glm::vec3& diffuseColor, const glm::vec3& boxPosition, const glm::vec3& boxSize );
		
		const BoxShape&					GetBoundingBox( void ) { return m_model->GetBoundingBox(); }
	private:
		bool							Init(const char* file);
		bool							InitWithMesh( float* vertexData, void* indexData, GLenum indexType, unsigned int vertexCount, unsigned int indexCount, VertexComponents::Value vertexComps, const glm::vec3& diffuseColor, const glm::vec3& boxPosition, const glm::vec3& boxSize );

	protected:
		std::vector<ModelNode*>			m_modelChildren;
		std::shared_ptr<ModelResource>	m_model;
	};

} // namespace vgs

#endif __ModelNode_H__