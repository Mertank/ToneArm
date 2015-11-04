/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	InitializationFileResource

	Created by: Karl Merten
	Created on: 24/03/2014

========================
*/
#include "InitializationFileResource.h"
#include "BinaryFileResource.h"
#include "CachedResourceLoader.h"

//#ifndef MERRY_SERVER
//#include "Engine.h"
//#else
//#include "../MerryServer/ServerEngine.h"
//#endif

namespace vgs {
IMPLEMENT_RTTI( InitializationFileResource, Resource )
IMPLEMENT_PROTOTYPE(InitializationFileResource)
/*
========
InitializationFileResource::InitializationFileResource

	InitializationFileResource default constructor.
========
*/
InitializationFileResource::InitializationFileResource( void ) 
{
	REGISTER_PROTOTYPE(InitializationFileResource, "ini")
}
/*
========
InitializationFileResource::~InitializationFileResource

	InitializationFileResource destructor.
========
*/
InitializationFileResource::~InitializationFileResource( void ) {
	BEGIN_STD_VECTOR_ITERATOR( InitializationCategory*, m_categories )
		delete currentItem;		
	END_STD_VECTOR_ITERATOR
}
/*
========
InitializationFileResource::Load

	Resource override.
	Loads an ini file.
	Returns if it was successful.
========
*/
bool InitializationFileResource::Load( const char* path, const char key ) {
	if (BinaryFileResource::Load(path, key))
	{
//#ifndef MERRY_SERVER
//	std::shared_ptr<BinaryFileResource> ini = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>( EngineModuleType::RESOURCELOADER )->
//		LoadResource<BinaryFileResource>( path, key );
//#else
//	std::shared_ptr<BinaryFileResource> ini = ServerEngine::GetInstance()->GetResLoader()->LoadResource<BinaryFileResource>( path, key );
//#endif

	//if ( !ini.get() ) { return false; } //File failed to load

		char*			data			= ReadFile();
		unsigned int	fileLength		= ( unsigned int )GetFileLength();
		unsigned int	currentIndex	= 0;

		m_resourceSize = fileLength;

		while ( currentIndex < fileLength ) {
			currentIndex += ReadCategory( &data[currentIndex], ( fileLength - currentIndex ) );
		}

		delete[] data;
		return true;
	}
	return false;
}
/*
========
InitializationFileResource::ReadCategory

	Reads the category from the startingIndex.
	Returns how many bytes were read for the category.
========
*/
unsigned int InitializationFileResource::ReadCategory( const char* data, unsigned int bytesRemaining ) {
	unsigned int index = 0;
	//Skip comments
	if ( data[index] == '#' ) {
		while( data[index] != '\n' ) {
			++index;
		}
		return index + 1;
	}

	//Find the end of the title
	while( data[index] != ']' ) {
		++index;
	}

	InitializationCategory* iniCategory = InitializationCategory::CreateWithTitle( &data[1], ( index - 1 ) );
	while ( index < bytesRemaining ) {		
		//Skip white space
		while ( data[index] == '\n' ||
				data[index] == '\r' ||
				data[index] == ' '  ||
				data[index] == ']' ) {
			++index;
		}
		if ( index == bytesRemaining ) { break; }

		if ( data[index] == '[' ) { //Next category
			m_categories.push_back( iniCategory );
			return index;
		} else {
			index += ReadValue( &data[index], bytesRemaining - index, iniCategory );
		}
	}

	m_categories.push_back( iniCategory );
	return index;
}
/*
========
InitializationFileResource::ReadValue

	Reads the next key/value pair from the file.
========
*/
unsigned int InitializationFileResource::ReadValue( const char* start, unsigned int bytesRemaining, InitializationCategory* category ) {
	unsigned int index = 0;	
	if ( start[index] == '#' ) {
		while( start[index] != '\n' ) {
			++index;
		}
		return index + 1;
	}
	//Find length of the key
	char*		 key		= ( char* )&start[index];
	unsigned int keyLength	= 0;
	while ( start[index] != '=' ) {
		++index;
		++keyLength;
	}
	++index; //=
	//Find length of the value
	char*		 value			= ( char* )&start[index];
	unsigned int valueLength	= 0;
	while ( start[index] != '\r' && start[index] != '\n' && index < bytesRemaining ) {
		++index;
		++valueLength;
	}

	category->AddKeyValue( key, keyLength, value, valueLength );

	return index + 1;
}
/*
========
InitializationFileResource::GetCategory

	Returns the category with the name.
	NULL if the category doesn't exist.
========
*/
const InitializationCategory* const InitializationFileResource::GetCategory( const char* name ) const {
	for ( std::vector<InitializationCategory*>::const_iterator category = m_categories.begin();
		  category != m_categories.end(); ++category ) {
		if ( strcmp( ( *category )->GetTitle(), name ) == 0 ) {
			return *category;
		}
	}
	return NULL;
}
}