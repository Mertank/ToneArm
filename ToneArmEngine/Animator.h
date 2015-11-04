/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Animator

		Processes the blending between skeleton frames.

		Created by: Karl Merten
		Created on: 18/07/2014

========================
*/
#ifndef __ANIMATOR_H__
#define __ANIMATOR_H__

#include <unordered_map>
#include <GL\glew.h>
#include <SFML\System.hpp>
#include <glm\glm.hpp>
#include "AnimationStructs.h"

namespace vgs {

struct PlayBlendedAnimationsMessage;
class Animation;
class AnimationFrameCalculator;

namespace AnimationStatus {
	enum Value : char {
		PLAYING		= 0,
		STOPPED		= 1,
		TRANSITION	= 2
	};
}

class Animator {
public:
	virtual											~Animator( void );

	virtual void									AddAvailableAnimation( Animation* anim );

	AnimationStatus::Value							GetAnimationStatus( void );
	void											SetAnimationStatus( AnimationStatus::Value status );

	inline unsigned int								GetJointCount( void ) const { return m_animationJointCount; }

	void											SetBindMatricies( glm::mat4* matricies ) { m_inverseBoneMatricies = matricies; }

	virtual void									PreRender( void ) = 0;
	virtual void									PostRender( void ) = 0;
	virtual void									UpdateAnimation( float dt );

	void											PlayAdditiveAnimation( unsigned long nameHash, int loopAmount, float influence, float speed );
	void											PlayAnimation( unsigned long nameHash, int loopAmount, float transitionTime, float speed );
	void											PlayBlendedAnimations( PlayBlendedAnimationsMessage* msg );
	
	const SkeletonJoint*							GetJointNamed( unsigned long hash );
protected:
													Animator( void );
													Animator( const Animator& orig );

	void											SwapAnimationCalculators( void );
	void											SetJointCount( unsigned int joints );

	float											m_animationTransitionTime;
	float											m_animationTransitionTimeElapsed;

	std::unordered_map<unsigned long, Animation*>	m_availableAnimations;
	AnimationStatus::Value							m_animationStatus;

	sf::Mutex										m_animationStatusMutex;
	glm::mat4*										m_inverseBoneMatricies;	

	AnimationFrameCalculator*						m_targetAnimation;
	AnimationFrameCalculator*						m_currentAnimation;
	const Skeleton*									m_currentSkeleton;

	unsigned int									m_animationJointCount;
};

}

#endif //__ANIMATOR_H__