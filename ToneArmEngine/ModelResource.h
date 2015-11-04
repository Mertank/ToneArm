/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ModelResource

		Logical representation of a model.

		Created by: Karl Merten
		Created on: 12/05/2014

========================
*/

#ifndef __MODELRESOURCE_H__
#define __MODELRESOURCE_H__

#include "ModelFileResource.h"
#include "BoxShape.h"
#include <glm\glm.hpp>
#include <memory>
#include <vector>

namespace vgs {

struct Joint;
class ModelFileResource;
struct ModelFileHeader;
class Animation;

class ModelResource : public ModelFileResource {

struct Face;

friend class CachedResourceLoader;

DECLARE_RTTI
DECLARE_PROTOTYPE(ModelResource)

public:
									~ModelResource( void );

	virtual bool					Load( const char* path, const char key = 0x00 );
	void							SetOwnerNodeID( unsigned int newId );
	
	BoxShape&						GetBoundingBox( void ) { return m_boundingBox; }

	bool							HasAnimations( void ) const { return m_animations.size() != 0; }
	const std::vector<Animation*>&	GetAnimations( void ) { return m_animations; }
private:
									ModelResource( void );

	void							CreateNewModel( void );
	void							CreateTextures(  );
	void							CreateAnimations(  );
	
	void							CreateAnimatedModel( char* data, unsigned long long dataLength );
	void							CreateStaticModel( char* data, unsigned long long dataLength );

	void							CreateAnimatedMesh( char* buffer, const Joint* joints, unsigned int jointCount, char** endPosition, const ModelFileHeader& fileHeader );
	void							CreateStaticMesh( const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<Face>& faces, const ModelFileHeader& fileHeader );
	
	void							SendBindPoseJoints( Joint* joints, unsigned int jointCount );

	void							DuplicateModel( unsigned int newID );

	char*							ReadJoints( char* buffer, Joint* jointsDest );
	void							ReadJoint( char* buffer, Joint* jointDest );

	void							ReadVec2( char* line, glm::vec2& dest ); 
	void							ReadVec3( char* line, glm::vec3& dest );
	void							ReadFace( char* line, std::vector<Face>& dest );

	unsigned int					m_ownerNode;

	BoxShape						m_boundingBox;

	std::vector<Animation*>			m_animations;
	glm::mat4*						m_inverseBoneMatricies;

struct Vertex {
	unsigned int position;
	unsigned int texture;
	unsigned int normal;

	Vertex( void ) :
		position( 0 ),
		normal( 0 ),
		texture( 0 )
	{}

	bool operator<( const Vertex& other ) const {
		if ( other.position != position ) {
			return ( position < other.position );
		} else if ( other.normal != normal ) {
			return ( normal < other.normal );
		} else if ( other.texture != texture ) {
			return ( texture < other.texture );
		}
		return false;
	}
}; //struct Vertex

struct Face {
	Vertex verticies[3];

	Face( void ) {
		memset( verticies, 0, sizeof( Vertex ) * 3 );
	}

	Face( const Face& other ) {
		memcpy( &verticies[0], other.verticies, sizeof( Vertex ) * 3 );
	}
}; //struct Face

}; //class ModelResource

} //namespace vgs

#endif //__MODELRESOURCE_H__