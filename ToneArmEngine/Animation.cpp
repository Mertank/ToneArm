/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Animation

	Created by: Karl Merten
	Created on: 15/07/2014

========================
*/
#include "Animation.h"
#include "StringUtils.h"
#include "MathHelper.h"
#include "Engine.h"
#include "RenderModule.h"

namespace vgs {

/*
========
Animation::Animation

	Animation default constructor
========
*/
Animation::Animation( void ) :
	m_animationName( NULL ),
	m_animNameHash( 0 ),
	m_animationDuration( 0.0f ),
	m_numberOfFrames( 0 ),
	m_numberOfJoints( 0 ),
	m_frameRate( 0 ),
	m_numberOfAnimatedComponents( 0 ),
	m_frameDuration( 0.0f ),
	m_animTime( 0.0f )
{}
/*
========
Animation::~Animation

	Animation destructor
========
*/
Animation::~Animation( void ) {
	delete[] m_animationName;
}
/*
========
Animation::CreateAnimationFromBuffer

	Creates an animation from the buffer
========
*/
Animation* Animation::CreateAnimationFromBuffer( const char* data, unsigned long dataLength ) {
	Animation* anim = new Animation();
	if ( anim && anim->InitializeAnimationFromBuffer( data, dataLength ) ) {
		return anim;
	}

	delete anim;
	return NULL;
}
/*
========
Animation::InitializeAnimationFromBuffer

	Initializes the animation.
========
*/
bool Animation::InitializeAnimationFromBuffer( const char* buffer, unsigned long dataLength ) {
	char* data			= ( char* )buffer;
	char* nextLineToken	= NULL;
	char* currentLine   = strtok_s( data, "\n", &nextLineToken );

	FrameDataList	frameData;
	JointInfoList	jointInfo;
	BaseFrameJoints	baseFrameJoints;

	while ( currentLine != NULL ) {
        char* nextParam     = NULL;
        char* currentParam  = strtok_s( currentLine, " ", &nextParam );

        if ( StringUtils::IsEqual( currentParam, "MD5Version" ) ) { //Read the version
			 nextParam[2] = '\0'; //Truncate to 2 chars.
            if ( !StringUtils::IsEqual( nextParam, "10" ) ) {
				printf( "Only MD5Version 10 is supported\n" );
				return false;                
			}
        } else if ( StringUtils::IsEqual( currentParam, "numFrames" ) ) { //Read numFrames
            m_numberOfFrames = std::atoi( nextParam );
            frameData.resize( m_numberOfFrames );
			m_frameBounds.resize( m_numberOfFrames );
			m_skeletonFrames.resize( m_numberOfFrames );
        } else if ( StringUtils::IsEqual( currentParam, "numJoints" ) ) { //Read numJoints
            m_numberOfJoints = std::atoi( nextParam );
			jointInfo.resize( m_numberOfJoints );
			baseFrameJoints.resize( m_numberOfJoints );
		} else if ( StringUtils::IsEqual( currentParam, "frameRate" ) ) { //Read frameRate
            m_frameRate = std::atoi( nextParam );
		} else if ( StringUtils::IsEqual( currentParam, "numAnimatedComponents" ) ) { //Read numAnimatedComponents
            m_numberOfAnimatedComponents = std::atoi( nextParam );
			//Resize data pointer to hold enough floats to hold data.
			for ( FrameDataList::iterator currentFrame = frameData.begin();
				  currentFrame != frameData.end(); ++currentFrame ) {
				delete[] currentFrame->data;
				currentFrame->data = new float[m_numberOfAnimatedComponents];
			}			  
        } else if ( StringUtils::IsEqual( currentParam, "hierarchy" ) ) { //Read hierarchy
			if ( jointInfo.size() == 0 ) {
                printf( "numJoints was not specified\n" );
                return false;
            }
            nextLineToken = ReadHierarchy( nextLineToken, jointInfo );
        } else if ( StringUtils::IsEqual( currentParam, "bounds" ) ) { //Read the bounds
            if ( m_frameBounds.size() == 0 ) {
                printf( "numFrames was not specified\n" );
                return false;
            }
            nextLineToken = ReadBounds( nextLineToken );
        } else if ( StringUtils::IsEqual( currentParam, "baseframe" ) ) { //Read the baseframe
            if ( m_frameBounds.size() == 0 ) {
                printf( "numJoints was not specified\n" );
                return false;
            }
            nextLineToken = ReadBaseFrame( nextLineToken, baseFrameJoints );
        } else if ( StringUtils::IsEqual( currentParam, "frame" ) ) { //Read a frame
            if ( frameData.size() == 0 ) {
                printf( "numFrames was not specified\n" );
                return false;
            }
			unsigned frameIndex = std::atoi( strtok_s( NULL, " ", &nextParam ) );
            nextLineToken = ReadFrame( nextLineToken, frameIndex, frameData );
        }

		currentLine = strtok_s( NULL, "\n", &nextLineToken );

		if ( ( unsigned int )( currentLine - buffer ) > dataLength ) {
			break;
		}
	}

	m_frameDuration		= 1.0f / m_frameRate;
	m_animationDuration = m_frameDuration * m_numberOfFrames;

	BuildSkeletonFrames( jointInfo, frameData, baseFrameJoints );

	return true;
}
/*
========
Animation::SetAnimationName

	Sets the name of the animation.
========
*/
void Animation::SetAnimationName( const char* const newName ) {
	delete[] m_animationName;
	m_animationName = StringUtils::CopyCString( newName );
	m_animNameHash = StringUtils::Hash( newName );
}
/*
========
Animation::GetSkeletonForFrame

	Returns the skeleton for the frame number.
========
*/
 const Skeleton& Animation::GetSkeletonForFrame( unsigned int frameNum ) const {
	unsigned int actualFrame = frameNum;
	while ( frameNum > m_numberOfFrames ) {
		actualFrame -= m_numberOfFrames;		
	}

	return m_skeletonFrames[actualFrame];
}
/*
=============
Animation::ReadHierarchy

	Reads in the hierarchy information.
=============
*/
char* Animation::ReadHierarchy( char* startingPosition, JointInfoList& destination ) {
	char* nextLine      = NULL;
	char* currentLine   = strtok_s( startingPosition, "\n", &nextLine );
	char* nextToken     = NULL;
	int   jointIndex    = 0;    

	while ( currentLine[0] != '}' ) {     
		ReadJointInfo( currentLine, destination[jointIndex] );

		++jointIndex;
		currentLine = strtok_s( NULL, "\n", &nextLine );
	}

	return nextLine;
}
/*
=============
Animation::ReadJointInfo

	Reads the current line into a JointInfo struct
=============
*/
void Animation::ReadJointInfo( char* startPosition, JointInfo& dest ) {
	char* nextToken = NULL;

	dest.name		= strtok_s( startPosition, "\t\"", &nextToken );	 //Read the joint name	
	dest.parentID	= std::atoi( strtok_s( NULL, "\t ", &nextToken ) );	 //Joint's parent id
	dest.flags		= std::atoi( strtok_s( NULL, "\t ", &nextToken ) );	 //Joint's flags
	dest.startIndex = std::atoi( strtok_s( NULL, "\t ", &nextToken ) );	 //Joint's startIndex
}
/*
=============
Animation::ReadBounds

	Reads in the bounds information
=============
*/
char* Animation::ReadBounds( char* startingPosition ) {
	char* nextLine      = NULL;
	char* currentLine   = strtok_s( startingPosition, "\n", &nextLine );
	char* nextToken     = NULL;
	int   boundsIndex   = 0;    

	while ( currentLine[0] != '}' ) {     
		ReadFrameBound( currentLine, m_frameBounds[boundsIndex] );
		++boundsIndex;
		currentLine = strtok_s( NULL, "\n", &nextLine );
	}

	return nextLine;
}
/*
=============
Animation::ReadFrameBound

	Reads in a frame bound.
=============
*/
void Animation::ReadFrameBound( char* startPosition, Bound& dest ) {
	char* nextToken = NULL;
	nextToken = strtok_s( startPosition, "\t", &nextToken );
	
	StringUtils::ToVec3( strtok_s( NULL, "()", &nextToken ), " ", dest.minBounds ); //Read min bounds
	++nextToken;																	//Skip Space
	StringUtils::ToVec3( strtok_s( NULL, "()", &nextToken ), " ", dest.maxBounds ); //Read max bounds
}
/*
=============
Animation::ReadBaseFrameJoint

	Reads in a base frame joint.
=============
*/
void Animation::ReadBaseFrameJoint( char* startPosition, BaseFrameJoint& dest ) {
	char* nextToken = NULL;
	nextToken = strtok_s( startPosition, "\t", &nextToken );
	
	StringUtils::ToVec3( strtok_s( NULL, "()", &nextToken ), " ", dest.position );	  //Read joint position
	++nextToken;																	  //Skip Space
	StringUtils::ToQuat( strtok_s( NULL, "()", &nextToken ), " ", dest.orientation ); //Read joint orientation
}
/*
=============
Animation::ReadBaseFrame

	Reads in the base frame information
=============
*/
char*Animation::ReadBaseFrame( char* startingPosition, BaseFrameJoints& destination ) {
	char* nextLine      = NULL;
	char* currentLine   = strtok_s( startingPosition, "\n", &nextLine );
	char* nextToken     = NULL;
	int   jointIndex	= 0;    

	while ( currentLine[0] != '}' ) {     
		ReadBaseFrameJoint( currentLine, destination[jointIndex] );
		++jointIndex;
		currentLine = strtok_s( NULL, "\n", &nextLine );
	}

	return nextLine;
}
/*
=============
Animation::ReadFrame

	Reads in the data for a frame
=============
*/
char* Animation::ReadFrame( char* startingPosition, unsigned frameIndex, FrameDataList& destination ) {
	FrameData&	currentFrame	= destination[frameIndex];
	char*		nextLine		= NULL;
	char*		currentLine		= strtok_s( startingPosition, "\n", &nextLine );
	char*		currentToken	= NULL;	
	char*		nextToken		= NULL;
	unsigned	dataIndex		= 0;

	while ( currentLine[0] != '}' ) {     
		currentToken = strtok_s( currentLine, "\t", &nextToken );		
		currentToken = strtok_s( currentToken, " ", &nextToken );
		while ( currentToken != NULL ) {
			currentFrame.data[dataIndex++] = ( float )std::atof( currentToken );
			currentToken = strtok_s( NULL, " ", &nextToken );
		}
		currentLine = strtok_s( NULL, "\n", &nextLine );
	}

	return nextLine;
}
/*
=============
Animation::BuildSkeletonFrames

	Builds the skeletons for all the frames.
=============
*/
void Animation::BuildSkeletonFrames( const JointInfoList& jointInfo, const FrameDataList& frameData, const BaseFrameJoints& baseFrameJoints ) {
	const BaseFrameJoint*	baseJoint			= NULL;
	Skeleton*				skeletonFrame		= NULL;
	float*					curFrameData		= NULL;
	unsigned				currentJointIndex	= 0;
	glm::vec3				rotatedPosition;

	for ( unsigned currentFrame = 0; currentFrame < m_numberOfFrames; ++currentFrame ) { //Each frame
		curFrameData	= frameData[currentFrame].data; 
		skeletonFrame	= &m_skeletonFrames[currentFrame];
		
        skeletonFrame->joints.resize( m_numberOfJoints );

		unsigned int jointIndex = 0;
		for ( JointInfoList::const_iterator currentJointInfo = jointInfo.begin();
			  currentJointInfo != jointInfo.end(); ++currentJointInfo ) { //Each joint

			m_jointHashIndex[StringUtils::Hash( currentJointInfo->name.c_str() )] = jointIndex;
			++jointIndex;

			baseJoint = &baseFrameJoints[currentJointIndex];
			unsigned dataOffset = 0;
			
			SkeletonJoint& currentSkeletonJoint = skeletonFrame->joints[currentJointIndex];
			
			currentSkeletonJoint.parentID		= currentJointInfo->parentID;
			currentSkeletonJoint.position		= baseJoint->position;
			currentSkeletonJoint.orientation	= baseJoint->orientation;

			if ( currentJointInfo->flags & TRANSLATE_X ) {
				currentSkeletonJoint.position.x = curFrameData[currentJointInfo->startIndex + dataOffset++];
			}
			if ( currentJointInfo->flags & TRANSLATE_Y ) {
				currentSkeletonJoint.position.y = curFrameData[currentJointInfo->startIndex + dataOffset++];
			}
			if ( currentJointInfo->flags & TRANSLATE_Z ) {
				currentSkeletonJoint.position.z = curFrameData[currentJointInfo->startIndex + dataOffset++];
			}
			if ( currentJointInfo->flags & QUATERNION_X ) {
				currentSkeletonJoint.orientation.x = curFrameData[currentJointInfo->startIndex + dataOffset++];
			}
			if ( currentJointInfo->flags & QUATERNION_Y ) {
				currentSkeletonJoint.orientation.y = curFrameData[currentJointInfo->startIndex + dataOffset++];
			}
			if ( currentJointInfo->flags & QUATERNION_Z ) {
				currentSkeletonJoint.orientation.z = curFrameData[currentJointInfo->startIndex + dataOffset++];
			}

			MathHelper::ComputeQuaternionW( currentSkeletonJoint.orientation );
			
			//Has a parent
			if ( currentSkeletonJoint.parentID > -1 ) {		
				SkeletonJoint& parent				= skeletonFrame->joints[currentSkeletonJoint.parentID];
				rotatedPosition						= parent.orientation * currentSkeletonJoint.position; //Rotate position				
				//Inherit parent transforms
				currentSkeletonJoint.position		= parent.position + rotatedPosition;
                currentSkeletonJoint.orientation	= parent.orientation * currentSkeletonJoint.orientation;
				
				//glm::normalize( currentSkeletonJoint.orientation );
			}
			
            ++currentJointIndex;			
		}		
		currentJointIndex	= 0;		
	}
}

int Animation::GetIndexOfJoint( unsigned long hash ) {
	std::unordered_map<unsigned long, unsigned int>::iterator iter = m_jointHashIndex.find( hash );
	if ( iter != m_jointHashIndex.end() ) {
		return ( int )m_jointHashIndex.find( hash )->second;
	} else {
		return -1;
	}
}

}