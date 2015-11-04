/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	MeshHelper

	Created by: Karl Merten
	Created on: 12/05/2014

========================
*/
#include "MeshHelper.h"
#include "StringUtils.h"
#include <gl\glew.h>

namespace vgs {
/*
========
MeshHelper::MakeLine

	Makes a line mesh.
	You must delete the buffers when done.
========
*/
void MeshHelper::MakeLine( float length, float** vertDataDest, unsigned short** indexDataDest, unsigned int& vertexBufferSize, unsigned int& indexCount ) {
	MakeLine( length, glm::vec3( 0.0f, 0.0f, 1.0f ), vertDataDest, indexDataDest, vertexBufferSize, indexCount );
}
/*
========
MeshHelper::MakeLine

	Makes a line mesh in a certain direction.
	You must delete the buffers when done.
========
*/
void MeshHelper::MakeLine( float length, const glm::vec3& direction, float** vertDataDest, unsigned short** indexDataDest, unsigned int& vertexBufferSize, unsigned int& indexCount ) {
	unsigned short vertexSize = 3;
	indexCount = 2;
	vertexBufferSize = 2;

	*vertDataDest = new float[6];
	*indexDataDest = new unsigned short[2];

	( *indexDataDest )[0] = 0;
	( *indexDataDest )[1] = 1;

	memset( *vertDataDest, 0, sizeof( float ) * 6 );

	( *vertDataDest )[3] = direction.x * length;
	( *vertDataDest )[4] = direction.y * length;
	( *vertDataDest )[5] = direction.z * length;
}
/*
========
MeshHelper::MakeRectMesh

	Fills the destination buffers with vertex and index data.
	Supported components are Normals and Positions.
	You must delete the buffers when done.
========
*/
void MeshHelper::MakeRectMesh( float width, float height, float depth, VertexComponents::Value components, float** vertDataDest, unsigned short** indexDataDest, unsigned int& vertexBufferSize, unsigned int& indexCount ) {
	unsigned short vertexSize = 3;
	unsigned short duplicatesRequired = 0; //For normals each vertex needs to exist 3 times with a different normal
	
	indexCount = 36;

	if ( components & VertexComponents::NORMAL ) {
		vertexSize += 3;
		duplicatesRequired = 2;
	}

	vertexBufferSize = 8 * ( duplicatesRequired + 1 );

	*indexDataDest = new unsigned short[indexCount];
	*vertDataDest = new float[vertexBufferSize * vertexSize];

	glm::vec3 corners[8];

	float halfWidth		= width * 0.5f;
	float halfHeight	= height * 0.5f;
	float halfDepth		= depth * 0.5f;

	//Top Quad
	corners[0] = glm::vec3( -halfWidth,  halfHeight,  halfDepth );
	corners[1] = glm::vec3(  halfWidth,  halfHeight,  halfDepth );
	corners[2] = glm::vec3(  halfWidth,  halfHeight, -halfDepth );
	corners[3] = glm::vec3( -halfWidth,  halfHeight, -halfDepth );

	//Bottom Quad
	corners[4] = glm::vec3( -halfWidth, -halfHeight,  halfDepth );
	corners[5] = glm::vec3(  halfWidth, -halfHeight,  halfDepth );
	corners[6] = glm::vec3(  halfWidth, -halfHeight, -halfDepth );
	corners[7] = glm::vec3( -halfWidth, -halfHeight, -halfDepth );
	
	if ( components & VertexComponents::NORMAL ) {
		glm::vec3 normals[6];

		normals[0] = glm::vec3(  0.0f,  1.0f,  0.0f ); //Up
		normals[1] = glm::vec3(  0.0f, -1.0f,  0.0f ); //Down
		normals[2] = glm::vec3( -1.0f,  0.0f,  0.0f ); //Left
		normals[3] = glm::vec3(  1.0f,  0.0f,  0.0f ); //Right
		normals[4] = glm::vec3(  0.0f,  0.0f,  1.0f ); //Forward
		normals[5] = glm::vec3(  0.0f,  0.0f, -1.0f ); //Back

		//Top normals
		memcpy( &( ( *vertDataDest )[( vertexSize * 0 ) + 3] ), &normals[0][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 1 ) + 3] ), &normals[0][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 2 ) + 3] ), &normals[0][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 3 ) + 3] ), &normals[0][0], sizeof( float ) * 3 );

		//Bottom normals
		memcpy( &( ( *vertDataDest )[( vertexSize * 4 ) + 3] ), &normals[1][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 5 ) + 3] ), &normals[1][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 6 ) + 3] ), &normals[1][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 7 ) + 3] ), &normals[1][0], sizeof( float ) * 3 );

		//Left normals
		memcpy( &( ( *vertDataDest )[( vertexSize * 8  ) + 3] ), &normals[2][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 12 ) + 3] ), &normals[2][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 11 ) + 3] ), &normals[2][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 15 ) + 3] ), &normals[2][0], sizeof( float ) * 3 );

		//Right normals
		memcpy( &( ( *vertDataDest )[( vertexSize * 9  ) + 3] ), &normals[3][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 10 ) + 3] ), &normals[3][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 13 ) + 3] ), &normals[3][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 14 ) + 3] ), &normals[3][0], sizeof( float ) * 3 );

		//Front normals
		memcpy( &( ( *vertDataDest )[( vertexSize * 16 ) + 3] ), &normals[4][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 17 ) + 3] ), &normals[4][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 20 ) + 3] ), &normals[4][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 21 ) + 3] ), &normals[4][0], sizeof( float ) * 3 );

		//Back normals
		memcpy( &( ( *vertDataDest )[( vertexSize * 18 ) + 3] ), &normals[5][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 19 ) + 3] ), &normals[5][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 22 ) + 3] ), &normals[5][0], sizeof( float ) * 3 );
		memcpy( &( ( *vertDataDest )[( vertexSize * 23 ) + 3] ), &normals[5][0], sizeof( float ) * 3 );

		//Top Quad
		( *indexDataDest )[0] = 0;
		( *indexDataDest )[1] = 1;
		( *indexDataDest )[2] = 2;
		( *indexDataDest )[3] = 0;
		( *indexDataDest )[4] = 3;
		( *indexDataDest )[5] = 2;

		//Bottom Quad
		( *indexDataDest )[6]  = 4;
		( *indexDataDest )[7]  = 6;
		( *indexDataDest )[8]  = 5;
		( *indexDataDest )[9]  = 4;
		( *indexDataDest )[10] = 7;
		( *indexDataDest )[11] = 6;

		//Left Quad
		( *indexDataDest )[12] = 8;
		( *indexDataDest )[13] = 12;
		( *indexDataDest )[14] = 11;
		( *indexDataDest )[15] = 12;
		( *indexDataDest )[16] = 15;
		( *indexDataDest )[17] = 11;

		//Right Quad
		( *indexDataDest )[18] = 9;
		( *indexDataDest )[19] = 14;
		( *indexDataDest )[20] = 13;
		( *indexDataDest )[21] = 9;
		( *indexDataDest )[22] = 10;
		( *indexDataDest )[23] = 14;

		//Front Quad
		( *indexDataDest )[24] = 19;
		( *indexDataDest )[25] = 22;
		( *indexDataDest )[26] = 18;
		( *indexDataDest )[27] = 19;
		( *indexDataDest )[28] = 23;
		( *indexDataDest )[29] = 22;

		//Back Quad
		( *indexDataDest )[30] = 16;
		( *indexDataDest )[31] = 17;
		( *indexDataDest )[32] = 21;
		( *indexDataDest )[33] = 16;
		( *indexDataDest )[34] = 21;
		( *indexDataDest )[35] = 20;

		//Set Vertex Data.
		//Make 3 copies.
		for ( int i = 0; i < 8; i++ ) {
			memcpy( &( ( *vertDataDest )[i			* vertexSize] ), &corners[i][0], sizeof( float ) * 3 );
			memcpy( &( ( *vertDataDest )[( i + 8 )  * vertexSize] ), &corners[i][0], sizeof( float ) * 3 );
			memcpy( &( ( *vertDataDest )[( i + 16 ) * vertexSize] ), &corners[i][0], sizeof( float ) * 3 );
		}

	} else {
		//Set Index Data
		( *indexDataDest )[0]  = ( *indexDataDest )[3]  = ( *indexDataDest )[23] = ( *indexDataDest )[25]							= 0;
		( *indexDataDest )[1]  = ( *indexDataDest )[20] = ( *indexDataDest )[22] = ( *indexDataDest )[35]							= 1;
		( *indexDataDest )[2]  = ( *indexDataDest )[4]  = ( *indexDataDest )[7]  = ( *indexDataDest )[32] = ( *indexDataDest )[34]	= 2;		
		( *indexDataDest )[5]  = ( *indexDataDest )[6]  = ( *indexDataDest )[9]  = ( *indexDataDest )[26] = ( *indexDataDest )[29]	= 3;

		( *indexDataDest )[17] = ( *indexDataDest )[18] = ( *indexDataDest )[21] = ( *indexDataDest )[24] = ( *indexDataDest )[28]	= 4;
		( *indexDataDest )[14] = ( *indexDataDest )[16] = ( *indexDataDest )[19] = ( *indexDataDest )[30] = ( *indexDataDest )[33]	= 5;
		( *indexDataDest )[8]  = ( *indexDataDest )[10] = ( *indexDataDest )[13] = ( *indexDataDest )[31]							= 6;
		( *indexDataDest )[11] = ( *indexDataDest )[12] = ( *indexDataDest )[15] = ( *indexDataDest )[27]							= 7;

		//Set Vertex Data
		for ( int i = 0; i < 8; i++ ) {
			memcpy( &( ( *vertDataDest )[i * vertexSize] ), &corners[i][0], sizeof( float ) * 3 );
		}
	}
}
/*
========
MeshHelper::MakeCylinderMesh

	Makes a cylindrical mesh.
	You must delete the buffers when done.
========
*/
void MeshHelper::MakeCylinderMesh( float radius, float height, unsigned int sections, VertexComponents::Value components, float** vertexDataDest, unsigned short** indexDataDest, unsigned int& vertexCount, unsigned int& indexCount ) {
	unsigned int vertexSize = ( components & VertexComponents::NORMAL ) ? 6 : 3;

	if ( vertexSize == 3 ) {
		vertexCount = ( sections * 2 ) + 2; //2: Top and bottom center point
	} else {
		vertexCount = ( sections * 4 ) + 2;
	}

	indexCount = ( sections * 4 ) * 3;

	*indexDataDest	= new unsigned short[indexCount];
	*vertexDataDest = new float[vertexCount * vertexSize];

	memset( *vertexDataDest, 0, sizeof( float ) * vertexCount * vertexSize );
	memset( *indexDataDest, 0, sizeof( unsigned short ) * indexCount );

	unsigned int bottomCircleOffset = ( unsigned int )( sections + 1 );
	const float  halfHeight = ( height * 0.5f );

	glm::vec3 position( 0.0f );
	glm::vec3 normal( 0.0f );

	position.y = halfHeight;
	normal.y = 1.0f;

	//Top
	memcpy( &( ( *vertexDataDest )[0] ), &position[0], sizeof( float ) * 3 );
	if ( vertexSize == 6 ) { //Normals 
		memcpy( &( ( *vertexDataDest[3] ) ), &normal[0], sizeof( float ) * 3 );
	}

	position.y = -position.y;
	normal.y = -normal.y;

	//Bottom
	memcpy( &( ( *vertexDataDest )[bottomCircleOffset * vertexSize] ), &position[0], sizeof( float ) * 3 );
	if ( vertexSize == 6 ) { //Normals 
		memcpy( &( ( *vertexDataDest )[bottomCircleOffset * vertexSize + 3] ), &normal[0], sizeof( float ) * 3 );
	}

	const float sectionsAngles = 360.0f / sections;
	float currentAngle = 0.0f;

	if ( vertexSize == 3 ) {
		glm::vec3 direction( 0.0f );

		for ( unsigned int i = 0; i < sections; ++i ) {
			direction.x = cos( glm::radians( currentAngle ) );
			direction.z = sin( glm::radians( currentAngle ) );

			direction	= glm::normalize( direction );
			position	= ( direction * radius );
			position.y	= halfHeight;
			memcpy( &( ( *vertexDataDest )[3 + ( i * vertexSize )] ), &position[0], sizeof( float ) * 3 );

			position.y = -position.y;
			memcpy( &( ( *vertexDataDest )[3 + ( bottomCircleOffset * vertexSize ) + ( i * vertexSize )] ), &position[0], sizeof( float ) * 3 );

			currentAngle += sectionsAngles;
		}

		unsigned int bottomCircleIndexOffset = sections * 3;
		for ( unsigned int i = 0; i < sections - 1; ++i ) {
			( *indexDataDest )[i * 3]		= 0;
			( *indexDataDest )[i * 3 + 1]	= i + 2;
			( *indexDataDest )[i * 3 + 2]	= i + 1;

			( *indexDataDest )[i * 3 + bottomCircleIndexOffset]		= bottomCircleOffset;
			( *indexDataDest )[i * 3 + bottomCircleIndexOffset + 1]	= bottomCircleOffset + i + 1;
			( *indexDataDest )[i * 3 + bottomCircleIndexOffset + 2]	= bottomCircleOffset + i + 2;
		}

		( *indexDataDest )[( sections - 1 ) * 3]		= 0;
		( *indexDataDest )[( sections - 1 ) * 3 + 1]	= 1;
		( *indexDataDest )[( sections - 1 ) * 3 + 2]	= sections;

		( *indexDataDest )[( sections - 1 ) * 3 + bottomCircleIndexOffset]		= bottomCircleOffset;
		( *indexDataDest )[( sections - 1 ) * 3 + bottomCircleIndexOffset + 1]	= bottomCircleOffset + sections;
		( *indexDataDest )[( sections - 1 ) * 3 + bottomCircleIndexOffset + 2]	= bottomCircleOffset + 1;

		unsigned int planesOffset = ( sections * 2 ) * 3;
		for ( unsigned int i = 0; i < sections - 1; ++i ) {
			( *indexDataDest )[i * 6 + planesOffset]		= i + 1;
			( *indexDataDest )[i * 6 + planesOffset + 1]	= i + 2;
			( *indexDataDest )[i * 6 + planesOffset + 2]	= sections + i + 2;

			( *indexDataDest )[i * 6 + planesOffset + 3]	= i + 2;
			( *indexDataDest )[i * 6 + planesOffset + 4]	= sections + i + 3;
			( *indexDataDest )[i * 6 + planesOffset + 5]	= sections + i + 2;
		}

		( *indexDataDest )[( sections - 1 ) * 6 + planesOffset]		= sections;
		( *indexDataDest )[( sections - 1 ) * 6 + planesOffset + 1]	= 1;
		( *indexDataDest )[( sections - 1 ) * 6 + planesOffset + 2]	= ( sections * 2 ) + 1;

		( *indexDataDest )[( sections - 1 ) * 6 + planesOffset + 3]	= 1;
		( *indexDataDest )[( sections - 1 ) * 6 + planesOffset + 4]	= sections + 2;
		( *indexDataDest )[( sections - 1 ) * 6 + planesOffset + 5]	= ( sections * 2 ) + 1;

	} else {
		for ( unsigned int i = 0; i < sections; ++i ) {

		}
	}
}
/*
========
MeshHelper::MakeCircleMesh

	Makes a circle mesh.
	You must delete the buffers when done.
========
*/
void MeshHelper::MakeCircleMesh( float radius, unsigned int sections, VertexComponents::Value components, float** vertexDataDest, unsigned short** indexDataDest, unsigned int& vertexCount, unsigned int& indexCount ) {
	vertexCount = sections + 1; //Center point
	indexCount = sections * 3;

	*indexDataDest	= new unsigned short[indexCount];
	*vertexDataDest = new float[vertexCount * 3];

	memset( *vertexDataDest, 0, sizeof( float ) * vertexCount * 3 );
	memset( *indexDataDest, 0, sizeof( unsigned short ) * indexCount );

	glm::vec3 position( 0.0f );
	glm::vec3 normal( 0.0f, 1.0f, 0.0f );

	//Top
	memcpy( &( ( *vertexDataDest )[0] ), &position[0], sizeof( float ) * 3 );

	const float sectionsAngles = 360.0f / sections;
	float currentAngle = 0.0f;

	glm::vec3 direction( 0.0f );

	for ( unsigned int i = 0; i < sections; ++i ) {
		direction.x = cos( glm::radians( currentAngle ) );
		direction.z = sin( glm::radians( currentAngle ) );

		direction	= glm::normalize( direction );
		position	= ( direction * radius );
		memcpy( &( ( *vertexDataDest )[3 + ( i * 3 )] ), &position[0], sizeof( float ) * 3 );

		currentAngle += sectionsAngles;
	}

	for ( unsigned int i = 0; i < sections - 1; ++i ) {
		( *indexDataDest )[i * 3]		= 0;
		( *indexDataDest )[i * 3 + 1]	= i + 2;
		( *indexDataDest )[i * 3 + 2]	= i + 1;
	}

	( *indexDataDest )[( sections - 1 ) * 3]		= 0;
	( *indexDataDest )[( sections - 1 ) * 3 + 1]	= 1;
	( *indexDataDest )[( sections - 1 ) * 3 + 2]	= sections;
}
/*
========
MeshHelper::MakeMeshFromBuffer

	Creates a mesh from a buffer
========
*/
void MeshHelper::MakeMeshFromBuffer( const Joint* joints, unsigned int jointCount, char* buffer, float** vertDataDest, unsigned int** indexDataDest, unsigned int& vertexBufferSize, unsigned int& indexCount, VertexComponents::Value& components, char** endPosition, glm::vec3& minVertex, glm::vec3& maxVertex ) {
	char* nextLine      = NULL;
	char* currentLine   = strtok_s( buffer, "\n", &nextLine );
	char* junk          = NULL;
	char* currentToken  = NULL;
	char* nextToken     = NULL;
	
	unsigned int	vertexCount		= 0;
	Vertex*			nextVertex		= NULL;
	Vertex*			verticies		= NULL;
	
	unsigned int	triangleCount	= 0;
	Triangle*		nextTriangle	= NULL;
	Triangle*		triangles		= NULL;
	
	unsigned int	weightCount		= 0;
	Weight*			nextWeight		= NULL;
	Weight*			weights			= NULL;
	
	components = ( VertexComponents::Value )( VertexComponents::POSITION | VertexComponents::NORMAL | VertexComponents::TEXTURE | VertexComponents::WEIGHTS | VertexComponents::MATRIX_INDEX );

	while ( currentLine[0] != '}' ) {
		currentToken    = strtok_s( currentLine, "\t", &nextToken );
		currentToken    = strtok_s( currentToken, " ", &nextToken );
		
		if ( StringUtils::IsEqual( currentToken, "shader" ) ) {
			//shaderName = strtok_s( NULL, "\"", &nextToken );
			//shaderName.append( ".tga" );
		} else if ( StringUtils::IsEqual( currentToken, "numverts" ) ) {
			vertexCount = std::atoi( strtok_s( NULL, " ", &nextToken ) );
			verticies	= new Vertex[vertexCount];
			nextVertex	= verticies;
		} else if ( StringUtils::IsEqual( currentToken, "vert" ) ) {
			if ( vertexCount > 0 ) {
				ReadVertex( nextToken, nextVertex );				
				++nextVertex;
			}
		} else if ( StringUtils::IsEqual( currentToken, "numtris" ) ) {
			triangleCount	= std::atoi( strtok_s( NULL, " ", &nextToken ) );
			triangles		= new Triangle[triangleCount];
			nextTriangle	 = triangles;
		} else if ( StringUtils::IsEqual( currentToken, "tri" ) ) {
			if ( triangleCount > 0 ) {
				ReadTriangle( nextToken, triangles );
			} 
		} else if ( StringUtils::IsEqual( currentToken, "numweights" ) ) {
			weightCount = std::atoi( strtok_s( NULL, " ", &nextToken ) );
			weights		= new Weight[weightCount];
			nextWeight	= weights;
		} else if ( StringUtils::IsEqual( currentToken, "weight" ) ) {
			if ( weightCount > 0 ) {
				ReadWeight( nextToken, weights );
			}
		}

		currentLine = strtok_s( NULL, "\n", &nextLine );
	}

	GenerateMeshData( vertDataDest, indexDataDest, vertexBufferSize, indexCount,
					  joints, jointCount, verticies, vertexCount, weights, weightCount,
					  triangles, triangleCount, minVertex, maxVertex );

	delete[] weights;
	delete[] triangles;
	delete[] verticies;

	*endPosition = nextLine;
}
/*
========
MeshHelper::GenerateMeshData

	Generates a mesh from the data loaded from the file.
========
*/
void MeshHelper::GenerateMeshData( float** vertDataDest, unsigned int** indexDataDest, unsigned int& vertexBufferSize, unsigned int& indexCount, 
							 const Joint* joints, unsigned int jointCount, Vertex* verticies, unsigned int vertexCount, Weight* weights, unsigned int weightCount,
							 Triangle* triangles, unsigned int triangleCount, glm::vec3& minVertex, glm::vec3& maxVertex ) {
	
	//TODO: Textureless meshes.
	*vertDataDest = new float[vertexCount * 16];				//3 for position + 3 for normal + 2 for texture + 4 for weights + 4 for matrix indicies
	*indexDataDest = new unsigned int[triangleCount * 3];	//3 indicies per tri
	
	vertexBufferSize = vertexCount;
	indexCount = triangleCount * 3;

	ComputeVerticies( vertDataDest, verticies, vertexCount, joints, jointCount, weights, weightCount, minVertex, maxVertex );
	ComputeIndicies( indexDataDest, triangles, triangleCount );
	ComputeNormals( vertDataDest, verticies, vertexCount, weights, weightCount, triangles, triangleCount, joints );
}
/*
========
MeshHelper::GenerateMeshData

	Generates a mesh from the data loaded from the file.
========
*/
void MeshHelper::ReadWeight( char* buffer, Weight* weights ) {
	char* nextToken = NULL;

	int weightIndex = std::atoi( strtok_s( buffer, " ", &nextToken ) );					//Read the weight index

	Weight& currentWeight = weights[weightIndex];
	currentWeight.weightIndex = weightIndex;

	currentWeight.joint = std::atoi( strtok_s( NULL, " ", &nextToken ) );						//Read joint
	currentWeight.bias  = ( float )std::atof( strtok_s( NULL, " ", &nextToken ) );				//Read bias
	StringUtils::ToVec3( strtok_s( NULL, "()", &nextToken ), " ", currentWeight.position );		//Read position
}
/*
========
MeshHelper::GenerateMeshData

	Generates a mesh from the data loaded from the file.
========
*/
void MeshHelper::ReadTriangle( char* buffer, Triangle* triangles ) {
	char* nextToken = NULL;

	int triIndex = std::atoi( strtok_s( buffer, " ", &nextToken ) );	

	Triangle& currentTri = triangles[triIndex];
	currentTri.triIndex = triIndex;
	
	currentTri.indices[0] = std::atoi( strtok_s( NULL, " ", &nextToken ) );
	currentTri.indices[1] = std::atoi( strtok_s( NULL, " ", &nextToken ) );
	currentTri.indices[2] = std::atoi( strtok_s( NULL, " ", &nextToken ) );
}
/*
========
MeshHelper::GenerateMeshData

	Generates a mesh from the data loaded from the file.
========
*/
void MeshHelper::ComputeVerticies( float** vertexDataDest, Vertex* verticies, unsigned int vertexCount, const Joint* joints, unsigned int jointCount, Weight* weights, unsigned int weightCount, glm::vec3& minVertex, glm::vec3& maxVertex ) {	
	std::vector<Weight> weightsToSort;
	weightsToSort.reserve( 10 );

	for ( unsigned int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex ) {
		Vertex* currentVertex = &verticies[vertexIndex];

		glm::vec3 vertexPosition    = glm::vec3( 0.0f );

		unsigned weightStart = currentVertex->startWeight;
		unsigned weightCount = currentVertex->countWeight;
		
		//Calculate position using all weights
		for ( unsigned i = 0; i < weightCount; i++ ) {            
			Weight&			currentWeight	= weights[weightStart + i];
			const Joint&	currentJoint	= joints[currentWeight.joint];            
			
			weightsToSort.push_back( currentWeight );

			glm::vec3 weightedVertex = currentJoint.orientation * currentWeight.position;
			vertexPosition += ( ( currentJoint.position + weightedVertex ) * currentWeight.bias );
		}

		float scaleAmount = 1.0f;
		if ( weightsToSort.size() > 4 ) {
			std::sort( weightsToSort.begin(), weightsToSort.end() );
			float currentBiasTotal = 0.0f;
			for ( int i = 0; i < 4; ++i ) {
				currentBiasTotal += weightsToSort[i].bias;
			}
			scaleAmount = 1.0f / currentBiasTotal; //How much the weights should scale to = 1.
		}	
		//Figure out which verticies are most important since they're out of order by default
		for ( unsigned i = 0; i < weightsToSort.size() && i < 4; ++i ) {
			currentVertex->boneWeights[i]	= weightsToSort[i].bias * scaleAmount;
			currentVertex->boneIndicies[i]	= weightsToSort[i].joint;
		}

		memcpy( &( *vertexDataDest )[vertexIndex * 16]     , &vertexPosition[0]						, sizeof( float ) * 3 ); //Vertex Position
		memcpy( &( *vertexDataDest )[vertexIndex * 16 + 6] , &currentVertex->textureCoordinate[0]	, sizeof( float ) * 2 ); //Vertex TexCoord
        memcpy( &( *vertexDataDest )[vertexIndex * 16 + 8] , &currentVertex->boneWeights[0]			, sizeof( float ) * 4 ); //Vertex Weights
        memcpy( &( *vertexDataDest )[vertexIndex * 16 + 12], &currentVertex->boneIndicies[0]		, sizeof( float ) * 4 ); //Matrix Indicies

		currentVertex->bindPosition = vertexPosition;

		if ( currentVertex->bindPosition.x < minVertex.x ) {
			minVertex.x = currentVertex->bindPosition.x;
		} else if ( currentVertex->bindPosition.x > maxVertex.x ) {
			maxVertex.x = currentVertex->bindPosition.x;
		}

		if ( currentVertex->bindPosition.y < minVertex.y ) {
			minVertex.y = currentVertex->bindPosition.y;
		} else if ( currentVertex->bindPosition.y > maxVertex.y ) {
			maxVertex.y = currentVertex->bindPosition.y;
		}

		if ( currentVertex->bindPosition.z < minVertex.z ) {
			minVertex.z = currentVertex->bindPosition.z;
		} else if ( currentVertex->bindPosition.z > maxVertex.z ) {
			maxVertex.z = currentVertex->bindPosition.z;
		}
		
		weightsToSort.clear();
	}
}
/*
========
MeshHelper::ComputeIndicies

	Computes the indicies for the mesh
========
*/
void MeshHelper::ComputeIndicies( unsigned int** indexDataDest, Triangle* triangles, unsigned int triangleCount ) {
	for ( unsigned int i = 0; i < triangleCount; ++i ) {
		Triangle* currentTri = &triangles[i];
		memcpy( &( *indexDataDest )[i * 3], &currentTri->indices[0], sizeof( GLuint ) * 3 );
	}
}
/*
========
MeshHelper::ComputeNormals

	Computes the normals for the mesh.
========
*/
void MeshHelper::ComputeNormals( float** vertexDataDest, Vertex* verticies, unsigned int vertexCount, Weight* weights, unsigned int weightCount, Triangle* triangles, unsigned int triangleCount, const Joint* joints ) {
	Triangle* currentTriangle = NULL;
	for ( unsigned int i = 0; i < triangleCount; ++i ) {
		currentTriangle = &triangles[i];
		glm::vec3 faceNormal = glm::cross( verticies[currentTriangle->indices[1]].bindPosition - verticies[currentTriangle->indices[0]].bindPosition,
										   verticies[currentTriangle->indices[2]].bindPosition - verticies[currentTriangle->indices[0]].bindPosition );

		verticies[currentTriangle->indices[0]].bindNormal += faceNormal;
		verticies[currentTriangle->indices[1]].bindNormal += faceNormal;
		verticies[currentTriangle->indices[2]].bindNormal += faceNormal;
	}

	int vertexIndex = 0;
	for ( unsigned int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex ) {
		Vertex* currentVertex = &verticies[vertexIndex];

		currentVertex->bindNormal = MathHelper::Normalize( currentVertex->bindNormal );
		memcpy( &( *vertexDataDest )[vertexIndex * 16 + 3], &currentVertex->bindNormal[0], sizeof( float ) * 3 );

		int weightCount = currentVertex->countWeight;
		int weightStart = currentVertex->startWeight;

		//Calculate normal to joint local space
		//This is apparently faster for calculating the normal later
		for ( int i = 0; i < weightCount; i++ ) {            
			Weight& currentWeight		= weights[weightStart + i];
			const Joint&  currentJoint  = joints[currentWeight.joint];

		   currentVertex->jointNormal += ( ( currentVertex->bindNormal * currentJoint.orientation ) * currentWeight.bias );
		}

		vertexIndex++;
	}
}
/*
========
MeshHelper::ReadVertex

	Reads a vertex from the buffer
========
*/
void MeshHelper::ReadVertex( char* buffer, Vertex* vert ) {
	char* nextToken = NULL;

	int vertexIndex = std::atoi( strtok_s( buffer, " ", &nextToken ) );		//Read the vertex index

	vert->vertexIndex = vertexIndex;

	StringUtils::ToVec2( strtok_s( NULL, "()", &nextToken ), " ", vert->textureCoordinate );	//Read texture coordinates
	vert->startWeight = std::atoi( strtok_s( NULL, " ", &nextToken ) );							//Read start weight
	vert->countWeight = std::atoi( strtok_s( NULL, " ", &nextToken ) );							//Read weight count
}

}