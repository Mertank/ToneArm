/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	RenderableNode

	Created by: Karl Merten
	Created on: 28/07/2014

========================
*/
#include "RenderableNode.h"
#include "RenderModule.h"
#include "ModelNode.h"
#include "AnimationNode.h"

#include <GetTime.h>

#include <iostream>
#include <iomanip>

namespace vgs {
IMPLEMENT_RTTI( RenderableNode, Node )
/*
==========
RenderableNode::RenderableNode

	RenderableNode default constructor
==========
*/
RenderableNode::RenderableNode( void ) :
	m_visible( true ),
	m_opacity(1.0f)
{
	m_renderModule = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
}
/*
==========
RenderableNode::~RenderableNode

	RenderableNode destructor
==========
*/
RenderableNode::~RenderableNode( void ) {
	DestroyMessage* msg = m_renderModule->GetRenderMessage<DestroyMessage>();
	msg->Init( m_id );
	m_renderModule->SendRenderMessage( msg );
}
/*
==========
RenderableNode::AddChild

	Adds the child.
	Sets up parenting on GPU if needed.
==========
*/
void RenderableNode::AddChild( Node* node ) {
	Node::AddChild( node );
	
	if ( node->GetRTTI().DerivesFrom( RenderableNode::rtti ) ) {
		SetParentMessage* msg = m_renderModule->GetRenderMessage<SetParentMessage>();
		msg->Init( node->GetID(), m_id );
		m_renderModule->SendRenderMessage( msg );
	}
}
/*
==========
RenderableNode::SendTransformMessage

	Sends a transform message if needed.
==========
*/
void RenderableNode::SendTransformMessage( void ) {
	if ( IsTransformChanged() ) {
		TransformMessage* msg = m_renderModule->GetRenderMessage<TransformMessage>();
		const glm::mat4& globalModel = GetGlobalModelMatrix();
		msg->Init (m_id, glm::mat3( globalModel ), glm::vec3( globalModel[3] ) );
		//std::cout << "Set time: (id" << m_id << ") " << std::setprecision(16) << RakNet::GetTime() * 0.001 << std::endl;
		m_renderModule->SendRenderMessage( msg );
	}
}
/*
==========
RenderableNode::SendUpdateDiffuseColorMessage

	Updates the diffuse color
==========
*/
void RenderableNode::SendUpdateDiffuseColorMessage(const glm::vec3& color) {
	UpdateDiffuseColorMessage* msg = m_renderModule->GetRenderMessage<UpdateDiffuseColorMessage>();
	msg->Init( m_id, color );
	m_renderModule->SendRenderMessage( msg );
}
/*
==========
RenderableNode::Update

	Checks if a transform was changed
==========
*/
void RenderableNode::Update(float dt) {
	SendTransformMessage();
	Node::Update( dt );
}
/*
==========
RenderableNode::SetVisible

	Sets the visibilty of the node, and all it's children
==========
*/
void RenderableNode::SetVisible( bool visible ) {
	m_visible = visible;
	VisibilityMessage* msg = m_renderModule->GetRenderMessage<VisibilityMessage>();
	msg->Init( m_id, visible );
	m_renderModule->SendRenderMessage( msg );

	BEGIN_STD_LIST_ITERATOR( Node*, m_children )
		if ( currentItem->GetRTTI().DerivesFrom( RenderableNode::rtti ) ) {
			//( ( RenderableNode* )currentItem )->SetVisible( visible );
		}		
	END_STD_LIST_ITERATOR
}
/*
==========
RenderableNode::SetOutlineOnly

	Sets the draw mode of the node
==========
*/
void RenderableNode::SetOutlineOnly( bool outline ) {
	OutlineMessage* msg = m_renderModule->GetRenderMessage<OutlineMessage>();
	msg->Init( m_id, outline );
	m_renderModule->SendRenderMessage( msg );
}
/*
==========
RenderableNode::SetOpacity

	Sets the opacity of the node
==========
*/
void RenderableNode::SetOpacity( float alpha ) {
	UpdateOpacityMessage* msg = m_renderModule->GetRenderMessage<UpdateOpacityMessage>();
	msg->Init( m_id, alpha );
	m_renderModule->SendRenderMessage( msg );

	m_opacity = alpha;
}
/*
==========
RenderableNode::SetLighting

	Sets the lighting type of the node
==========
*/
void RenderableNode::SetLighting( bool lit, LightingType::Value lighting ) {
	LightingMessage* msg = m_renderModule->GetRenderMessage<LightingMessage>();
	msg->Init( m_id, lit, lighting );
	m_renderModule->SendRenderMessage( msg );
}
/*
==========
RenderableNode::SetPassType

	Sets which pass affects the node
==========
*/
void RenderableNode::SetPassType( PassType::Value pass ) {
	UpdatePassTypeMessage* msg = m_renderModule->GetRenderMessage<UpdatePassTypeMessage>();
	msg->Init( m_id, pass );
	m_renderModule->SendRenderMessage( msg );
}

void RenderableNode::RemoveChild( unsigned int id ) {
	Node* node = GetChild( id );
	Node::RemoveChild( id );

	if ( node->GetRTTI().DerivesFrom( RenderableNode::rtti ) ) {
		SetParentMessage* msg = m_renderModule->GetRenderMessage<SetParentMessage>();
		msg->Init( node->GetID(), 0 );
		m_renderModule->SendRenderMessage( msg );
	}
}

void RenderableNode::RemoveChild( Node* node ) {
	Node::RemoveChild( node );

	if ( node->GetRTTI().DerivesFrom( RenderableNode::rtti ) ) {
		SetParentMessage* msg = m_renderModule->GetRenderMessage<SetParentMessage>();
		msg->Init( node->GetID(), 0 );
		m_renderModule->SendRenderMessage( msg );
	}
}

void RenderableNode::RemoveFromParent( void ) {
	if ( m_parent->GetRTTI().DerivesFrom( RenderableNode::rtti ) ) {
		SetParentMessage* msg = m_renderModule->GetRenderMessage<SetParentMessage>();
		msg->Init( m_id, 0 );
		m_renderModule->SendRenderMessage( msg );
	}

	Node::RemoveFromParent();
}

void RenderableNode::SetParentBone( unsigned long boneHash ) {
	if ( m_parent->GetRTTI().DerivesFrom( ModelNode::rtti ) && ( ( ModelNode* )m_parent )->GetChildOfType<AnimationNode>() ) {
		SetParentBoneMessage* msg = m_renderModule->GetRenderMessage<SetParentBoneMessage>();
		msg->Init( m_id, boneHash );
		m_renderModule->SendRenderMessage( msg );
	}
}
}