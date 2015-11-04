/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	AnimationNode

	Created by: Karl Merten
	Created on: 15/07/2014

========================
*/
#include "AnimationNode.h"
#include "ModelNode.h"
#include "Animation.h"

namespace vgs {

IMPLEMENT_RTTI( AnimationNode, Node )
/*
===========
AnimationNode::AddAnimation

	Adds a playable animation
===========
*/
void AnimationNode::AddAnimation( Animation* anim ) {
	std::unordered_map<unsigned int, Animation*>::iterator animIter = m_anims.find( anim->GetAnimationNameHash() );
	if ( animIter == m_anims.end() ) {
		RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
		AddAnimationMessage* msg = renderer->GetRenderMessage<AddAnimationMessage>();
		msg->Init( GetParent()->GetID(), anim );
		renderer->SendRenderMessage( msg );

		m_anims[anim->GetAnimationNameHash()] = anim;
	} else {
		printf( "Could not add animation %s to Animation Node. Possible name hash collision.\n", anim->GetAnimationName() );
	}	
}
/*
===========
AnimationNode::Init

	Stores renderer.
===========
*/
bool AnimationNode::Init( void ) {
	m_renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	return Node::Init();
}
/*
===========
AnimationNode::Update

	Updates the animtion.
===========
*/
#ifdef TONEARM_DEBUG
void AnimationNode::Update( float dt ) {
	if ( !m_parent || !m_parent->GetRTTI().DerivesFrom( ModelNode::rtti ) ) {
		Log::GetInstance()->WriteToLog( "AnimationNode", "Animation Node is not the child of a model node" );
		return;
	}
}
#endif
/*
===========
AnimationNode::PlayAnimation

	Plays an animation by name.
===========
*/
void AnimationNode::PlayAnimation( const char* name, int loop, float transition, float speed ) {
	PlayAnimation( StringUtils::Hash( name ), loop, transition, speed );
}
/*
===========
AnimationNode::PlayAnimation

	Plays an animation by hash.
===========
*/
void AnimationNode::PlayAnimation( unsigned long name, int loop, float transition, float speed ) {
	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	PlayAnimationMessage* msg = renderer->GetRenderMessage<PlayAnimationMessage>();

	msg->Init( GetParent()->GetID(), name, loop, transition, speed );
	
	renderer->SendRenderMessage( msg );
}
/*
===========
AnimationNode::PlayAdditiveAnimation

	Plays an animation additively.
===========
*/
void AnimationNode::PlayAdditiveAnimation( unsigned long name, int loop, float influence, float speed ) {
	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	PlayAdditiveAnimationMessage* msg = renderer->GetRenderMessage<PlayAdditiveAnimationMessage>();
	msg->Init( GetParent()->GetID(), name, loop, influence, speed );	
	renderer->SendRenderMessage( msg );
}
/*
===========
AnimationNode::PlayBlendedAnimation

	FORMAT: 1st transition time
			2nd amount of animations to blend

	  THEN:( ulong hash, float influence, int loopCount, float speed )
===========
*/
void AnimationNode::PlayBlendedAnimation( float transitionTime, unsigned int count, ... ) {
	va_list args;
	va_start( args, count );

	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	PlayBlendedAnimationsMessage* msg = renderer->GetRenderMessage<PlayBlendedAnimationsMessage>();
	msg->Init( GetParent()->GetID(), transitionTime, count, args );	
	renderer->SendRenderMessage( msg );

	va_end( args );
}
/*
===========
AnimationNode::GetAnimationStatus

	Returns status of animation
===========
*/
AnimationStatus::Value AnimationNode::GetAnimationStatus( void ) {
	return m_renderer->QueryAnimationStatus( GetParent()->GetID() );
}
}