/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	ModelNode

	Created by: Vladyslav D.
	Created on: 5/8/2014 4:41:26 PM

========================
*/
#include "ModelNode.h"
#include "RenderMessages.h"
#include "Engine.h"
#include "CachedResourceLoader.h"
#include "AnimationNode.h"


using namespace vgs;

IMPLEMENT_RTTI( ModelNode, RenderableNode )

ModelNode::ModelNode()
{}

ModelNode::~ModelNode()
{
	m_modelChildren.clear();	
}

bool ModelNode::Init(const char* file)
{
	if ( !Node::Init() ) {
		return false;
	}

	CachedResourceLoader* resLoader = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>(EngineModuleType::RESOURCELOADER);

	if (resLoader)
	{
		m_model = resLoader->LoadResource<ModelResource>(file, (char)0xA6);

		if (m_model.get())
		{
			m_model->SetOwnerNodeID(m_id);
			if ( m_model->HasAnimations() )  {
				AnimationNode* animNode = AnimationNode::Create();
				AddChild( animNode );

				const std::vector<Animation*>& anims = m_model->GetAnimations();
				for ( std::vector<Animation*>::const_iterator iter = anims.begin();
					  iter != anims.end(); ++iter ) {
					animNode->AddAnimation( ( *iter ) );
				}

			}

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

ModelNode* ModelNode::CreateWithMesh( float* vertexData, void* indexData, GLenum indexType, unsigned int vertexCount, unsigned int indexCount, VertexComponents::Value vertexComps, const glm::vec3& diffuseColor, const glm::vec3& boxPosition, const glm::vec3& boxSize ) {
	ModelNode* node = new ModelNode();
	if ( node && node->InitWithMesh( vertexData, indexData, indexType, vertexCount, indexCount, vertexComps, diffuseColor, boxPosition, boxSize ) ) {
		return node;
	} else {
		delete node;
		return NULL;
	}
}

bool ModelNode::InitWithMesh( float* vertexData, void* indexData, GLenum indexType, unsigned int vertexCount, unsigned int indexCount, VertexComponents::Value vertexComps, const glm::vec3& diffuseColor, const glm::vec3& boxPosition, const glm::vec3& boxSize ) {
	if ( !Node::Init() ) {
		return false;
	}
	m_renderModule = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );

	CreateModelMessage* modelMsg = m_renderModule->GetRenderMessage<CreateModelMessage>();
	modelMsg->Init( m_id, 1 );
	m_renderModule->SendRenderMessage( modelMsg );

	CreateMeshMessage* meshMsg = m_renderModule->GetRenderMessage<CreateMeshMessage>();
	meshMsg->Init( m_id, vertexData, vertexCount, indexData, indexCount, indexType, vertexComps, diffuseColor, boxPosition, boxSize );

	m_renderModule->SendRenderMessage( meshMsg );

	if ( vertexComps == VertexComponents::POSITION ) {
		LightingMessage* lightingMsg = m_renderModule->GetRenderMessage<LightingMessage>();
		lightingMsg->Init( m_id, false, LightingType::VERTEX_LIT );
		m_renderModule->SendRenderMessage( lightingMsg );
	}

	return true;
}