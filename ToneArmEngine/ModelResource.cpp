/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	ModelResource

	Created by: Karl Merten
	Created on: 12/05/2014

========================
*/
#include "ModelResource.h"

#include "CachedResourceLoader.h"
#include "MeshStructs.h"
#include "Log.h"
#include "MeshHelper.h"
#include "RenderModule.h"
#include "Engine.h"
#include "FileManager.h"
#include "RenderMessages.h"

#include "Animation.h"

#include "stb_image_aug.h" //Image loading

#include <map>

#include "ModelNode.h"
#include "Engine.h"
#include "Scene.h"

namespace vgs {
IMPLEMENT_RTTI( ModelResource, Resource )
IMPLEMENT_PROTOTYPE(ModelResource)
/*
========
ModelResource::ModelResource

	ModelResource default constructor.
========
*/
ModelResource::ModelResource( void ) :
	m_ownerNode( 0 ),
	m_inverseBoneMatricies( NULL )
{
	REGISTER_PROTOTYPE(ModelResource, "vgsModel")
}
/*
========
ModelResource::ModelResource

	ModelResource destructor.
========
*/
ModelResource::~ModelResource( void ) {
	DELETE_STD_VECTOR_POINTER( Animation, m_animations );
	delete[] m_inverseBoneMatricies;
}
/*
========
ModelResource::Load

	Load override from resource.
	Stores the path for later.
========
*/
bool ModelResource::Load( const char* path, const char key ) {	
	if ( ModelFileResource::Load(path, key) ) {
		return true;
	} else {
		char buffer[255];
		memset( buffer, 0, 255 );
		sprintf_s( buffer, "Model at path %s does not exist", path );
		Log::GetInstance()->WriteToLog( "ModelResource", buffer );

		return false;
	}
}
/*
========
ModelResource::Load

	Load override from resource.
	Stores the path for later.
========
*/
void ModelResource::SetOwnerNodeID( unsigned int nodeID ) {
	if ( m_ownerNode == 0 ) {
		m_ownerNode = nodeID;
		CreateNewModel();
	} else {
		DuplicateModel( nodeID );
	}
}
/*
========
ModelResource::DuplicateModel

	Creates a duplicate of an existing model.
========
*/
void ModelResource::DuplicateModel( unsigned int newID ) {
	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	
	DuplicateModelMessage* msg = renderer->GetRenderMessage<DuplicateModelMessage>();
	msg->Init( newID, m_ownerNode );
	renderer->SendRenderMessage( msg );
}
/*
========
ModelResource::Load

	Load override from resource.
	Stores the path for later.
========
*/
void ModelResource::CreateNewModel( void ) {
	//std::shared_ptr<ModelFileResource> modelFile = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>( EngineModuleType::RESOURCELOADER )
	//	->LoadResource<ModelFileResource>( m_filePath, m_fileKey );

	//if ( modelFile.get() ) {
		unsigned long long dataLength = 0;
		char* modelData = GetModelData( dataLength );

		if ( GetHeader().modelType == ModelType::STATIC ) {
			CreateStaticModel( modelData, dataLength );
		} else {
			CreateAnimatedModel( modelData, dataLength );
		}

		if ( GetHeader().textureCount > 0 ) {
			CreateTextures( );
		}

		if ( GetHeader().animationCount > 0 ) {
			CreateAnimations( );
		}

		delete[] modelData;
	//}
}
/*
========
ModelResource::CreateTextures

	Creates the textures for the model
========
*/
void ModelResource::CreateTextures( ) {
	TextureInfo* texInfo;
	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );

	for ( int i = 0; i < GetHeader().textureCount; ++i ) {
		texInfo = &GetHeader().textures[i];

		int imageHeight;
		int imageWidth;
		int components;

		unsigned long long length;

		char* fileData = GetTextureData( i, length );
		unsigned char* imageData = stbi_load_from_memory( ( unsigned char* )fileData, ( int )length, &imageWidth, &imageHeight, &components, 0 );

		delete[] fileData;

		if ( texInfo->textureType == TextureType::DIFFUSE_W_ALPHA ) {
			int i = 0;
		}

		CreateTextureMessage* msg = renderer->GetRenderMessage<CreateTextureMessage>();
		msg->Init( m_ownerNode, ( char* )imageData, components, imageWidth, imageHeight, ( TextureType::Value )texInfo->textureType );
		renderer->SendRenderMessage( msg );
	}
}
/*
========
ModelResource::CreateAnimations

	Creates the animations for the model
========
*/
void ModelResource::CreateAnimations( ) {
	AnimationInfo* info;
	for ( int i = 0; i < GetHeader().animationCount; ++i ) {
		info = &GetHeader().animations[i];

		unsigned long long length;
		char* buffer = GetAnimationData( i, length );

		Animation* anim = Animation::CreateAnimationFromBuffer( buffer, ( unsigned long )length );
		anim->SetAnimationName( info->animationName );
		m_animations.push_back( anim );

		delete[] buffer;
	}
}
/*
========
ModelResource::CreateStaticModel

	Creates a model from a file with an OBJ
========
*/
void ModelResource::CreateStaticModel( char* data, unsigned long long length ) {
	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	
	CreateModelMessage* msg = renderer->GetRenderMessage<CreateModelMessage>();
	msg->Init( m_ownerNode, 1 );
	renderer->SendRenderMessage( msg );

	char* nextLineToken	= NULL;
	char* currentLine   = strtok_s( data, "\n", &nextLineToken );
	
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	glm::vec3 vector3;
	glm::vec2 vector2;

	std::vector<Face> faces;

	glm::vec3 minVertex;
	glm::vec3 maxVertex;

	while ( currentLine != NULL ) {
		if ( currentLine[0] != '#' ) { //Not comment
			if ( StringUtils::IsEqual( currentLine, "v ", 2 ) ) {
				ReadVec3( currentLine, vector3 );
				if ( vector3.x < minVertex.x ) {
					minVertex.x = vector3.x;
				} else if ( vector3.x > maxVertex.x ) {
					maxVertex.x = vector3.x;
				}
				if ( vector3.y < minVertex.y ) {
					minVertex.y = vector3.y;
				} else if ( vector3.y > maxVertex.y ) {
					maxVertex.y = vector3.y;
				}
				if ( vector3.z < minVertex.z ) {
					minVertex.z = vector3.z;
				} else if ( vector3.z > maxVertex.z ) {
					maxVertex.z = vector3.z;
				}
				positions.push_back( vector3 );
			} else if ( StringUtils::IsEqual( currentLine, "vt", 2 ) &&
						GetHeader().textureCount > 0 ) {
				ReadVec2( currentLine, vector2 );
				texCoords.push_back( vector2 );
			} else if ( StringUtils::IsEqual( currentLine, "vn", 2 ) ) {
				ReadVec3( currentLine, vector3 );
				if ( vector3.x != 0.0f || vector3.y != 0.0f || vector3.z != 0.0f ) {
					vector3 = glm::normalize( vector3 ); //Might not be normalized
				}
				normals.push_back( vector3 );
			} else if ( StringUtils::IsEqual( currentLine, "f", 1 ) ) {
				ReadFace( currentLine, faces );
			}
		}		

		currentLine = strtok_s( NULL, "\n", &nextLineToken );		
	}

	m_boundingBox.SetDimensions( maxVertex - minVertex );
	m_boundingBox.SetPosition( ( maxVertex + minVertex ) * 0.5f );

	CreateStaticMesh( positions, normals, texCoords, faces, GetHeader() );
}
/*
========
ModelResource::ReadVec3

	Reads a vec3 from an OBJ buffer
========
*/
void ModelResource::ReadVec3( char* line, glm::vec3& dest ) {
	char* nextParam     = NULL;
	char* currentParam  = strtok_s( line, " ", &nextParam );

	currentParam = strtok_s( NULL, " ", &nextParam );
	dest.x = ( float )atof( currentParam );
	
	currentParam = strtok_s( NULL, " ", &nextParam );
	dest.y = ( float )atof( currentParam );

	currentParam = strtok_s( NULL, " ", &nextParam );
	dest.z = ( float )atof( currentParam );
}
/*
========
ModelResource::ReadVec2

	Reads a vec2 from an OBJ buffer
========
*/
void ModelResource::ReadVec2( char* line, glm::vec2& dest ) {
	char* nextParam     = NULL;
	char* currentParam  = strtok_s( line, " ", &nextParam );

	currentParam = strtok_s( NULL, " ", &nextParam );
	dest.x = ( float )atof( currentParam );
	
	currentParam = strtok_s( NULL, " ", &nextParam );
	dest.y = ( float )atof( currentParam );
}
/*
========
ModelResource::ReadFace

	Reads a face from an OBJ buffer
========
*/
void ModelResource::ReadFace( char* line, std::vector<Face>& faces ) {
	char* nextParam     = NULL;
	char* currentParam  = strtok_s( line, " ", &nextParam );
	currentParam = strtok_s( NULL, " ", &nextParam );

	std::vector<Vertex> verticies;
	std::vector<char*> tokens;
	//Read verticies
	while ( currentParam != NULL ) {
		Vertex v;

		StringUtils::Tokenize( currentParam, '/', tokens );

		v.position = atoi( tokens[0] );		
		//Position + texture
		if ( tokens.size() == 2 ) {
			v.texture = atoi( tokens[1] );
		} else {
			if ( tokens[1] ) {
				v.texture = std::atoi( tokens[1] );
			}

			if ( tokens[2] ) {
				v.normal = std::atoi( tokens[2] );
			}
		}

		//Clean up tokens
		BEGIN_STD_VECTOR_ITERATOR( char*, tokens )
			delete[] currentItem;
		END_STD_VECTOR_ITERATOR
		tokens.clear();
		
		currentParam = strtok_s( NULL, " ", &nextParam );
		verticies.push_back( v );
	}
	
	//Triangulate: if required
	for ( unsigned int i = 1; i < verticies.size() - 1; ++i ) {
		Face f;
		f.verticies[0] = verticies[0];
		f.verticies[1] = verticies[i];
		f.verticies[2] = verticies[i + 1];
		faces.push_back( f );
	}
}
/*
========
ModelResource::CreateStaticMesh

	Creates a mesh from vectors with data.
========
*/
void ModelResource::CreateStaticMesh( const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords, const std::vector<Face>& faces, const ModelFileHeader& fileHeader ) {
	VertexComponents::Value vertexComps = VertexComponents::POSITION;
	if ( normals.size() > 0 ) {
		vertexComps = ( VertexComponents::Value )( vertexComps | VertexComponents::NORMAL );
	}
	if ( texCoords.size() > 0 ) {
		vertexComps = ( VertexComponents::Value )( vertexComps | VertexComponents::TEXTURE );
	}

	unsigned int indexCount = faces.size() * 3;
	unsigned int* indicies = new unsigned int[indexCount];

	std::map<Vertex, unsigned int>vertexMap;
	unsigned int nextIndex = 0;
	unsigned int nextIndexNum = 0;

	for ( std::vector<Face>::const_iterator iter = faces.begin();
		  iter != faces.end(); ++iter ) {
		for ( int i = 0; i < 3; ++i ) {
			std::map<Vertex, unsigned int>::iterator location = vertexMap.find( iter->verticies[i] );
			if ( location == vertexMap.end() ) {
				vertexMap[iter->verticies[i]] = nextIndexNum;
				indicies[nextIndex] = nextIndexNum;
				++nextIndexNum;
			} else {
				indicies[nextIndex] = location->second;
			}
			++nextIndex;
		}
	}

	unsigned int vertexSize = 3;
	if ( vertexComps & VertexComponents::NORMAL ) {
		vertexSize += 3;
	}
	if ( vertexComps & VertexComponents::TEXTURE ) {
		vertexSize += 2;
	}
	
	unsigned int vertexCount = vertexMap.size();
	float* vertexData = new float[vertexCount * vertexSize];

	for ( std::map<Vertex, unsigned int>::const_iterator iter = vertexMap.begin();
		  iter != vertexMap.end(); ++iter ) {
		const Vertex& v = iter->first;

		unsigned int offset = 0;
		if ( v.position != 0 ) {
			memcpy( &vertexData[iter->second * vertexSize], &positions[v.position - 1][0], sizeof( float ) * 3 );			
			offset += 3;
		}
		if ( v.normal != 0 && normals.size() > 0 ) {
			memcpy( &vertexData[iter->second * vertexSize + offset], &normals[v.normal - 1][0], sizeof( float ) * 3 );			
			offset += 3;
		}
		if ( v.texture != 0 && texCoords.size() > 0 ) {
			memcpy( &vertexData[iter->second * vertexSize + offset], &texCoords[v.texture - 1][0], sizeof( float ) * 2 );			
		}
	}

	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	CreateMeshMessage* msg = renderer->GetRenderMessage<CreateMeshMessage>();
	msg->Init( m_ownerNode, vertexData, vertexCount, indicies, indexCount, GL_UNSIGNED_INT, vertexComps, fileHeader.diffuseColor, m_boundingBox.GetPosition(), m_boundingBox.GetBoundingDimensions() );
	renderer->SendRenderMessage( msg );
}
/*
========
ModelResource::CreateAnimatedModel

	Creates a model from an MD5MESH file
========
*/
void ModelResource::CreateAnimatedModel( char* data, unsigned long long dataLength ) {
	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );

	char* nextLineToken	= NULL;
	char* currentLine   = strtok_s( data, "\n", &nextLineToken );

	Joint*				joints		= NULL;
	Joint*				nextJoint	= NULL;
	unsigned int		jointCount	= 0;
	unsigned int		meshCount	= 0;

	while ( currentLine != NULL ) {
		char* nextParam     = NULL;
		char* currentParam  = strtok_s( currentLine, " ", &nextParam );

		if ( StringUtils::IsEqual( currentParam, "MD5Version" ) ) { //Read the version
			nextParam[2] = '\0'; //Truncate to 2 chars
			if ( !StringUtils::IsEqual( nextParam, "10" ) ) {
				Log::GetInstance()->WriteToLog( "RenderableModel", "Only MD5Version 10 is supported\n" );
				return;                
			}
		} else if ( StringUtils::IsEqual( currentParam, "numJoints" ) ) { //Read numjoints
			jointCount = std::atoi( nextParam );
			joints = new Joint[jointCount];
			nextJoint = joints;
		} else if ( StringUtils::IsEqual( currentParam, "numMeshes" ) ) { //Read nummeshes
			meshCount = std::atoi( nextParam );

			CreateModelMessage* msg = renderer->GetRenderMessage<CreateModelMessage>();
			msg->Init( m_ownerNode, meshCount );
			renderer->SendRenderMessage( msg );

		} else if ( StringUtils::IsEqual( currentParam, "joints" ) ) { //Read joints
			if ( jointCount == 0 ) {
				Log::GetInstance()->WriteToLog( "RenderableModel", "numJoints was not specified\n" );
				return;    
			}
			nextLineToken = ReadJoints( nextLineToken, joints );
		} else if ( StringUtils::IsEqual( currentParam, "mesh" ) ) { //Read a mesh
			CreateAnimatedMesh( nextLineToken, joints, jointCount, &nextLineToken, GetHeader() );
		}

		currentLine = strtok_s( NULL, "\n", &nextLineToken );
	}

	SendBindPoseJoints( joints, jointCount );

	delete[] joints;
}
/*
========
Animation::SendAnimationBoneMatricies

	Sends the default inverse bone matricies.
========
*/
void ModelResource::SendBindPoseJoints( Joint* joints, unsigned int jointCount ) {
	unsigned matrixIndex = 0;
	m_inverseBoneMatricies = new glm::mat4[jointCount];
	
	Joint* currentJoint = NULL;
    for ( unsigned int joint = 0; joint < jointCount; ++joint, ++matrixIndex ) {

		currentJoint = &joints[joint];

        glm::mat4 translation   = glm::translate( glm::mat4( 1.0 ), currentJoint->position );
        glm::mat4 rotation      = glm::toMat4( currentJoint->orientation );

        glm::mat4 finalMatrix   = translation * rotation;

        m_inverseBoneMatricies[matrixIndex] = glm::inverse( finalMatrix );
    }

	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	InverseBoneMatriciesMessage* msg = renderer->GetRenderMessage<InverseBoneMatriciesMessage>();
	msg->Init( m_ownerNode, m_inverseBoneMatricies, jointCount );
	renderer->SendRenderMessage( msg );
}
/*
========
ModelResource::CreateAnimatedMesh

	Creates an animated mesh from an md5mesh buffer
========
*/
void ModelResource::CreateAnimatedMesh( char* buffer, const Joint* joints, unsigned int jointCount, char** endPosition, const ModelFileHeader& fileHeader ) {
	float* vertexData = NULL;
	unsigned int* indexData = NULL;
	unsigned int indexCount = 0;
	unsigned int vertexCount = 0;
	VertexComponents::Value vertexComponents;

	glm::vec3 minVertex, maxVertex;

	MeshHelper::MakeMeshFromBuffer( joints, jointCount, buffer, &vertexData, &indexData, vertexCount, indexCount, vertexComponents, endPosition, minVertex, maxVertex );
	
	m_boundingBox.SetDimensions( maxVertex - minVertex );
	m_boundingBox.SetPosition( ( maxVertex + minVertex ) * 0.5f );

	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );

	CreateMeshMessage* msg = renderer->GetRenderMessage<CreateMeshMessage>();
	msg->Init( m_ownerNode, vertexData, vertexCount, indexData, indexCount, GL_UNSIGNED_INT, vertexComponents, fileHeader.diffuseColor, m_boundingBox.GetPosition(), m_boundingBox.GetBoundingDimensions() );
	renderer->SendRenderMessage( msg );
}
/*
=============
ModelResource::ReadJoints

	Reads all joints from a char buffer.
	Stores them in joints member variable.
	Returns a pointer to where the buffer should continue reading.
=============
*/
char* ModelResource::ReadJoints( char* buffer, Joint* jointsDest ) {
	char* nextLine      = NULL;
	char* currentLine   = strtok_s( buffer, "\n", &nextLine );
	char* nextToken     = NULL;
	
	Joint* nextJoint	= jointsDest;

	while ( currentLine[0] != '}' ) {     
		ReadJoint( currentLine, jointsDest );

		++jointsDest;
		currentLine = strtok_s( NULL, "\n", &nextLine );
	}

	return nextLine;
}
/*
=============
ModelResource::ReadJoints

	Reads a joint from a char buffer.
=============
*/
void ModelResource::ReadJoint( char* buffer, Joint* jointsDest ) {
	char* nextToken = NULL;

	jointsDest->name = strtok_s( buffer, "\t\"", &nextToken );									//Read the joint name	
	jointsDest->parentID = std::atoi( strtok_s( NULL, "\t ", &nextToken ) );					//Joint's parent id
	StringUtils::ToVec3( strtok_s( NULL, "()", &nextToken ), " ", jointsDest->position );	    //The joint's position	
	++nextToken;																				//Skip the space between the position and orientation		
	StringUtils::ToQuat( strtok_s( NULL, "()", &nextToken ), " ", jointsDest->orientation );	//The joint's orientation
}

}