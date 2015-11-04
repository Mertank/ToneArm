/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/28/2014 2:28:20 PM
------------------------------------------------------------------------------------------
*/

#ifndef __PARTICLE_EMITTER_NODE_POOL_H__
#define __PARTICLE_EMITTER_NODE_POOL_H__

#include "VGSMacro.h"
#include "ParticleEmitterNode.h"

namespace vgs
{

class ParticleEmitterNode;

/*
------------------------------------------------------------------------------------------
	ParticleEmitterNodePool

	A container for ParticleEmitterNode objects thta gives them away on request.
------------------------------------------------------------------------------------------
*/
class ParticleEmitterNodePool : public Node {

public:
										~ParticleEmitterNodePool();

	static ParticleEmitterNodePool*		CreateEmitterPool( unsigned int size );											
	ParticleEmitterNode*				GetEmitterNode();

private:
										ParticleEmitterNodePool( void );
	bool								InitializeEmitterPool( unsigned int size );

	std::vector<ParticleEmitterNode*>	m_emitterNodes;
	
	unsigned int						m_poolIndex;
};

}

#endif __PARTICLE_EMITTER_NODE_POOL_H__