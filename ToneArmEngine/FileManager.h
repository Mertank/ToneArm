/*
========================

Copyright (c) 2014 Vinyl Games Studio

	FileManager

		Does basic operations on BinaryFiles.

		Created by: Karl Merten
		Created on: 2/28/2014 5:45:28 PM

========================
*/
#ifndef __FILEMANAGER_H__
#define __FILEMANAGER_H__

#include "BinaryFileResource.h"

#include <vector>
#include <string>

namespace vgs {

class FileManager {
public:
	static bool				CreateFileAtPath( const char* path );
	static bool				DeleteFileAtPath( const char* path );	
	static bool				FileExists( const char* path );
};

}

#endif //__FILEMANAGER_H__