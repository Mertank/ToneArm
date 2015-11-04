/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ProcessUtils.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/21/2014 11:44:04 AM

========================
*/
#ifndef __PROCESSUTILS_H__
#define __PROCESSUTILS_H__

namespace vgs
{
	/*
	========================

		ProcessUtils

			Helper functions to create and manage processes.

			Created by: Vladyslav Dolhopolov
			Created on: 5/21/2014 11:44:04 AM

	========================
	*/
	class ProcessUtils
	{
	public:
		static bool IsProcessRunning(const char* processName);
		static bool StartProcess(char* processName, char* cmdLine);

	};

} // namespace vgs

#endif __PROCESSUTILS_H__