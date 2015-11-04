/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	PathUtils

	Created by: Karl Merten
	Created on: 17/03/2014

========================
*/
#include "PathUtils.h"

#include <iostream>
#include <dirent.h>

namespace vgs {
/*
========
PathUtils::GetExtension

	Returns a pointer to the start of the extension from a c-string.
	Path must be NULL-terminated.
	The length of the extension is stored in the second parameter.
========
*/
const char* PathUtils::GetExtension( const char* path, unsigned int& length ) {	
	unsigned int pathLength = strlen( path );
	int index = pathLength - 1;
	while ( index > 0 ) {
		if ( path[index] == '.' ) {
			length = ( pathLength - index ) - 1;
			return &( path[index + 1] );
		}
		--index;
	}

	length = 0;
	return NULL;
}
/*
========
PathUtils::GetExtension

	Stores the extension in the second string that is passed in.
========
*/
void PathUtils::GetExtension( std::string& src, std::string& dest ) {
	unsigned int extensionStart = src.find_last_of( '.' );
	if ( extensionStart != std::string::npos ) {
		dest = src.substr( extensionStart + 1, src.length() - extensionStart - 1 );
	}
}
/*
========
PathUtils::GetFileName

	Returns a pointer to the start of the filename from a c-string.
	Path must be NULL-terminated.
	The length of the filename is stored in the second parameter.
========
*/
const char* PathUtils::GetFileName( const char* path, unsigned int& length ) {
	unsigned int pathLength				= strlen( path );
	unsigned int indexOfLastDirDelim	= 0;
	unsigned int indexOfExtension		= 0;
	unsigned int currentIndex			= 0;
	
	while ( currentIndex < pathLength ) {
		if ( path[currentIndex] == '\\' ||
			 path[currentIndex] == '/' ) {
			indexOfLastDirDelim = currentIndex;
		} else if ( path[currentIndex] == '.' ) {
			indexOfExtension = currentIndex;
		}
		++currentIndex;
	}

	length = ( indexOfExtension - indexOfLastDirDelim ) - ( ( indexOfLastDirDelim == 0) ? 0 : 1 );
	return &( path[indexOfLastDirDelim + ( ( indexOfLastDirDelim == 0) ? 0 : 1 )] );
}
/*
========
PathUtils::GetFileName

	Stores the filename in the second string that is passed in.
========
*/
void PathUtils::GetFileName( std::string& src, std::string& dest ) {
	unsigned int pathLength				= src.length();	
	unsigned int indexOfLastDirDelim	= 0;
	unsigned int indexOfExtension		= 0;
	unsigned int currentIndex			= 0;
	
	while ( currentIndex < pathLength ) {
		if ( src[currentIndex] == '\\' ||
			 src[currentIndex] == '/' ) {
			indexOfLastDirDelim = currentIndex;
		} else if ( src[currentIndex] == '.' ) {
			indexOfExtension = currentIndex;
		}
		++currentIndex;
	}
	
	unsigned int length = ( indexOfExtension - indexOfLastDirDelim ) - ( ( indexOfLastDirDelim == 0) ? 0 : 1 );
	dest = src.substr( indexOfLastDirDelim + ( ( indexOfLastDirDelim == 0) ? 0 : 1 ), length );
}
/*
========
PathUtils::GetFileName

	Returns a pointer to the start of the directory from a c-string.
	Path must be NULL-terminated.
	The length of the directory is stored in the second parameter.
========
*/
const char* PathUtils::GetDirectory( const char* path, unsigned int& length ) {
	unsigned int pathLength				= strlen( path );	
	unsigned int indexOfLastDirDelim	= 0;
	unsigned int indexOfExtension		= 0;
	unsigned int currentIndex			= 0;
	
	while ( currentIndex < pathLength ) {
		if ( path[currentIndex] == '\\' ||
			 path[currentIndex] == '/' ) {
			indexOfLastDirDelim = currentIndex;
		}
		++currentIndex;
	}

	if ( indexOfLastDirDelim == 0 ) {
		length = 0;
		return NULL;
	} else {
		length = indexOfLastDirDelim;
		return path;
	}
}
/*
========
PathUtils::GetDirectory

	Stores the directory in the second string that is passed in.
========
*/
void PathUtils::GetDirectory( std::string& src, std::string& dest ) {
	unsigned int pathLength				= src.length();	
	unsigned int indexOfLastDirDelim	= 0;
	unsigned int indexOfExtension		= 0;
	unsigned int currentIndex			= 0;
	
	while ( currentIndex < pathLength ) {
		if ( src[currentIndex] == '\\' ||
			 src[currentIndex] == '/' ) {
			indexOfLastDirDelim = currentIndex;
		}
		++currentIndex;
	}

	if ( indexOfLastDirDelim != 0 ) {
		dest = src.substr( 0, indexOfLastDirDelim );
	}
}
/*
========
FileManager::ListDirectory

	Returns whether or not the file was deleted.
========
*/
StrVec PathUtils::ListDirectory( const char* dirname, int mode )
{
	DIR*			dir;
    struct dirent*	ent;
	StrVec			result;
                
    // Open directory stream
    dir = opendir(dirname);
    if (dir != NULL) 
	{
        // Print all files and directories within the directory
        while ((ent = readdir(dir)) != NULL) 
		{
            switch (ent->d_type)
			{
            case DT_REG:
				if (mode & ListMode::FILES)
				{
					result.push_back(ent->d_name);
				}
                break;

            case DT_DIR:
                if (mode & ListMode::DIRECTORIES)
				{
					result.push_back(ent->d_name);
				}
                break;

            case DT_LNK:
                if (mode & ListMode::LINKS)
				{
					result.push_back(ent->d_name);
				}
                break;

            default:
				break;
            }
        }

        closedir(dir);
    }
	else
	{
        // Could not open directory 
		std::cout << "Cannot open directory " << dirname << std::endl;
    }

	return result;
}
}