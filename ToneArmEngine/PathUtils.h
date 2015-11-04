/*
========================

Copyright (c) 2014 Vinyl Games Studio

	PathUtils

		Does operations on strings related to paths.

		Created by: Karl Merten
		Created on: 17/03/2014

========================
*/
#ifndef __PATHUTILS_H__
#define __PATHUTILS_H__

#include <string>
#include <vector>

namespace vgs {

typedef std::vector<std::string> StrVec;

namespace ListMode
{
	enum ListMode
	{
		FILES		= 0x0001,
		DIRECTORIES	= 0x0010,
		LINKS		= 0x0100
	};
}

class PathUtils {
public:
	static const char*	GetExtension( const char* path, unsigned int& length );
	static void			GetExtension( std::string& src, std::string& dest );

	static const char*	GetFileName( const char* path, unsigned int& length );
	static void			GetFileName( std::string& src, std::string& dest );

	static const char*	GetDirectory( const char* path, unsigned int& length );
	static void			GetDirectory( std::string& src, std::string& dest );

	static StrVec		ListDirectory( const char* dirname, int mode = ListMode::FILES & ListMode::DIRECTORIES ); 

};

}

#endif //__PATHUTILS_H__