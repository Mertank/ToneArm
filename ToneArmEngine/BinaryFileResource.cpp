/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	BinaryFileResource

	Created by: Karl Merten
	Created on: 17/03/2014

========================
*/
#include "BinaryFileResource.h"
#include "VGSAssert.h"
#include "FileManager.h"
#include "Log.h"
#include "StringUtils.h"

#include <algorithm>

namespace vgs {
IMPLEMENT_RTTI( BinaryFileResource, Resource )
/*
========
BinaryFileResource::BinaryFileResource

	BinaryFileResource default constructor
========
*/
BinaryFileResource::BinaryFileResource( void ) :
	m_filePath( NULL ),
	m_fileLength( 0 ),
	m_fileKey( 0 ),
	m_filePosition( 0 )
{
}
/*
========
BinaryFileResource::~BinaryFileResource

	BinaryFileResource destructor
========
*/
BinaryFileResource::~BinaryFileResource( void ) {
	delete m_filePath;
}
/*
========
BinaryFileResource::Load

	Resource Override.
	Loads a binary file.
	Returns if it was successful.
========
*/
bool BinaryFileResource::Load( const char* path, const char key ) {
	if ( !FileManager::FileExists( path ) ) {
		char buffer[255];
		memset( buffer, 0, 255 );
		sprintf_s( buffer, 254, "File at path %s does not exist", path );
		Log::GetInstance()->WriteToLog( "BinaryFileResource", buffer );
		return false;
	}

	m_filePath = StringUtils::CopyCString( path );
	m_fileKey = key;

	std::ios_base::openmode stdMode = std::fstream::in | std::fstream::binary;

	std::ifstream file( path, stdMode );

	if ( file.good() ) {
		file.seekg( 0, file.end );
		m_fileLength = ( unsigned int )file.tellg();
		m_resourceSize = ( unsigned int )m_fileLength;
		return true;
	} else {
		return false;
	}

	file.close();
}
/*
========
BinaryFileResource::Write

	Writes bytes to the file at its current index.
========
*/
void BinaryFileResource::Write( const char* bytes, unsigned int byteCount ) {

	std::ofstream file( m_filePath, std::ios_base::out | std::ios_base::binary );
	file.seekp( m_filePosition, std::ios_base::beg );

	if ( m_fileKey ) {
		char* keyedBytes = new char[byteCount];
		memcpy( keyedBytes, bytes, byteCount );
		
		for ( unsigned int i = 0; i < byteCount; ++i ) {
			keyedBytes[i] ^= m_fileKey;
		}
		file.write( keyedBytes, byteCount );

		delete[] keyedBytes;
	} else {
		file.write( bytes, byteCount );
	}
	
	file.close();

	m_filePosition = file.tellp();
	m_fileLength = std::max<unsigned long long>( m_fileLength, m_filePosition ); //Update file size if necessary
}
/*
========
BinaryFileResource::Read

	Reads an amount of bytes from the file.
	YOU MUST DELETE THE POINTER WHEN DONE!
========
*/
char* BinaryFileResource::Read( unsigned long long count ) {
	std::ifstream file( m_filePath, std::ios_base::in | std::ios_base::binary );
	file.seekg( m_filePosition, std::ios_base::beg );

	char* buffer = new char[( unsigned int )count + 1];
	buffer[count] = '\0';
	file.read( buffer, count );

	if ( m_fileKey ) {
		for ( unsigned int i = 0; i < count; ++i ) {
			buffer[i] ^= m_fileKey;
		}
	}
	
	IncreaseIndex( count );

	file.close();

	return buffer;
}
/*
========
BinaryFileResource::ReadFile

	Reads the whole file.
	YOU MUST DELETE THE POINTER WHEN DONE!
========
*/
char* BinaryFileResource::ReadFile( void ) const {
	std::ifstream file( m_filePath, std::ios_base::in | std::ios_base::binary );

	char* buffer = new char[( unsigned int )m_fileLength + 1];
	buffer[m_fileLength] = '\0';
	file.read( buffer, m_fileLength );

	if ( m_fileKey ) {
		for ( unsigned int i = 0; i < m_fileLength; ++i ) {
			buffer[i] ^= m_fileKey;
		}
	}
	
	file.close();

	return buffer;
}
/*
========
BinaryFileResource::SeekToIndex

	Seeks to the index from the beginning of the file.
========
*/
void BinaryFileResource::SeekToIndex( unsigned long long newIndex ) {
	VGSAssert( newIndex <= m_fileLength, "Trying to seek beyond end of file" );
	m_filePosition = newIndex;
}
/*
========
BinaryFileResource::IncreaseIndex

	Increases the current index by the amount passed in.
========
*/
void BinaryFileResource::IncreaseIndex( unsigned int amount ) {
	VGSAssert( ( unsigned long long )( amount + m_filePosition ) <= m_fileLength, "Trying to seek beyond end of file" );
	m_filePosition += amount;
}
/*
========
BinaryFileResource::GetIndex

	Returns the index the file is currently read/writing at.
========
*/
unsigned long long BinaryFileResource::GetIndex( void ) const {
	return m_filePosition;
}
/*
========
BinaryFileResource::GetFileLength

	Gets the length of the file
========
*/
unsigned long long BinaryFileResource::GetFileLength( void ) const {
	return m_fileLength;
}

}