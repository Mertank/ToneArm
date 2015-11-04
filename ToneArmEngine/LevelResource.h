/*
========================

Copyright (c) 2014 Vinyl Games Studio

	LevelResource

		Loads a level as a resource.

		Created by: Karl Merten
		Created on: 26/05/2014

========================
*/

#ifndef __LEVELRESOURCE_H__
#define __LEVELRESOURCE_H__

#include "BinaryFileResource.h"
#include "ModelResource.h"
#include "Transform.h"

namespace vgs {

class Shape;

struct ModelInfo {
	std::string						modelName;
	std::vector<Transform>			transforms;
};

namespace ColliderType {
	enum Value : char {
		BOX_COLLIDER		= 0x00,
		CYLINDER_COLLIDER	= 0x01
	};
}

struct TriggerInfo {
	Shape*		TriggerShape;
	std::string	TriggerTag;

	TriggerInfo( void ) :
		TriggerShape( NULL )
	{}
};

class LevelResource : public BinaryFileResource {
	friend class CachedResourceLoader;
											DECLARE_RTTI
											DECLARE_PROTOTYPE(LevelResource)
public:
											~LevelResource( void );

	bool									Load( const char* path, const char key = 0x00 );
	const std::vector<ModelInfo*>& 			GetModels( void ) const { return m_models; }
	const std::vector<Shape*>&				GetColliders( void ) const { return m_colliders; }
	const std::vector<TriggerInfo*>&		GetTriggers( void ) const { return m_triggers; }
	const std::string&						GetModelDirectory( void ) const { return m_levelDirectory; }
	std::vector<glm::vec3>&					GetSpawnPoints( void ) { return m_spawnPoints; }

private:
											LevelResource();

	std::vector<ModelInfo*>					m_models;
	std::vector<Shape*>						m_colliders;
	std::vector<TriggerInfo*>				m_triggers;
	std::vector<glm::vec3>					m_spawnPoints;
	std::string								m_levelDirectory;
};

}

#endif //__LEVELRESOURCE_H__