/*
========================

Copyright (c) 2014 Vinyl Games Studio

	InitializationFileResource

		Reads a .ini file at the specified path.
		Contains a list of categories with key/value pairs in them.

		Created by: Karl Merten
		Created on: 24/03/2014

========================
*/

#ifndef __INITIALIZATIONFILERESOURCE_H__
#define __INITIALIZATIONFILERESOURCE_H__

#include <vector>

#include "InitializationCategory.h"
#include "BinaryFileResource.h"

namespace vgs {

class InitializationFileResource : public BinaryFileResource {
	friend class CachedResourceLoader;
	DECLARE_RTTI
	DECLARE_PROTOTYPE(InitializationFileResource)

public:
												~InitializationFileResource( void );

	virtual bool								Load( const char* path, const char key = 0x00 );

	const InitializationCategory* const			GetCategory( const char* name ) const;
private:
												InitializationFileResource( void );
	unsigned int								ReadCategory( const char* startBracket, unsigned int bytesRemaining );
	unsigned int								ReadValue( const char* start, unsigned int bytesRemaining, InitializationCategory* category );

	std::vector<InitializationCategory*>		m_categories;
};

}

#endif //__INITIALIZATIONFILERESOURCE_H__