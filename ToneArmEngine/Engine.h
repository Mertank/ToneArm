/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Engine.h

		Base of the ToneArm Engine.

		Created by: Karl Merten
		Created on: 25/04/2014

========================
*/

#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <vector>
#include <unordered_map>

#include "VGSMacro.h"

#include <NetConsts.h>
#include <Modular.h>

namespace vgs {

class Scene;
class EngineModule;

struct RunningInfo;

class Engine : public Modular {
									CREATE_SINGLETON( Engine )
public:
									~Engine( void );

	static bool						Initialize( void );
	static int						RunScene( Scene* scene );
	static void						SetScene( Scene* scene );
	inline static Scene*			GetRunningScene( void )						{ return GetInstance()->m_runningScene; }
	inline static RunningInfo*		GetRunningInfo( void )						{ return GetInstance()->m_engineInfo; }
	void							ApplicationWasClosed( void );
	const Tick&						GetTick() const								{ return m_tick; }

#ifdef TONEARM_DEBUG
	unsigned int					GetInstantaniousFrameRate( void )			{ return m_instantaniousFrameRate; }
	float							GetAverageFrameRate( void )					{ return m_averageFrameRate; }
#endif

private:
	void							Startup( void );
	int								MainLoop( void );
	void							Shutdown( void );
	void							ProcessUICommands();

private:
	RunningInfo*					m_engineInfo;
	bool							m_applicationRunning;
	Scene*							m_runningScene;

	Tick							m_tick;

#ifdef TONEARM_DEBUG
	unsigned int					m_instantaniousFrameRate;
	float							m_averageFrameRate;
	float							m_averageTimeElapsed;
	unsigned int					m_averageFrameRateSum;
	unsigned int					m_frameCount;
#endif

};
} //namespace vgs

#endif //__ENGINE_H__