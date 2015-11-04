/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/28/2014 2:28:20 PM
------------------------------------------------------------------------------------------
*/

#include "ParticleEmitterNodePool.h"

using namespace vgs;

//
// default destructor
//
ParticleEmitterNodePool::ParticleEmitterNodePool() :
	m_poolIndex(0)
{}

//
// destructor
//
ParticleEmitterNodePool::~ParticleEmitterNodePool() 
{}

//
// gives away a ParticleEmitterNode object
//
ParticleEmitterNode* ParticleEmitterNodePool::GetEmitterNode() {

	// update the index
	if (++m_poolIndex >= m_emitterNodes.size()) {
		m_poolIndex = 0;
	}

	unsigned int newIndex = m_poolIndex >= 0 ? m_poolIndex : m_poolIndex - 1;

	// give the node
	return m_emitterNodes[newIndex];
}

ParticleEmitterNodePool* ParticleEmitterNodePool::CreateEmitterPool( unsigned int size ) {
	ParticleEmitterNodePool* pool = new ParticleEmitterNodePool();
	if ( pool && pool->InitializeEmitterPool( size ) ) {
		return pool;
	}

	delete pool;
	return NULL;
}

bool ParticleEmitterNodePool::InitializeEmitterPool( unsigned int size ) {
	if ( !Node::Init() ) {
		return false;	
	}

	m_emitterNodes.resize( size );
	for (unsigned int i = 0; i < size; i++) {
		m_emitterNodes[i] = ParticleEmitterNode::CreateWithSettings(ParticleEmitterSettings());
		AddChild( m_emitterNodes[i] );
	}

	return true;
}