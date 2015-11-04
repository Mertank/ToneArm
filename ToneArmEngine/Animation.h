/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Animation

		An MD5 animation.

		Created by: Karl Merten
		Created on: 15/07/2014

========================
*/

#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <unordered_map>
#include "AnimationStructs.h"

//Flags from MD5 format
#define TRANSLATE_X		0x01
#define TRANSLATE_Y		0x02
#define TRANSLATE_Z		0x04
#define QUATERNION_X	0x08
#define QUATERNION_Y	0x10
#define QUATERNION_Z	0x20

namespace vgs {

class Animation {
public:
													~Animation( void );		

	static Animation*								CreateAnimationFromBuffer( const char* data, unsigned long dataLength );

	unsigned int									GetAnimationNameHash( void ) const { return m_animNameHash; }
	const char* const								GetAnimationName( void ) const { return m_animationName; }
	void											SetAnimationName( const char* const newName );

	const Skeleton&									GetSkeletonForFrame( unsigned int frameNum ) const;

	inline unsigned int								GetJointCount( void ) { return m_numberOfJoints; }
	inline unsigned int								GetFrameCount( void ) { return m_numberOfFrames; }

	inline float									GetFrameDuration( void ) { return m_frameDuration; }
	inline float									GetAnimationDuration( void ) { return m_animationDuration; }

	int												GetIndexOfJoint( unsigned long hash );
private:
													Animation( void );

	bool											InitializeAnimationFromBuffer( const char* data, unsigned long dataLength );

	char*											ReadHierarchy( char* startingPosition, JointInfoList& destination );
	char*											ReadFrame( char* startingPosition, unsigned frameIndex, FrameDataList& destination );
	char*											ReadBaseFrame( char* startingPosition, BaseFrameJoints& destination );
	char*											ReadBounds( char* startingPosition );
	
	void											ReadFrameBound( char* startPosition, Bound& dest );
	void											ReadJointInfo( char* startPosition, JointInfo& dest );	
	void											ReadBaseFrameJoint( char* startPosition, BaseFrameJoint& dest );

	void											BuildSkeletonFrames( const JointInfoList& jointInfo, const FrameDataList& frameData, const BaseFrameJoints& baseFrameJoints );

	char*											m_animationName;
	unsigned int									m_animNameHash;

	unsigned int									m_numberOfFrames;
    unsigned int									m_numberOfJoints;
    unsigned int									m_frameRate;
    unsigned int									m_numberOfAnimatedComponents;
	unsigned int									m_currentFrame;

	float											m_frameDuration;
	float											m_animationDuration;
	float											m_animTime;

	std::unordered_map<unsigned long, unsigned int>	m_jointHashIndex;
	SkeletonList									m_skeletonFrames;
	Bounds											m_frameBounds;
};

}

#endif //__ANIMATION_H__