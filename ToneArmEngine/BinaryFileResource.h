/*
========================

Copyright (c) 2014 Vinyl Games Studio

	BinaryFileResource

		A class which represents a binary file on the disk.
		Has a variable amount of RAM storage to buffer the file.

		Created by: Karl Merten
		Created on: 17/03/2014

========================
*/

#ifndef __BINARYFILERESOURCE_H__
#define __BINARYFILERESOURCE_H__

#include <fstream>
#include "Resource.h"

namespace vgs {

namespace FileMode {
	enum Value {
		READ		= 0x01,
		WRITE		= 0x02
	};	
}

class BinaryFileResource : public Resource {
	friend class CachedResourceLoader;
	DECLARE_RTTI

public:
							~BinaryFileResource( void );
	
	char*					Read( unsigned long long byteCount );
	char*					ReadFile( void ) const;
	void					Write( const char* bytes, unsigned int count );

	void					SeekToIndex( unsigned long long index );
	void					IncreaseIndex( unsigned int index );
	unsigned long long		GetIndex( void ) const;
	unsigned long long		GetFileLength( void ) const;

	virtual bool			Load( const char* path, const char key = 0x00 );

protected:
							BinaryFileResource( void );

	char*					m_filePath;
	char					m_fileKey;
	unsigned long long		m_fileLength;
	unsigned long long		m_filePosition;
};

}

#endif //__BINARYFILERESOURCE_H__