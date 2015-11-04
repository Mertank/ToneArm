/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Log

		Singleton class for writing to a log file.
		
		Created by: Karl Merten
		Created on: 3/17/2014 5:39:10 PM

========================
*/
#ifndef __LOG_H__
#define __LOG_H__

#include <cstdlib>
#include <fstream>
#include "VGSMacro.h"

namespace vgs {

class Log {
					CREATE_SINGLETON( Log )
public:
					~Log( void );

	void			Initialize( const char* path = NULL, bool writeSysInfo = true );
	void			DeleteLogFile( void );
	bool			WriteToLog( const char* tag, const char* msg, bool leaveStreamOpen = false );
	void			CloseStream( void );
private:					
	void			SetLogPath( const char* path );

	char*			logPath;
	bool			streamOpen;
	std::ofstream	logStream;
};

}

#endif //__LOG_H__