/*
========================

Copyright (c) 2014 Vinyl Games Studio

	AnimationNode

		A node that sends the animation data to the renderer.

		Created by: Karl Merten
		Created on: 15/07/2014

========================
*/

#ifndef __ANIMATIONNODE_H__
#define __ANIMATIONNODE_H__

#include "Node.h"
#include "Animator.h"
#include <unordered_map>

namespace vgs {

class Animation;
class RenderModule;

typedef std::unordered_map<unsigned int, Animation*> Animations;

class AnimationNode : public Node {
							DECLARE_RTTI
public:
							CREATE_METH( AnimationNode )
							~AnimationNode( void ) {}

	void					AddAnimation( Animation* anim );
	void					PlayAnimation( const char* name, int loop = -1, float transition = 0.2f, float speed = 1.0f );
	void					PlayAnimation( unsigned long name, int loop = -1, float transition = 0.2f, float speed = 1.0f );
	void					PlayAdditiveAnimation( unsigned long name, int loop, float influence, float speed );
	void					PlayBlendedAnimation( float transitionTime, unsigned int count, ... );

	AnimationStatus::Value	GetAnimationStatus( void );
#ifdef TONEARM_DEBUG
	virtual void			Update( float dt ) override;
#endif
private:
	virtual bool			Init( void );

	Animations				m_anims;
	RenderModule*			m_renderer;
};

}

#endif //__ANIMATIONNODE_H__