/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MeshStructs

		Structs required to load MD5 Models.

		Created by: Karl Merten	
		Created on: 01/05/2014

========================
*/

#ifndef __MD5MODELCOMPONENTS_H__
#define __MD5MODELCOMPONENTS_H__

#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>

namespace vgs {
/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Joint

		Info for a single joint.

========================
*/
struct Joint {
    std::string     name;    
    int             parentID;    
    glm::vec3       position;
    glm::quat       orientation;

    Joint() :
        name( "NULL" ),
        parentID( -1 ),
        position( 0.0f, 0.0f, 0.0f ),
        orientation( 0.0f, 0.0f, 0.0f, 1.0f )
    {}
};
typedef std::vector<Joint> Joints;
/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Vertex

		Info about a single vertex.

========================
*/
struct Vertex {
    unsigned    vertexIndex;
    unsigned    startWeight;
    unsigned    countWeight;

    glm::vec2   textureCoordinate;
    glm::vec3   bindPosition;
    glm::vec3   bindNormal;
    glm::vec3   jointNormal;
    glm::vec3   gpuNormal;
    glm::vec4   boneWeights;
    glm::ivec4	boneIndicies;

    Vertex() :
        vertexIndex( 0 ),
        startWeight( 0 ),
        countWeight( 0 ),
        textureCoordinate( 0.0f ),
        bindPosition( 0.0f ),
        bindNormal( 0.0f ),
		jointNormal( 0.0f ),
        boneWeights( 0.0f ),
        boneIndicies( 0 )
    {}
};
typedef std::vector<Vertex> Verticies;
/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Triangle

		Info for a single triangle.

========================
*/
struct Triangle {
    unsigned int	triIndex;
    unsigned int	indices[3];

    Triangle( void ) :
        triIndex( 0 )
    {
        memset( &indices[0], 0, sizeof( unsigned int ) * 3 );
    }
};
typedef std::vector<Triangle> Triangles;
/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Weight

		Info for a single weight.

========================
*/
struct Weight {
    unsigned    weightIndex;
    unsigned    joint;
    float       bias;
    glm::vec3   position;

    Weight( void ) :
        weightIndex( 0 ),
        joint( 0 ),
        bias( 0.0f )
    {}

	bool operator< ( const Weight& w ) {
		return ( bias > w.bias );
	}
};
typedef std::vector<Weight> Weights;

} //namespace vgs

#endif //__MD5MODELCOMPONENTS_H__