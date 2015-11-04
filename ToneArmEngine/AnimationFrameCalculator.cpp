/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	AnimationFrameCalculator

	Created by: Karl Merten
	Created on: 29/07/2014

========================
*/
#include "AnimationFrameCalculator.h"
#include "Animation.h"
#include "MathHelper.h"

namespace vgs {
/*
========
AnimationFrameCalculator::AnimationFrameCalculator

	AnimationFrameCalculator default constructor
========
*/
AnimationFrameCalculator::AnimationFrameCalculator( void ) :
	m_animationCount( 0 )
{}
/*
========
AnimationFrameCalculator::~AnimationFrameCalculator

	AnimationFrameCalculator destructor
========
*/
AnimationFrameCalculator::~AnimationFrameCalculator( void ) 
{}
/*
========
AnimationFrameCalculator::Create

	Creates a AnimationFrameCalculator
========
*/
AnimationFrameCalculator* AnimationFrameCalculator::Create( void ) {
	AnimationFrameCalculator* ptr = new AnimationFrameCalculator();
	if ( ptr && ptr->Initialize() ) {
		return ptr;
	}

	delete ptr;
	return NULL;
}
/*
========
AnimationFrameCalculator::Initialize

	Initializes the framecalculator
========
*/
bool AnimationFrameCalculator::Initialize( void ) {
	return true;
}
/*
========
AnimationFrameCalculator::Update

	Updates the animations.
========
*/
void AnimationFrameCalculator::Update( float dt ) {
	UpdateAnimationTimes( dt );

	//Get rid of animations that are completed.
	for ( unsigned int i = 0; i < m_animationCount; ) {
		if ( m_animationsPlaying[i].loopCount == 0 ) {
			RemoveAnimation( i );
			if ( m_animationCount == 0 ) {
				return;
			}
		} else {
			++i;
		}
	}

	if ( m_animationCount == 1 ) {
		float interpolationAmount = std::max( std::min( m_animationsPlaying[0].timeElapsed / m_animationsPlaying[0].animationPtr->GetFrameDuration(), 1.0f ), 0.0f ); //Clamp amount to 0.0f - 1.0f
		
		//Get frame numbers to blend
		unsigned int startFrame, endFrame;
		GetFrameNumbers( 0, startFrame, endFrame );
		Blend( m_animationsPlaying[0].animationPtr->GetSkeletonForFrame( startFrame ), m_animationsPlaying[0].animationPtr->GetSkeletonForFrame( endFrame ), interpolationAmount, m_currentSkeleton );
	} else if ( m_animationCount > 1 ) {
		float totalInfluence = m_animationsPlaying[0].influence;

		float interpolationAmount = std::max( std::min( m_animationsPlaying[0].timeElapsed / m_animationsPlaying[0].animationPtr->GetFrameDuration(), 1.0f ), 0.0f ); //Clamp amount to 0.0f - 1.0f
		unsigned int startFrame, endFrame;

		GetFrameNumbers( 0, startFrame, endFrame );
		Blend( m_animationsPlaying[0].animationPtr->GetSkeletonForFrame( startFrame ), m_animationsPlaying[0].animationPtr->GetSkeletonForFrame( endFrame ), interpolationAmount, m_currentSkeleton );

		for ( unsigned int i = 1; i < m_animationCount; ++i ) {
			//Blend animframe
			interpolationAmount = std::max( std::min( m_animationsPlaying[i].timeElapsed / m_animationsPlaying[i].animationPtr->GetFrameDuration(), 1.0f ), 0.0f ); //Clamp amount to 0.0f - 1.0f
			GetFrameNumbers( i, startFrame, endFrame );
			Blend( m_animationsPlaying[i].animationPtr->GetSkeletonForFrame( startFrame ), m_animationsPlaying[i].animationPtr->GetSkeletonForFrame( endFrame ), interpolationAmount, m_tempFrameSkeleton );

			//Blend into the current skeleton
			interpolationAmount = 1.0f / ( m_animationsPlaying[i].influence + totalInfluence ) * m_animationsPlaying[i].influence;
			Blend( m_currentSkeleton, m_tempFrameSkeleton, interpolationAmount, m_currentSkeleton );
		}
	}	
} 
/*
========
AnimationFrameCalculator::GetFrameNumbers

	Gets the start and end frames of an animation.
========
*/
void AnimationFrameCalculator::GetFrameNumbers( unsigned int animIndex, unsigned int& s, unsigned int& e ) {
	s = e = m_animationsPlaying[animIndex].currentFrame;
	if ( e == m_animationsPlaying[animIndex].animationPtr->GetFrameCount() - 1 ) {
		if ( m_animationsPlaying[animIndex].loopCount > 1 || m_animationsPlaying[animIndex].loopCount < 0 ) {
			e = 0;
		}
	} else {
		++e;
	}
}
/*
========
AnimationFrameCalculator::UpdateAnimationTimes

	Updates the animation times
========
*/
void AnimationFrameCalculator::UpdateAnimationTimes( float dt ) {
	for ( unsigned int i = 0; i < m_animationCount; ++i ) {
		m_animationsPlaying[i].timeElapsed += ( dt * m_animationsPlaying[i].speed );
		
		//Calculate new frame time
		float frameDuration = m_animationsPlaying[i].animationPtr->GetFrameDuration();
		while ( m_animationsPlaying[i].timeElapsed > frameDuration ) {
			 m_animationsPlaying[i].timeElapsed -= frameDuration;
			++m_animationsPlaying[i].currentFrame; //Increase the frame counter
		}

		//Wrap the frames back to 0
		unsigned int frameCount = m_animationsPlaying[i].animationPtr->GetFrameCount();
		while ( m_animationsPlaying[i].currentFrame >= frameCount ) {
			m_animationsPlaying[i].currentFrame -= frameCount;
			if ( m_animationsPlaying[i].loopCount > 0 ) {
				--m_animationsPlaying[i].loopCount;
			}
		}
	}
}
/*
========
AnimationFrameCalculator::GetCurrentSkeleton

	Returns the skeleton for the current frame.
========
*/
Skeleton& AnimationFrameCalculator::GetCurrentSkeleton( void ) {
	return m_currentSkeleton;
}
/*
========
AnimationFrameCalculator::GetCurrentSkeleton

	Returns the skeleton for the current frame.
========
*/
void AnimationFrameCalculator::SetJointCount( unsigned int jointCount ) {
	m_currentSkeleton.jointMatricies.resize( jointCount );
	m_currentSkeleton.joints.resize( jointCount );

	m_tempFrameSkeleton.jointMatricies.resize( jointCount );
	m_tempFrameSkeleton.joints.resize( jointCount );
}
/*
========
AnimationFrameCalculator::AddAnimation

	Adds anm animation to influence this animation.
========
*/
void AnimationFrameCalculator::AddAnimation( Animation* anim, float influence, int loopCount, float speed ) {	
	if ( m_animationCount < 5 ) {
		m_animationsPlaying[m_animationCount].animationPtr	= anim;
		m_animationsPlaying[m_animationCount].timeElapsed	= 0.0f;
		m_animationsPlaying[m_animationCount].influence		= influence;
		m_animationsPlaying[m_animationCount].currentFrame	= 0;
		m_animationsPlaying[m_animationCount].loopCount		= loopCount;
		m_animationsPlaying[m_animationCount].speed			= speed;

		++m_animationCount;
	}
}
/*
========
AnimationFrameCalculator::AddAdditiveAnimation

	Adds an additive animation. Recalculates influences.
========
*/
void AnimationFrameCalculator::AddAdditiveAnimation( Animation* anim, float influence, int loopCount, float speed ) {	
	if ( m_animationCount < 4 ) {
		float influenceRemaining = 1.0f - influence;

		for ( unsigned int i = 0; i < m_animationCount; ++i ) {
			m_animationsPlaying[i].influence = m_animationsPlaying[m_animationCount].influence * influenceRemaining;
		}

		m_animationsPlaying[m_animationCount].animationPtr	= anim;
		m_animationsPlaying[m_animationCount].timeElapsed	= 0.0f;
		m_animationsPlaying[m_animationCount].influence		= influence;
		m_animationsPlaying[m_animationCount].currentFrame	= 0;
		m_animationsPlaying[m_animationCount].loopCount		= loopCount;
		m_animationsPlaying[m_animationCount].speed			= speed;

		++m_animationCount;
	}
}
/*
========
AnimationFrameCalculator::RemoveAnimation

	Removes an animation.
========
*/
void AnimationFrameCalculator::RemoveAnimation( unsigned int index ) {
	if ( m_animationCount == 1 ) {
		ClearAnimations();
	} else {
		m_animationsPlaying[index].animationPtr = m_animationsPlaying[m_animationCount - 1].animationPtr;
		m_animationsPlaying[index].influence	= m_animationsPlaying[m_animationCount - 1].influence;
		m_animationsPlaying[index].timeElapsed	= m_animationsPlaying[m_animationCount - 1].timeElapsed;
		m_animationsPlaying[index].loopCount	= m_animationsPlaying[m_animationCount - 1].loopCount;
		m_animationsPlaying[index].currentFrame = m_animationsPlaying[m_animationCount - 1].currentFrame;
		m_animationsPlaying[index].speed		= m_animationsPlaying[m_animationCount - 1].speed;

		--m_animationCount;
	}	
}
/*
========
AnimationFrameCalculator::Blend

	Blends 2 skeleton frames.
	Saves the result in the 4th parameter.
========
*/
void AnimationFrameCalculator::Blend( const Skeleton& s1, const Skeleton& s2, float interpolateAmount, Skeleton& destination ) {
	unsigned numberOfJoints         = s1.joints.size();
    glm::mat4 boneTranslationMatrix = glm::mat4( 1.0 );

	for ( unsigned i = 0; i < numberOfJoints; i++ ) {
		SkeletonJoint&  goalJoint	= destination.joints[i];
		glm::mat4&      goalMatrix  = destination.jointMatricies[i];

        const SkeletonJoint& joint1	= s1.joints[i];
		const SkeletonJoint& joint2	= s2.joints[i];

		goalJoint.parentID			= joint1.parentID;
		if ( interpolateAmount < FLT_EPSILON ) {
			goalJoint.position	  = joint1.position;
			goalJoint.orientation = joint1.orientation;
		} else if ( interpolateAmount > 1.0f - FLT_EPSILON ) {
			goalJoint.position	  = joint2.position;
			goalJoint.orientation = joint2.orientation;
		} else {
			goalJoint.position	  = joint1.position + ( interpolateAmount * ( joint2.position - joint1.position ) );
			goalJoint.orientation = glm::slerp( joint1.orientation, joint2.orientation, interpolateAmount );
		}

        boneTranslationMatrix[3][0] = goalJoint.position.x;
        boneTranslationMatrix[3][1] = goalJoint.position.y;
        boneTranslationMatrix[3][2] = goalJoint.position.z;

        goalMatrix = boneTranslationMatrix * glm::toMat4( goalJoint.orientation );
	}
}
/*
========
AnimationFrameCalculator::IsDoneAnimating

	Returns if the animator is done.
========
*/
bool AnimationFrameCalculator::IsDoneAnimating( void ) {
	for ( unsigned int i = 0; i < m_animationCount; ++i ) {
		if ( m_animationsPlaying[i].loopCount != 0 ) {
			return false;
		}
	}

	return true;
}
/*
========
AnimationFrameCalculator::ClearAnimations

	Resets the animationCalculator
========
*/
void AnimationFrameCalculator::ClearAnimations( void ) {
	m_animationCount = 0;
}
/*
========
AnimationFrameCalculator::ApplyInverseJoints

	Applies the inverse joint matricies to the current skeleton.
========
*/
void AnimationFrameCalculator::ApplyInverseJoints( const glm::mat4* matricies ) {
	if ( m_animationCount > 0 ) {
		unsigned int i = 0;
		for ( std::vector<glm::mat4>::iterator iter = m_currentSkeleton.jointMatricies.begin();
				iter != m_currentSkeleton.jointMatricies.end(); ++iter, ++i ) {
			( *iter ) = ( ( *iter ) * matricies[i] );
		}
	}
}
}