/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ModelFileResource

		A class representing the vgsModel File.

		Created by: Karl Merten
		Created on: 09/05/14

========================
*/

#ifndef __MODELFILERESOURCE_H__
#define __MODELFILERESOURCE_H__

#include "BinaryFileResource.h"

#include <memory>
#include <glm\glm.hpp>

namespace vgs {

struct AnimationInfo {
	char*				animationName;
	unsigned long long	animationStart;
	unsigned long long	animationSize;

	~AnimationInfo( void ) {
		delete[] animationName;
	}
};

struct TextureInfo {
	char*				textureName;
	char				textureType;	
	unsigned long long	textureStart;
	unsigned long long	textureSize;

	~TextureInfo( void ) {
		delete[] textureName;
	}
};

namespace ModelType {
	enum Value {
		STATIC = 0,
		ANIMATED = 1
	};
}

struct ModelFileHeader {
	char*				modelName;
	glm::vec3			diffuseColor;

	ModelType::Value	modelType;
	unsigned long long	modelStart;
	unsigned long long	modelSize;

	char				textureCount;
	TextureInfo*		textures;

	char				animationCount;
	AnimationInfo*		animations;

	ModelFileHeader( void ) :
		modelName( NULL ),
		modelStart( 0 ),
		modelSize( 0 ),
		textureCount( 0 ),
		textures( NULL ),
		animationCount( 0 ),
		animations( NULL ),
		modelType( ModelType::STATIC )
	{}

	~ModelFileHeader( void ) {
		if ( textures ) {
			delete[] textures;
		}

		if ( animations ) {
			delete[] animations;
		}

		delete[] modelName;
	}
};

class ModelFileResource : public BinaryFileResource {
	DECLARE_RTTI
public:
											ModelFileResource( void );
											~ModelFileResource( void );

	virtual bool							Load( const char* path, const char key = 0x00);
	char*									GetModelData( unsigned long long& length );
	char*									GetTextureData( char index, unsigned long long& length );
	char*									GetAnimationData( char index, unsigned long long& length );
	const ModelFileHeader&					GetHeader( void ) { return *m_fileHeader; }
private:
	void									ReadHeader( void );

	ModelFileHeader*						m_fileHeader;
};

}

#endif //__MODELFILERESOURCE_H__