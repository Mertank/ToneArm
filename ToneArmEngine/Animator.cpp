/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Animator

	Created by: Karl Merten
	Created on: 18/07/2014

========================
*/
#include "Animator.h"
#include "Animation.h"
#include "AnimationFrameCalculator.h"
#include "RenderMessages.h"

namespace vgs {
/*
========
Animator::Animator

	Animator default constructor.
========
*/
Animator::Animator( void ) :
	m_animationStatus( AnimationStatus::STOPPED ),
	m_inverseBoneMatricies( NULL ),
	m_animationTransitionTime( 0.0f ),
	m_animationTransitionTimeElapsed( 0.0f ),
	m_animationJointCount( 0 ),
	m_targetAnimation( AnimationFrameCalculator::Create() ),
	m_currentAnimation( AnimationFrameCalculator::Create() )
{
	m_currentSkeleton = &m_currentAnimation->GetCurrentSkeleton();
}
/*
========
Animator::Animator

	Animator copy constructor.
========
*/
Animator::Animator( const Animator& orig ) :
	m_animationStatus( AnimationStatus::STOPPED ),
	m_animationTransitionTime( 0.0f ),
	m_animationTransitionTimeElapsed( 0.0f ),
	m_animationJointCount( orig.GetJointCount() ),
	m_targetAnimation( AnimationFrameCalculator::Create() ),
	m_currentAnimation( AnimationFrameCalculator::Create() ),
	m_inverseBoneMatricies( orig.m_inverseBoneMatricies ) 
{
	m_currentSkeleton = &m_currentAnimation->GetCurrentSkeleton();
}
/*
========
Animator::~Animator

	Animator destructor.
========
*/
Animator::~Animator( void ) {
	delete m_targetAnimation;
	delete m_currentAnimation;
}
/*
========
Animator::AddAvailableAnimation

	Adds an animation to the animator.
========
*/
void Animator::AddAvailableAnimation( Animation* anim ) {
	if ( anim->GetJointCount() != m_animationJointCount ) {
		printf( "Animation %s has %i joints, and and Animator has %i!.", anim->GetAnimationName(), anim->GetJointCount(), m_animationJointCount );
		return;
	}
	unsigned int hash = anim->GetAnimationNameHash();
	std::unordered_map<unsigned long, Animation*>::iterator iter = m_availableAnimations.find( hash );
	if ( iter == m_availableAnimations.end() ) {
		m_availableAnimations[hash] = anim;
	} else {
		printf( "Animation %s already attached to model. Possible string hash collision.\n", anim->GetAnimationName() );
	}
}
/*
========
Animator::GetAnimationStatus

	Gets the status of the animator.
========
*/
AnimationStatus::Value Animator::GetAnimationStatus( void ) {
	AnimationStatus::Value status = AnimationStatus::STOPPED;
	
	m_animationStatusMutex.lock();
	status = m_animationStatus;
	m_animationStatusMutex.unlock();
	
	return status;
}
/*
========
Animator::SetAnimationStatus

	Sets the status of the animator.
========
*/
void Animator::SetAnimationStatus( AnimationStatus::Value status ) {
	m_animationStatusMutex.lock();
	m_animationStatus = status;
	m_animationStatusMutex.unlock();
}
/*
========
Animator::UpdateAnimation

	Updates the animation.
========
*/
void Animator::UpdateAnimation( float dt ) {
	AnimationStatus::Value status = GetAnimationStatus();
	if ( status == AnimationStatus::PLAYING ) { //Only play the primary animation
		m_currentAnimation->Update( dt );
		m_currentAnimation->ApplyInverseJoints( m_inverseBoneMatricies );

		if ( m_currentAnimation->IsDoneAnimating() ) {
			SetAnimationStatus( AnimationStatus::STOPPED );
		}
	} else if ( status == AnimationStatus::TRANSITION ) { //Blending to a secondary animation.
		m_animationTransitionTimeElapsed += dt;
		if ( m_animationTransitionTimeElapsed >= m_animationTransitionTime ) {
			SetAnimationStatus( AnimationStatus::PLAYING );
			SwapAnimationCalculators();
			UpdateAnimation( dt );
		} else {
			m_currentAnimation->Update( dt );
			m_targetAnimation->Update( dt );

			AnimationFrameCalculator::Blend( m_currentAnimation->GetCurrentSkeleton(), m_targetAnimation->GetCurrentSkeleton(), m_animationTransitionTimeElapsed / m_animationTransitionTime, m_currentAnimation->GetCurrentSkeleton() );
			m_currentAnimation->ApplyInverseJoints( m_inverseBoneMatricies );
		}
	}
}
/*
========
Animator::PlayAnimation

	Plays an animation.
========
*/
void Animator::PlayAnimation( unsigned long nameHash, int loopAmount, float transitionTime, float speed ) {
	std::unordered_map<unsigned long, Animation*>::iterator toPlayIter = m_availableAnimations.find( nameHash );
	if ( toPlayIter == m_availableAnimations.end() ) {
		printf( "Animation matching hash %i not found\n", nameHash );
		return;
	}

	if ( transitionTime > 0.0f && !m_currentAnimation->IsDoneAnimating() ) {//Requires blending, and there is an animation currently playing.
		SetAnimationStatus( AnimationStatus::TRANSITION );
		
		m_targetAnimation->ClearAnimations();
		m_targetAnimation->AddAnimation( toPlayIter->second, 1.0f, loopAmount, speed );
	} else {		
		SetAnimationStatus( AnimationStatus::PLAYING );

		m_currentAnimation->ClearAnimations();
		m_currentAnimation->AddAnimation( toPlayIter->second, 1.0f, loopAmount, speed );
	}

	m_animationTransitionTimeElapsed = 0.0f;
	m_animationTransitionTime = transitionTime;
}
/*
========
Animator::SetJointCount

	Sets the number of joints.
========
*/
void Animator::SetJointCount( unsigned int joints ) {
	m_animationJointCount = joints;
	
	m_currentAnimation->SetJointCount( joints );
	m_targetAnimation->SetJointCount( joints );	
}
/*
========
Animator::PlayAnimations

	Plays blended animations.
========
*/
void Animator::PlayBlendedAnimations( PlayBlendedAnimationsMessage* msg ) {
	AnimationFrameCalculator* targetCalc = NULL;
	if ( msg->transitionTime > 0.0f && !m_currentAnimation->IsDoneAnimating() ) {//Requires blending, and there is an animation currently playing.
		SetAnimationStatus( AnimationStatus::TRANSITION );		
		targetCalc = m_targetAnimation;
	} else {		
		SetAnimationStatus( AnimationStatus::PLAYING );
		targetCalc = m_currentAnimation;
	}

	targetCalc->ClearAnimations();
	
	std::unordered_map<unsigned long, Animation*>::iterator toPlayIter;
	for ( unsigned int i = 0; i < msg->animationCount; ++i ) {
		toPlayIter = m_availableAnimations.find( msg->animHash[i] );
		if ( toPlayIter == m_availableAnimations.end() ) {
			printf( "Animation matching hash %i not found\n", msg->animHash[i] );
			break;
		}

		targetCalc->AddAnimation( toPlayIter->second, msg->influences[i], msg->loopAmount[i], msg->playbackSpeeds[i] );
	}

	m_animationTransitionTimeElapsed = 0.0f;
	m_animationTransitionTime = msg->transitionTime;
}
/*
========
Animator::PlayAdditiveAnimation

	Plays an additive animation.
========
*/
void Animator::PlayAdditiveAnimation( unsigned long nameHash, int loopAmount, float influence, float speed ) {
	std::unordered_map<unsigned long, Animation*>::iterator toPlayIter = m_availableAnimations.find( nameHash );
	if ( toPlayIter == m_availableAnimations.end() ) {
		printf( "Animation matching hash %i not found\n", nameHash );
		return;
	}

	AnimationStatus::Value curState = GetAnimationStatus();
	if ( curState == AnimationStatus::PLAYING ) {
		m_currentAnimation->AddAdditiveAnimation( toPlayIter->second, influence, loopAmount, speed );
	} else if ( curState == AnimationStatus::TRANSITION ) {
		m_targetAnimation->AddAdditiveAnimation( toPlayIter->second, influence, loopAmount, speed );
	}
}
/*
========
Animator::SwapAnimationCalculators

	Swaps the calculators
========
*/
void Animator::SwapAnimationCalculators( void ) {
	AnimationFrameCalculator* temp = m_currentAnimation;
	m_currentAnimation = m_targetAnimation;
	m_targetAnimation = temp;
	m_currentSkeleton = &m_currentAnimation->GetCurrentSkeleton();
}

const SkeletonJoint* Animator::GetJointNamed( unsigned long hash ) {
	if ( m_availableAnimations.size() > 0 ) {
		int jointIndex = m_availableAnimations.begin()->second->GetIndexOfJoint( hash );
		if ( jointIndex > -1 ) {
			return &( m_currentSkeleton->joints[jointIndex] );
		}
	}

	return NULL;
}
}