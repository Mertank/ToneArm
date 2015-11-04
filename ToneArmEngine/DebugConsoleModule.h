/*
========================

Copyright (c) 2014 Vinyl Games Studio

	DebugConsoleModule

		Debug console.
		Open/close with ~.

		Created by: Karl Merten
		Created on: 14/05/2014

========================
*/

#ifndef __DEBUGCONSOLEMODULE_H__
#define __DEBUGCONSOLEMODULE_H__

#include "EngineModule.h"
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>
#include "Log.h"
#include "StringUtils.h"

namespace vgs {
class DebugGUILayer;

class DebugCommandHandler {	
public:
	virtual void HandleCommand( const std::vector<char*>& args ) {};
};

template <typename T>
class Command : public DebugCommandHandler {
	typedef void ( *funcPtr )( const std::vector<char*>& );
public:
	Command( funcPtr func ) {
		m_callbackFunc = func;
	}

	virtual void HandleCommand( const std::vector<char*>& args ) {
		( *m_callbackFunc )( args );
	}

private:
	funcPtr	m_callbackFunc;
};

typedef void ( DebugCommandHandler::*DebugConsoleCommandFunc )( char* );


class DebugConsoleModule : public EngineModule {
public:
	static DebugConsoleModule*										CreateDebugConsoleWithQueueSize( unsigned int maxBufferSize );
	bool															InitializeDebugConsoleWithQueueSize( unsigned int maxBufferSize );
						
																	~DebugConsoleModule( void );

	virtual void													Startup( void ) {}
	virtual void													Update( float dt );
	virtual void													Shutdown( void ) {}

	void															ProcessCommand( const char* cmd );
	const char*	const												GetPreviousCommand( void );
	const char*	const												GetNextCommand( void );
	
	template <typename T>
	static void														RegisterCommand( const char* commandName, void ( *funcPtr )( const std::vector<char*>& ) );
private:
																	DebugConsoleModule( void );

	static std::unordered_map<unsigned long, DebugCommandHandler*>&	GetCommandFunctions( void );
	std::deque<char*>::iterator										m_commandIter;
	std::deque<char*>												m_previousCommands;
	unsigned int													m_maxBufferSize;
	DebugGUILayer*													m_debugLayer;
	unsigned int													m_framesSinceUpdate;		
};

template <typename T>
void DebugConsoleModule::RegisterCommand( const char* commandName, void ( *funcPtr )( const std::vector<char*>& ) ) {
	unsigned long cmdHash = StringUtils::Hash( commandName );
	std::unordered_map<unsigned long, DebugCommandHandler*>& commandFunctions = GetCommandFunctions();

	if ( commandFunctions.find( cmdHash ) == commandFunctions.end() ) {
		commandFunctions[cmdHash] = new Command<T>( funcPtr );
	} else {
		char buff[512];
		memset( buff, 0, 512 );
		sprintf_s( buff, "Attempted to add command %s twice", commandName );

		Log::GetInstance()->WriteToLog( "DebugConsoleModule", buff );
	}
}

}

#endif // __DEBUGCONSOLEMODULE_H__