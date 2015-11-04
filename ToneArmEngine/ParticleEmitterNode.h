/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ParticleEmitterNode

		Node for a particle emitter.

		Created by: Karl Merten
		Created on: 24/07/2014

========================
*/
#ifndef __PARTICLEEMITTERNODE_H__
#define __PARTICLEEMITTERNODE_H__

#include "RenderableNode.h"
#include "ParticleEmitter.h"

namespace vgs {

class ParticleEmitterNode : public RenderableNode {
									DECLARE_RTTI
public:
									~ParticleEmitterNode( void );
	static ParticleEmitterNode*		CreateWithSettings( const ParticleEmitterSettings& settings );
	
	virtual void					Update( float dt ) { RenderableNode::Update( dt ); }
	void							Emit( bool emit = true, int amount = -1 );
	void							SetEmitterSettings(const ParticleEmitterSettings& settings);
	bool							IsEmitting( void );

	void							SetDirection( const glm::vec3& dir );
private:
	bool							InitializeWithSettings( const ParticleEmitterSettings& settings );

	ParticleEmitterSettings			m_settings;
	RenderModule*					m_renderer;
};

}

#endif //__PARTICLEEMITTERNODE_H__