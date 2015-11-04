/*
========================

Copyright (c) 2014 Vinyl Games Studio

	AnimationFrameCalculator

		Updates animations it contains, and will return the current skeleton frame.

		Created by: Karl Merten
		Created on: 29/07/2014

========================
*/

#ifndef __ANIMATIONFRAMECALCULATOR_H__
#define __ANIMATIONFRAMECALCULATOR_H__

#include "AnimationStructs.h"

namespace vgs {

class Animation;

struct AnimationInfo {
	AnimationInfo( void ) :
		animationPtr( NULL ),
		loopCount( 0 ),
		currentFrame( 0 ),
		timeElapsed( 0.0f ),
		influence( 0.0f ),
		speed( 1.0f )
	{}

	Animation*		animationPtr;

	int				loopCount;
	unsigned int	currentFrame;

	float			timeElapsed;
	float			influence;
	float			speed;
};

class AnimationFrameCalculator {
public:
										~AnimationFrameCalculator( void );
	static AnimationFrameCalculator*	Create( void );

	void								Update( float dt );
	void								UpdateAnimationTimes( float dt );

	Skeleton&							GetCurrentSkeleton( void );
	void								AddAnimation( Animation* anim, float influence, int loopCount, float speed );
	void								AddAdditiveAnimation( Animation* anim, float influence, int loopCount, float speed );
	void								RemoveAnimation( unsigned int index );
	
	static void							Blend( const Skeleton& s1, const Skeleton& s2, float interpolateAmount, Skeleton& blendedDest );
	bool								IsDoneAnimating( void );

	void								ClearAnimations( void );
	void								ApplyInverseJoints( const glm::mat4* matricies );

	void								SetJointCount( unsigned int joints );
private:
										AnimationFrameCalculator( void );
	bool								Initialize( void );

	void								GetFrameNumbers( unsigned int animIndex, unsigned int& start, unsigned int& end );

	unsigned int						m_animationCount;
	
	AnimationInfo						m_animationsPlaying[5];

	Skeleton							m_currentSkeleton;
	Skeleton							m_tempFrameSkeleton; //Make blending easier.
};

}

#endif //__ANIMATIONFRAMECALCULATOR_H__