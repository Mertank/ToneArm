/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Engine.h

	Created by: Karl Merten
	Created on: 25/04/2014

========================
*/
#include "Engine.h"
#include "ModelFileResource.h"
#include "RenderModule.h"
#include "Scene.h"
#include "InitializationFileResource.h"
#include "RunningInfo.h"
#include "CachedResourceLoader.h"
#include "Log.h"
#include "ModelResource.h"
#include "ModelNode.h"
#include "DebugConsoleModule.h"
#include "NetworkModule.h"
#include "RSync.h"
#include "UICommand.h"
#include "SoundModule.h"
#include "InputModule.h"
#include "PhysicsModule.h"

#include <RakSleep.h>

#include <SFML\System.hpp>

#define WIN_32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>
#include <sstream>

namespace vgs {

INITIALIZE_SINGLETON( Engine )
/*
========
Engine::Engine

	Engine default constructor
========
*/
Engine::Engine( void ) :
	m_applicationRunning( true ),
	m_runningScene( NULL ),
	m_engineInfo( NULL )
#ifdef TONEARM_DEBUG
	,
	m_instantaniousFrameRate( 30 ),
	m_averageFrameRate( 0 ),
	m_averageFrameRateSum( 0 ),
	m_averageTimeElapsed( 0.0f ),
	m_frameCount( 0 )
#endif
{}
/*
========
Engine::~Engine

	Engine destructor
========
*/
Engine::~Engine( void ) {
	delete m_engineInfo;	
}
/*
========
Engine::RunScene

	Initializes the engine, and runs the scene passed in.
========
*/
int Engine::RunScene( Scene* scene ) {
	Engine* inst = GetInstance();
	inst->SetScene(scene);
	
	int retValue = inst->MainLoop();
	
	GetInstance()->Shutdown();
	DestroyInstance();
	 
	return retValue;
}
/*
========
Engine::SetScene

	Sets scene.
========
*/
void Engine::SetScene( Scene* scene )
{
	Engine* inst = GetInstance();

	if (inst->m_runningScene)
	{
		inst->m_runningScene->ExitScene();
		delete inst->m_runningScene;
		inst->m_runningScene = nullptr;
	}

	if (scene)
	{
		inst->m_runningScene = scene;
	}

	if (inst->m_runningScene)
	{
		inst->m_runningScene->EnterScene();
	}
}
/*
========
Engine::Initialize

	Initializes the engine.
========
*/
bool Engine::Initialize( void ) {
	Engine* instance = GetInstance();
	Log::GetInstance()->Initialize();

	// load modules
	instance->m_modules[EngineModuleType::RESOURCELOADER]	= CachedResourceLoader::Create();
	instance->m_modules[EngineModuleType::NETWORKER]		= NetworkModule::Create();
	instance->m_modules[EngineModuleType::SOUNDER]			= SoundModule::Create();
	instance->m_modules[EngineModuleType::INPUTER]			= InputModule::Create();
	instance->m_modules[EngineModuleType::PHYSICS]			= PhysicsModule::Create();

	std::shared_ptr<InitializationFileResource> ini = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>( EngineModuleType::RESOURCELOADER )->LoadResource<InitializationFileResource>( "ToneArmEngine.ini" );
	instance->m_engineInfo = RunningInfo::FromInitializationFile( ini );	

	RenderModule* renderer = RenderModule::Create();
	if ( renderer ) {		
		instance->m_modules[EngineModuleType::RENDERER] = renderer;
	}

#ifdef TONEARM_DEBUG
	instance->m_modules[EngineModuleType::DEBUGCONSOLE] = DebugConsoleModule::CreateDebugConsoleWithQueueSize( 10 );
#endif

	instance->Startup();

	srand( ( unsigned int )time( NULL ) );

	return true;
}
/*
========
Engine::Startup

	Calls Startup on all modules
========
*/
void Engine::Startup( void ) {
	ModuleMap::iterator iter = m_modules.begin();
	while ( iter != m_modules.end() ) {
		if (iter->second)
		{
			iter->second->Startup();
		}
		++iter;
	}
}
/*
========
Engine::MainLoop

	The main loop of the engine.
	Updates all modules.
========
*/
int	Engine::MainLoop( void ) {
	m_tick.dt		= g_clientNetInfo.deltaTime;
	double overflow = 0;

	std::stringstream ss;
	ss << "Update rate: " << g_clientNetInfo.ticksPerSecond;
	Log::GetInstance()->WriteToLog("Engine", ss.str().c_str());
	
	ss.str(std::string());
	ss << "Fixed delta time: " << g_clientNetInfo.deltaTime << std::endl;
	Log::GetInstance()->WriteToLog("Engine", ss.str().c_str());
	
	while (m_applicationRunning)
	{
		// update global timing parameters
		//std::cout << "---L" << m_tick.frameno << "---" << std::endl;
		m_tick.t = m_tick.frameno * g_clientNetInfo.deltaTime;

		// ms to s
		double frameStart = 0.001 * RakNet::GetTime();   

		// update modules
		for (ModuleMap::iterator iter = m_modules.begin(); iter != m_modules.end(); ++iter) 
		{
			if (iter->second)
			{
				iter->second->Update(m_tick.dt);
			}
		}

		// process commands from rendering thread, if there is any
		if (UICommand::SizeEng() > 0)
		{
			ProcessUICommands();
		}

		// update scene node tree
		if (m_runningScene != nullptr) 
		{
			m_runningScene->Update(m_tick.dt);
		}

		// notify renderer to draw a frame
		RSync_SendNotification();

		// wait for the rest of the frame time to elapse
        double frameTime;
		RakNet::Time sleepStart = RakNet::GetTime();
		for(int i = 0; true; i++)
		{
            // figure out if we need to sleep for a bit
            double currTime = 0.001 * RakNet::GetTime();
            frameTime = overflow + currTime - frameStart;

			if (frameTime >= g_clientNetInfo.deltaTime)
			{
				m_tick.st = unsigned int(RakNet::GetTime() - sleepStart);

#ifdef TONEARM_DEBUG
				double delta = i == 0 ? frameTime : m_tick.dt;

				m_instantaniousFrameRate	= unsigned int(1.0f / delta + 0.5f);
				m_averageTimeElapsed		+= float(frameTime);
				m_averageFrameRateSum		+= m_instantaniousFrameRate;
				++m_frameCount;
			
				if (m_averageTimeElapsed > 1.0f)
				{
					m_averageTimeElapsed	= 0.0f;
					m_averageFrameRate		= (float)m_averageFrameRateSum / m_frameCount;

					m_averageFrameRateSum	= 0;
					m_frameCount			= 0;
				}
#endif
				break;
			}

            // chill for a bit
            RakSleep(1);
        }
        overflow = frameTime - g_clientNetInfo.deltaTime;
		m_tick.frameno++;
	}

	return 0;
}
/*
========
Engine::Shutdown

	Called before the program quits.
	Cleans up engine resources.
========
*/
void Engine::Shutdown( void ) {
	if (m_runningScene)
	{
		m_runningScene->ExitScene();
		delete m_runningScene;
		m_runningScene = nullptr;
	}

	ModuleMap::iterator iter = m_modules.begin();
	while ( iter != m_modules.end() ) {
		if (iter->second)
		{
			iter->second->Shutdown();
		}
		++iter;
	}	

	Log::DestroyInstance();
}
/*
========
Engine::ApplicationWasClosed

	Call back to notify the engine that the window was closed.
========
*/
void Engine::ApplicationWasClosed( void ) {
	m_applicationRunning = false;
	if (m_runningScene)
	{
		m_runningScene->WillClose();
	}
}
/*
========
Engine::ApplicationWasClosed

	ProcessUICommands
========
*/
void Engine::ProcessUICommands()
{
	// create temp queue
	UICommand::CmdQueue commands;
	// swap with existing
	UICommand::SwapEng(commands);

	while (!commands.empty())
	{
		UICommand cmd = commands.front();

		switch (cmd.GetId())
		{
		case UICommand::CMD_ENG_SET_SCENE:
			{
				Scene* scene = (Scene*)cmd.GetData();
				if (scene && scene->Init())
				{
					SetScene(scene);
					// we are gonna use the scene so no deletion
					
					// remove from list
					commands.pop_front();
					continue;
				}
			}
			break;

		case UICommand::CMD_ENG_SHUTDOWN:
			{
				ApplicationWasClosed();
			}
			break;

		default:
			break;
		}

		// free memory
		cmd.Free();
		// remove from list
		commands.pop_front();
	}
}

}