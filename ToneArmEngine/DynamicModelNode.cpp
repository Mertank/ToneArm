/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	DynamicModelNode

	Created by: Karl Merten
	Created on: 11/06/2014

========================
*/
#include "DynamicModelNode.h"

namespace vgs {

IMPLEMENT_RTTI( DynamicModelNode, ModelNode )
/*
==========
DynamicModelNode::CreateWithMesh

	Creates the node with a mesh
==========
*/
DynamicModelNode* DynamicModelNode::CreateWithMesh( float* vertexData, void* indexData, GLenum indexType, unsigned int vertexCount, unsigned int indexCount, VertexComponents::Value vertexComps, const glm::vec3& diffuseColor, const glm::vec3& boxPosition, const glm::vec3& boxSize ) {
	DynamicModelNode* node = new DynamicModelNode();
	if ( node && node->InitWithMesh( vertexData, indexData, indexType, vertexCount, indexCount, vertexComps, diffuseColor, boxPosition, boxSize ) ) {
		return node;
	} else {
		delete node;
		return NULL;
	}
}
/*
==========
DynamicModelNode::InitWithMesh

	Initializes the node with a mesh
==========
*/
bool DynamicModelNode::InitWithMesh( float* vertexData, void* indexData, GLenum indexType, unsigned int vertexCount, unsigned int indexCount, VertexComponents::Value vertexComps, const glm::vec3& diffuseColor, const glm::vec3& boxPosition, const glm::vec3& boxSize ) {
	m_renderModule = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );

	CreateModelMessage* modelMsg = m_renderModule->GetRenderMessage<CreateModelMessage>();
	modelMsg->Init( m_id, 1 );
	m_renderModule->SendRenderMessage( modelMsg );

	m_boundingBox.SetPosition( boxPosition );
	m_boundingBox.SetDimensions( boxSize );

	CreateDynamicMeshMessage* dynamicMeshMsg = m_renderModule->GetRenderMessage<CreateDynamicMeshMessage>();
	dynamicMeshMsg->Init( m_id, vertexData, vertexCount, indexData, indexCount, indexType, vertexComps, diffuseColor, boxPosition, boxSize );
	m_renderModule->SendRenderMessage( dynamicMeshMsg );

	if ( vertexComps == VertexComponents::POSITION ) {
		LightingMessage* lMsg = m_renderModule->GetRenderMessage<LightingMessage>();
		lMsg->Init( m_id, false, LightingType::VERTEX_LIT );
		m_renderModule->SendRenderMessage( lMsg );
	}

	return true;
}
/*
==========
DynamicModelNode::Update


	Updates the mesh data
==========
*/
void DynamicModelNode::UpdateMesh( float* vertexData, void* indexData, unsigned int vertexCount, unsigned int indexCount, const glm::vec3& boxPosition, const glm::vec3& boxSize ) {
	m_boundingBox.SetPosition( boxPosition );
	m_boundingBox.SetDimensions( boxSize );

	UpdateMeshMessage* msg = m_renderModule->GetRenderMessage<UpdateMeshMessage>();
	msg->Init( m_id, vertexData, vertexCount, indexData, indexCount, boxPosition, boxSize );
	m_renderModule->SendRenderMessage( msg );
}

}