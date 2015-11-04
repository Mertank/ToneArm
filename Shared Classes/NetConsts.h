/*
========================

Copyright (c) 2014 Vinyl Games Studio

	NetConsts.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/22/2014 1:57:27 PM

========================
*/
#ifndef __NETCONSTS_H__
#define __NETCONSTS_H__

#include <GetTime.h>

namespace vgs
{
	typedef unsigned long long tick_t;

	struct Tick 
	{
		// frame number
		tick_t			frameno;	
		// time since last frame (sec)
		float			dt;		
		// last frame sleep time (millisec)
		unsigned int	st;	
		// total elapsed game time (sec)
		double			t;			

		Tick()
			: frameno(0)
			, dt(0)
			, st(0)
			, t(0)
		{
		}
	};

	struct NetInfo
	{
		// frames per second
		int						ticksPerSecond;
		// length of fixed time step
		float					deltaTime;
		// ...
		unsigned short			defaultPort;
		// in case default is busy
		mutable unsigned short	actualPort;
		// lost connection timeout
		unsigned short			timeoutTime;
		// server will send updates every yay many frames (server side)
		tick_t					updatePeriod;

		NetInfo()
			: ticksPerSecond(0)
			, deltaTime(0)
			, defaultPort(0)
			, actualPort(0)
			, timeoutTime(0)
			, updatePeriod(0)
		{
		}

		NetInfo(int ticks, unsigned short port, unsigned short timeout)
			: ticksPerSecond(ticks)
			, deltaTime(1.f / ticksPerSecond)
			, defaultPort(port)
			, actualPort(defaultPort)
			, timeoutTime(timeout)
		{
		}

		NetInfo(int ticks, unsigned short port, unsigned short timeout, tick_t updateP)
			: ticksPerSecond(ticks)
			, deltaTime(1.f / ticksPerSecond)
			, defaultPort(port)
			, actualPort(defaultPort)
			, timeoutTime(timeout)
			, updatePeriod(updateP)
		{
		}
	};
	
#ifdef TONEARM_DEBUG
	const NetInfo g_clientNetInfo(30, 60001, 30000);
	const NetInfo g_serverNetInfo(15, 60000, 30000, 1);
#else
	const NetInfo g_clientNetInfo(60, 60001, 10000);
	const NetInfo g_serverNetInfo(60, 60000, 10000, 3);
#endif

}

#endif __NETCONSTS_H__