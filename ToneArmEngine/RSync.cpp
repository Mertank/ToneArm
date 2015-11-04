/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RSync.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 7/25/2014 1:26:02 PM

========================
*/
#include "RSync.h"

namespace vgs
{
	std::mutex				g_lockRenderer;
	std::condition_variable	g_logicDoneCheck;
	bool					g_rendererNotified = false;
}