/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	FileManager

	Created by: Karl Merten
	Created on: 2/28/2014 5:45:28 PM

========================
*/
#include "FileManager.h"
#include <fstream>

namespace vgs {
/*
========
FileManager::FileExists

	Returns whether or not a file exists at the path.
========
*/
bool FileManager::FileExists( const char* path ) {
	std::ifstream f( path, std::ifstream::in | std::ifstream::binary );
	return f.good();
}
/*
========
FileManager::CreateFileAtPath

	Returns whether or not a file was created at the path.
========
*/
bool FileManager::CreateFileAtPath( const char* path ) {
	if ( FileExists( path ) ) {
		return false;
	} else {
		std::ofstream o( path );
		return o.good();
	}
}
/*
========
FileManager::DeleteFileAtPath

	Returns whether or not the file was deleted.
========
*/
bool FileManager::DeleteFileAtPath( const char* path ) {
	return ( remove( path ) == 0 );
}
}