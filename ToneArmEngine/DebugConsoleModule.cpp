/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	DebugConsoleModule
	
	Created by: Karl Merten
	Created on: 14/05/2014

========================
*/
#include "DebugConsoleModule.h"
#include "Engine.h"
#include "OpenGLRenderModule.h"
#include "GUIManager.h"
#include "DebugGUILayer.h"
#include "OpenGLGUIPass.h"
#include "NetworkModule.h"

#include <NetConsts.h>

namespace vgs {
/*
==========
DebugConsoleModule::DebugConsoleModule

	DebugConsoleModule Default constructor
==========
*/
DebugConsoleModule::DebugConsoleModule( void ) :
	m_maxBufferSize( 5 ),
	m_framesSinceUpdate( 0 ),
	m_debugLayer(nullptr)
{
	m_commandIter = m_previousCommands.begin();
}
/*
==========
DebugConsoleModule::~DebugConsoleModule

	DebugConsoleModule Destructor
==========
*/
DebugConsoleModule::~DebugConsoleModule( void ) {
	//Clean up
	while ( !m_previousCommands.empty() ) {
		delete[] m_previousCommands.front();
		m_previousCommands.pop_front();
	}

	std::unordered_map<unsigned long, DebugCommandHandler*>& commandFuncs = GetCommandFunctions();
	for( std::unordered_map<unsigned long, DebugCommandHandler*>::iterator iter = commandFuncs.begin();
		 iter != commandFuncs.end(); ++iter ) {
		delete iter->second;
	}
}
/*
==========
DebugConsoleModule::CreateDebugConsoleWithQueueSize

	DebugConsoleModule creation
==========
*/
DebugConsoleModule* DebugConsoleModule::CreateDebugConsoleWithQueueSize( unsigned int maxBufferSize ) {
	 DebugConsoleModule* ptr = new DebugConsoleModule();
	 if ( ptr && ptr->InitializeDebugConsoleWithQueueSize( maxBufferSize ) ) {
		 return ptr;
	 } else {
		 delete ptr;
		 return NULL;
	 }
 }
/*
==========
DebugConsoleModule::InitializeDebugConsoleWithQueueSize

	DebugConsoleModule Initializer
==========
*/
 bool DebugConsoleModule::InitializeDebugConsoleWithQueueSize( unsigned int maxBufferSize ) {
	 (Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager())->
		 CreateLayer(new DebugGUILayer(), "debug_gui");
	 return true;
 }
 /*
==========
DebugConsoleModule::Update

	Updates debug labels
==========
*/
 void DebugConsoleModule::Update( float dt ) {
#ifdef TONEARM_DEBUG	
	 ++m_framesSinceUpdate;

	 if (!m_debugLayer)
	 {
		 m_debugLayer = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager()->GetLayer<DebugGUILayer>("debug_gui");
	 }

	 if ( m_debugLayer && m_debugLayer->IsVisible() ) {
		 if ( m_framesSinceUpdate > 0 ) {
			char buffer[255];
	 
			UpdateFPSLabelsMsg uMsg;

			bool runningSlow = Engine::GetInstance()->GetInstantaniousFrameRate() < ( unsigned int )g_clientNetInfo.ticksPerSecond;

			memset( &buffer[0], 0, 255 );
			sprintf_s( buffer, "Logic FPS: %u %s", Engine::GetInstance()->GetInstantaniousFrameRate(), runningSlow ? "(slow)" : "" );
			uMsg.lfr = std::string(buffer);

			memset( &buffer[0], 0, 255 );
			sprintf_s( buffer, "Logic Average FPS: %.2f", Engine::GetInstance()->GetAverageFrameRate() );
			uMsg.lafr = std::string(buffer);

			memset( &buffer[0], 0, 255 );
			sprintf_s( buffer, "Render FPS: %u", Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER )->GetInstantaniousFrameRate() );
			uMsg.rfr = std::string(buffer);

			memset( &buffer[0], 0, 255 );
			sprintf_s( buffer, "Render Average FPS: %.2f", Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER )->GetAverageFrameRate() );
			uMsg.rafr = std::string(buffer);

			m_debugLayer->SetUpdateMsg(uMsg);

			m_framesSinceUpdate = 0;

			UpdateNetworkLabelsMsg unMsg;
			std::stringstream ss;
			ss << "Client delay: " << NetworkModule::GetClientDelay();

			unMsg.inter = NetworkModule::GetEnableInterpolation() ? "Interpolation: on" : "Interpolation: off";
			unMsg.delay = ss.str();
			m_debugLayer->SetUpdateNetMsg(unMsg);
		 }
	 }
#endif
}
 /*
==========
DebugConsoleModule::ProcessCommand

	DebugConsoleModule processing commands
==========
*/
 void DebugConsoleModule::ProcessCommand( const char* cmd ) {
	 if ( m_previousCommands.size() > m_maxBufferSize ) {
		 delete m_previousCommands.front();
		 m_previousCommands.pop_front();
	 }

	 char* cmdCopy = StringUtils::CopyCString( cmd );
	 m_previousCommands.push_back( cmdCopy );
	 m_commandIter = m_previousCommands.end();

	 std::vector<char*> tokens;
	 StringUtils::Tokenize( cmdCopy, ' ', tokens );

	 unsigned long cmdHash = StringUtils::Hash( tokens[0] );
	 delete[] tokens[0];

	 tokens.erase( tokens.begin() );

	 std::unordered_map<unsigned long, DebugCommandHandler*>::iterator commandHandler = GetCommandFunctions().find( cmdHash );
	 if ( commandHandler != GetCommandFunctions().end() ) {
		commandHandler->second->HandleCommand( tokens );
	 } else {
		 char buffer[255];
		 memset( buffer, 0, 255 );
		 sprintf_s( buffer, "No commandHandler for command %s", cmd );
		 Log::GetInstance()->WriteToLog( "DebugConsole", buffer );
	 }

	 BEGIN_STD_VECTOR_ITERATOR( char*, tokens )
		 delete[] currentItem;
	 END_STD_VECTOR_ITERATOR

	tokens.clear();
 }
 /*
==========
DebugConsoleModule::GetPreviousCommand

	Gets the previous command from the position in the list.
==========
*/
const char* const DebugConsoleModule::GetPreviousCommand( void ) {
	char* ret = NULL;
	
	if ( m_commandIter != m_previousCommands.begin() ) {
		--m_commandIter;
	}
	
	if ( m_previousCommands.size() > 0 ) {
		ret = *m_commandIter;
	}
	
	return ret;
}
/*
==========
DebugConsoleModule::GetNextCommand

	Gets the next command from the position in the list.
==========
*/
const char* const DebugConsoleModule::GetNextCommand( void ) {
	char* ret = NULL;
	
	if ( m_commandIter != m_previousCommands.end() - 1 ) {
		++m_commandIter;		
	}

	if ( m_previousCommands.size() > 0 ) {
		ret = *m_commandIter;
	}
	
	return ret;
}
/*
==========
DebugConsoleModule::GetCommandFunctions

	No more initialization order russian roulette.
==========
*/
std::unordered_map<unsigned long, DebugCommandHandler*>& DebugConsoleModule::GetCommandFunctions( void ) {
	static std::unordered_map<unsigned long, DebugCommandHandler*> commandFuncs;
	return commandFuncs;
}

}