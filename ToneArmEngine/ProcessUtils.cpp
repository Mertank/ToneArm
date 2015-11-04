/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ProcessUtils.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 5/21/2014 11:44:04 AM

========================
*/
#include "ProcessUtils.h"

#include <Windows.h>
#include <TlHelp32.h>

#include <iostream>

using namespace vgs;

bool ProcessUtils::StartProcess(char* processName, char* cmdLine)
{
	// storage for the startup info
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	BOOL retVal = CreateProcess(NULL, processName, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	if (!retVal)
	{
		std::cerr << "Starting process: Fail to start process" << std::endl;
		return false;
	}

	// process was created
	std::cout << "Starting process: Blocking untill process startup is complete" << std::endl;
	WaitForInputIdle(pi.hProcess, INFINITE);
	std::cout << "Starting process: Startup complete" << std::endl;

	// cleanup
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	
	return true;
}

bool ProcessUtils::IsProcessRunning(const char* processName)
{
	HANDLE snapeshotOfProcesses = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (snapeshotOfProcesses == INVALID_HANDLE_VALUE)
	{
		// snapshot was not created
		std::cerr << "Starting process: Invalid handle returned by CreateToolhelp32Snapshot" << std::endl;
		return false;
	}

	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(PROCESSENTRY32);

	// get first process if one exists and parse through remaining ones
	if (Process32First(snapeshotOfProcesses, &processInfo))
	{
		do
		{
			// check if process name mathces the one we are looking for
			//if (processName.compare(processInfo.szExeFile) == 0)
			if (strcmp(processName, processInfo.szExeFile) == 0)
			{
				// this is the process we want
				CloseHandle(snapeshotOfProcesses);
				return true;
			}
		}
		while (Process32Next(snapeshotOfProcesses, &processInfo));
	}
	// process was not found
	CloseHandle(snapeshotOfProcesses);
	
	return false;
}

