/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	ModelFileResource

	Created by: Karl Merten
	Created on: 09/05/14

========================
*/
#include "ModelFileResource.h"
#include "CachedResourceLoader.h"
#include "Engine.h"

namespace vgs {
IMPLEMENT_RTTI( ModelFileResource, Resource )
/*
========
ModelFileResource::ModelFileResource

	ModelFileResource default constructor.
========
*/
ModelFileResource::ModelFileResource( void ) :
	m_fileHeader( NULL )
{
}
/*
========
ModelFileResource::ModelFileResource

	ModelFileResource destructor.
========
*/
ModelFileResource::~ModelFileResource( void ) {
	delete m_fileHeader;
}
/*
========
ModelFileResource::Load

	Loads a model file.
========
*/
bool ModelFileResource::Load( const char* path, char key ) {
	if (BinaryFileResource::Load(path, key))
	{
		m_fileHeader = new ModelFileHeader();
		ReadHeader();
		return true;
	}
	return false;
}
/*
========
ModelFileResource::ReadHeader

	Reads the header of a model file
========
*/
void ModelFileResource::ReadHeader( void ) {
	char* buffer = Read( 128 );

	//Read name
	unsigned int modelNameLen = strlen( buffer );	
	m_fileHeader->modelName = StringUtils::CopyCString( buffer );

	delete[] buffer;
	SeekToIndex( modelNameLen + 1 );
	
	//Read diffuse color
	buffer = Read( sizeof( float ) * 3 );
	char* color = buffer;

	StringUtils::ToFloat( color, m_fileHeader->diffuseColor[0] );
	color += 4;
	
	StringUtils::ToFloat( color, m_fileHeader->diffuseColor[1] );
	color += 4;

	StringUtils::ToFloat( color, m_fileHeader->diffuseColor[2] );

	delete[] buffer;

	//Read model start/size
	buffer = Read( sizeof ( unsigned long long ) * 2 + 2 );
	char* data = buffer;

	m_fileHeader->modelType = ( ModelType::Value )*data;
	++data;
	StringUtils::ToULong( data, m_fileHeader->modelStart );
	data += sizeof( unsigned long long );
	StringUtils::ToULong( data, m_fileHeader->modelSize );
	data += sizeof( unsigned long long );

	unsigned char numTextures = *data;
	delete[] buffer;

	m_fileHeader->textureCount = numTextures;
	if ( numTextures > 0 ) {
		m_fileHeader->textures = new TextureInfo[numTextures];
	}

	//Read Texture Info
	for ( int i = 0; i < numTextures; ++i ) {
		TextureInfo* currentTex = &m_fileHeader->textures[i];

		unsigned long long curIndex = GetIndex();
		buffer = Read( 255 );

		currentTex->textureName = StringUtils::CopyCString( buffer );
		SeekToIndex( curIndex + strlen( currentTex->textureName ) + 1 );

		delete[] buffer;

		buffer = Read( sizeof( unsigned long long ) * 2 + 1 );
		data = buffer;

		currentTex->textureType = *data;
		++data;

		StringUtils::ToULong( data, currentTex->textureStart );
		data += sizeof( unsigned long long );
		StringUtils::ToULong( data, currentTex->textureSize );
		data += sizeof( unsigned long long );

		delete[] buffer;
	}

	buffer = Read( 1 );
	data = buffer;
	char numAnims = *data;
	delete[] buffer;

	m_fileHeader->animationCount = numAnims;
	if ( numAnims > 0 ) {
		m_fileHeader->animations = new AnimationInfo[numAnims];
	}

	for ( int i = 0; i < numAnims; ++i ) {
		AnimationInfo* currrentAnim = &m_fileHeader->animations[i];
		
		unsigned long long curIndex = GetIndex();
		buffer = Read( 255 );

		currrentAnim->animationName = StringUtils::CopyCString( buffer );

		SeekToIndex( curIndex + strlen( currrentAnim->animationName ) + 1 );

		delete[] buffer;

		buffer = Read( sizeof( unsigned long long ) * 2 );
		data = buffer;

		StringUtils::ToULong( data, currrentAnim->animationStart );
		data += sizeof( unsigned long long );
		StringUtils::ToULong( data, currrentAnim->animationSize );

		delete[] buffer;
	}
}
/*
========
ModelFileResource::GetModelData

	Gets the data for the model.
========
*/
char* ModelFileResource::GetModelData( unsigned long long& length ) {
	length = m_fileHeader->modelSize;
	SeekToIndex( m_fileHeader->modelStart );

	char* data = NULL;
	data = Read( length );

	return data;
}
/*
========
ModelFileResource::GetTextureData

	Gets the data for the texture at that index.
========
*/
char* ModelFileResource::GetTextureData( char index, unsigned long long& length ) {
	if ( index < m_fileHeader->textureCount ) {
		length = m_fileHeader->textures[index].textureSize;
		SeekToIndex( m_fileHeader->textures[index].textureStart );

		char* data = NULL;
		data = Read( length );

		return data;
	} else {
		return NULL;
	}	
}
/*
========
ModelFileResource::GetAnimationData

	Gets the data for the animation at that index.
========
*/
char* ModelFileResource::GetAnimationData( char index, unsigned long long& length ) {
	if ( index < m_fileHeader->animationCount ) {
		length = m_fileHeader->animations[index].animationSize;
		SeekToIndex( m_fileHeader->animations[index].animationStart );

		char* data = NULL;
		data = Read( length );

		return data;
	} else {
		return NULL;
	}	
}
}