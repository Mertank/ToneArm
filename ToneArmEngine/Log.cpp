/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Log

	Created by: Karl Merten
	Created on: 3/17/2014 5:39:10 PM

========================
*/
#include "Log.h"

#include "FileManager.h"
#include "SystemInfo.h"
#include <cstring>

namespace vgs {
INITIALIZE_SINGLETON( Log )
/*
========
Log::Log

	Log default constructor
========
*/
Log::Log( void ) :
	streamOpen( false )
{	
	logPath = new char[20];
	memcpy( logPath, "./TonearmEngine.log", 19 );
	logPath[19] = '\0';
}
/*
========
Log::~Log

	Log destructor
========
*/
Log::~Log( void ) {
	delete[] logPath;
	SystemInfo::DestroyInstance();
}
/*
========
Log::Initialize

	Initializes the log.
========
*/
void Log::Initialize( const char* path, bool writeSysInfo ) {
	DeleteLogFile();
	if ( path ) {
		SetLogPath( path );				
	}
	if ( writeSysInfo ) {
		SystemInfo::GetInstance()->WriteToLog();		
	}
}
/*
========
Log::SetLogPath

	Sets the new log path
========
*/
void Log::SetLogPath( const char* path ) {
	delete[] logPath;
	
	unsigned int pathLen = strlen( path );
	logPath = new char[pathLen + 1];
	memcpy( logPath, path, pathLen );
	logPath[pathLen] = '\0';
}
/*
========
Log::DeleteLogFile

	Delets the current log file.
========
*/
void Log::DeleteLogFile( void ) {
	if ( streamOpen ) {
		CloseStream();
	}
	FileManager::DeleteFileAtPath( logPath );
}
/*
========
Log::WriteToLog

	Deletes the current log file.
========
*/
bool Log::WriteToLog( const char* tag, const char* msg, bool leaveStreamOpen ) {
	if ( !streamOpen ) {
		logStream.open( logPath, std::ios::out | std::ios::app );
		streamOpen = true;
		if ( !logStream.good() ) {
			return false;
		}
	}

	std::string logStr;

	if ( tag ) {
		logStr += tag;
		logStr += ": ";
	}

	logStr += msg;
	logStr += "\n";

	logStream.write( logStr.c_str(), logStr.length() );

#ifdef TONEARM_DEBUG
	printf( logStr.c_str() );
#endif

	if ( !leaveStreamOpen ) {
		CloseStream();
	}

	return true;
}
/*
========
Log::CloseStream

	Closes the stream.
	You should not be calling this manually!
========
*/
void Log::CloseStream( void ) {
	streamOpen = false;
	logStream.close();
}

}