/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	LevelResource

	Created by: Karl Merten
	Created on: 26/05/2014

========================
*/
#include "LevelResource.h"
#include "VGSMacro.h"
#include "BinaryFileResource.h"
#include "CachedResourceLoader.h"
#include "Log.h"
#include "PathUtils.h"
#include "BoxShape.h"
#include "CylinderShape.h"

//#ifndef MERRY_SERVER
//#include "Engine.h"
//#else
//#include "../MerryServer/ServerEngine.h"
//#endif

namespace vgs {

IMPLEMENT_RTTI( LevelResource, Resource )
IMPLEMENT_PROTOTYPE(LevelResource)

LevelResource::LevelResource()
{
	REGISTER_PROTOTYPE(LevelResource, "vgsLevel")
}

LevelResource::~LevelResource( void ) {
	DELETE_STD_VECTOR_POINTER( ModelInfo, m_models )	
	DELETE_STD_VECTOR_POINTER( Shape, m_colliders )
}

bool LevelResource::Load( const char* path, const char key ) {
	
	if (BinaryFileResource::Load(path, key))
	{
		PathUtils::GetDirectory( std::string( path ), m_levelDirectory );

		char*			fileData	= ReadFile();
		unsigned int	dataOffset	= 0;

		unsigned int groupCount = 0;
		memcpy( &groupCount, fileData, sizeof( unsigned int ) );
		dataOffset += sizeof( unsigned int );

		for ( unsigned int i = 0; i < groupCount; ++i ) {
			unsigned int groupNameLength = 0;		
			while ( fileData[dataOffset + groupNameLength] != '\0' ) {
				++groupNameLength;
			}
			++groupNameLength;
			
			char* groupName = StringUtils::CopyCString( &fileData[dataOffset], groupNameLength );
			dataOffset += groupNameLength;

			unsigned int transformCount = 0;
			memcpy( &transformCount, &fileData[dataOffset], sizeof( unsigned int ) );
			dataOffset += sizeof( unsigned int );

			ModelInfo* m = new ModelInfo();
			m->modelName = groupName;

			delete[] groupName;

			for ( unsigned int j = 0; j < transformCount; ++j ) {
				Transform t;
				glm::vec3 position;
				glm::vec3 scale;
				glm::vec3 rotation;

				memcpy( &position[0], &fileData[dataOffset], sizeof( float ) * 3 );
				dataOffset += sizeof( float ) * 3;
				memcpy( &scale[0], &fileData[dataOffset], sizeof( float ) * 3 );
				dataOffset += sizeof( float ) * 3;			
				memcpy( &rotation[0], &fileData[dataOffset], sizeof( float ) * 3 );
				dataOffset += sizeof( float ) * 3;

				t.SetPosition( position );
				t.SetScale( scale );
				t.SetRotation( rotation );

				m->transforms.push_back( t );
			}

			m_models.push_back( m );
		}

		unsigned int colliderCount = 0;
		memcpy( &colliderCount, &fileData[dataOffset], sizeof( unsigned int ) );
		dataOffset += sizeof( unsigned int );

		for ( unsigned int i = 0; i < colliderCount; ++i ) {
			char colliderType = ColliderType::BOX_COLLIDER;
			memcpy( &colliderType, &fileData[dataOffset], 1 );
			++dataOffset;

			glm::vec3 position;
			glm::vec3 scale;
			glm::vec3 rotation;
			glm::vec3 dimensions;

			memcpy( &position[0], &fileData[dataOffset], sizeof( float ) * 3 );
			dataOffset += sizeof( float ) * 3;
			memcpy( &scale[0], &fileData[dataOffset], sizeof( float ) * 3 );
			dataOffset += sizeof( float ) * 3;			
			memcpy( &rotation[0], &fileData[dataOffset], sizeof( float ) * 3 );
			dataOffset += sizeof( float ) * 3;

			/*
			collider->transform.SetPosition( position );
			collider->transform.SetScale( scale );
			collider->transform.SetRotation( -rotation );
			*/
			if ( ( ColliderType::Value )colliderType == ColliderType::BOX_COLLIDER ) {
				memcpy( &dimensions[0], &fileData[dataOffset], sizeof( float ) * 3 );
				dataOffset += sizeof( float ) * 3;

				BoxShape* box = BoxShape::CreateBoxShape( position, dimensions );
				box->SetRotation( rotation );

				m_colliders.push_back( box );

			} else {
				memcpy( &dimensions[0], &fileData[dataOffset], sizeof( float ) * 2 );
				dataOffset += sizeof( float ) * 2;

				CylinderShape* cyl = CylinderShape::CreateCylinderShape( position, dimensions.x, dimensions.y );
				cyl->SetRotation( -rotation );

				m_colliders.push_back( cyl );
			}
		}

		unsigned int triggerCount = 0;
		memcpy( &triggerCount, &fileData[dataOffset], sizeof( unsigned int ) );
		dataOffset += sizeof( unsigned int );

		for ( unsigned int i = 0; i < triggerCount; ++i ) {
			char colliderType = ColliderType::BOX_COLLIDER;
			memcpy( &colliderType, &fileData[dataOffset], 1 );
			++dataOffset;

			glm::vec3 position;
			glm::vec3 scale;
			glm::vec3 rotation;
			glm::vec3 dimensions;

			memcpy( &position[0], &fileData[dataOffset], sizeof( float ) * 3 );
			dataOffset += sizeof( float ) * 3;
			memcpy( &scale[0], &fileData[dataOffset], sizeof( float ) * 3 );
			dataOffset += sizeof( float ) * 3;			
			memcpy( &rotation[0], &fileData[dataOffset], sizeof( float ) * 3 );
			dataOffset += sizeof( float ) * 3;

			/*
			collider->transform.SetPosition( position );
			collider->transform.SetScale( scale );
			collider->transform.SetRotation( -rotation );
			*/
			TriggerInfo* trigger = new TriggerInfo();

			if ( ( ColliderType::Value )colliderType == ColliderType::BOX_COLLIDER ) {
				memcpy( &dimensions[0], &fileData[dataOffset], sizeof( float ) * 3 );
				dataOffset += sizeof( float ) * 3;

				BoxShape* box = BoxShape::CreateBoxShape( position, dimensions );
				box->SetRotation( rotation );

				trigger->TriggerShape = box;

			} else {
				memcpy( &dimensions[0], &fileData[dataOffset], sizeof( float ) * 2 );
				dataOffset += sizeof( float ) * 2;

				CylinderShape* cyl = CylinderShape::CreateCylinderShape( position, dimensions.x, dimensions.y );
				cyl->SetRotation( -rotation );

				trigger->TriggerShape = cyl;
			}

			trigger->TriggerTag = std::string( &fileData[dataOffset] );
			m_triggers.push_back(trigger);

			dataOffset += ( trigger->TriggerTag.length() + 1 );
		}

		unsigned int spawnPointCount = 0;
		memcpy( &spawnPointCount, &fileData[dataOffset], sizeof( unsigned int ) );
		dataOffset += sizeof( unsigned int );

		m_spawnPoints.resize( spawnPointCount );

		for ( unsigned int i = 0; i < spawnPointCount; ++i ) {
			memcpy( &m_spawnPoints[i][0], &fileData[dataOffset], sizeof( float ) * 3 );
			dataOffset += sizeof( float ) * 3;
		}

		delete[] fileData;
		return true;
	}
	return false;
}

}