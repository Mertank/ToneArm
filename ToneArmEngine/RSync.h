/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RSync.h

		Created by: Vladyslav Dolhopolov
		Created on: 7/25/2014 1:26:02 PM

========================
*/
#ifndef __RSYNC_H__
#define __RSYNC_H__

#include <mutex>
#include <condition_variable>

#include "NetConsts.h"

namespace vgs
{
	extern std::mutex				g_lockRenderer;
	extern std::condition_variable	g_logicDoneCheck;
	extern bool						g_rendererNotified;

	inline void RSync_SendNotification()
	{
		std::unique_lock<std::mutex> locker(g_lockRenderer);
        
		g_rendererNotified = true;
        g_logicDoneCheck.notify_one();
	}

	inline void RSync_WaitForNotification()
	{
		std::unique_lock<std::mutex> locker(g_lockRenderer);
		
		// to avoid spurious wakeups 
        while(!g_rendererNotified) 
        {
            g_logicDoneCheck.wait(locker);
        }
		
		g_rendererNotified = false;
	}

} // namespace vgs

#endif __RSYNC_H__