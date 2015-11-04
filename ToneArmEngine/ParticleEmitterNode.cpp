#include "ParticleEmitterNode.h"
#include "Engine.h"
#include "RenderModule.h"

namespace vgs {
IMPLEMENT_RTTI( ParticleEmitterNode, RenderableNode )
/*
========
ParticleEmitterNode::~ParticleEmitterNode

	ParticleEmitterNode destructor
========
*/
ParticleEmitterNode::~ParticleEmitterNode( void ) {
	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	DestroyMessage* msg = renderer->GetRenderMessage<DestroyMessage>();
	msg->Init( m_id );
	renderer->SendRenderMessage( msg );
}
/*
========
ParticleEmitterNode::CreateWithSettings

	Creates an emitter node
========
*/
ParticleEmitterNode* ParticleEmitterNode::CreateWithSettings( const ParticleEmitterSettings& settings ) {
	ParticleEmitterNode* node = new ParticleEmitterNode();
	if ( node && node->InitializeWithSettings( settings ) ) {
		return node;
	}

	delete node;
	return NULL;
}
/*
========
ParticleEmitterNode::InitializeWithSettings

	Inits the emitter
========
*/
bool ParticleEmitterNode::InitializeWithSettings( const ParticleEmitterSettings& settings ) {
	m_settings = settings;

	m_renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	CreateParticleEmitterMessage* msg = m_renderer->GetRenderMessage<CreateParticleEmitterMessage>();
	msg->Init( m_id, settings );
	m_renderer->SendRenderMessage( msg );

	return true;
}
/*
========
ParticleEmitterNode::Emit

	Begins Emitting
========
*/
void ParticleEmitterNode::Emit( bool emit, int amount ) {
	if ( emit ) {
		RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
		EmitParticlesMessage* msg = renderer->GetRenderMessage<EmitParticlesMessage>();
		msg->Init( m_id, amount );
		renderer->SendRenderMessage( msg );

		SendTransformMessage();		
	}
}
/*
========
ParticleEmitterNode::Emit

	Changes emitter settings
========
*/
void ParticleEmitterNode::SetEmitterSettings(const ParticleEmitterSettings& settings) {

	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	UpdateEmitterMessage* msg = renderer->GetRenderMessage<UpdateEmitterMessage>();
	msg->Init( m_id, settings );
	renderer->SendRenderMessage( msg );
}
/*
========
ParticleEmitterNode::IsEmitting

	Returns emission status
========
*/
bool ParticleEmitterNode::IsEmitting( void ) {
	return m_renderer->QueryEmittingStatus( m_id );
}

void ParticleEmitterNode::SetDirection( const glm::vec3& dir ) {
	glm::vec3 up = glm::vec3( 0.0f, 1.0f, 0.0f );
	m_transform.SetLocalAxes( dir, glm::cross( dir, up ), up);
}
}