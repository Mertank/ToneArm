/*
========================

Copyright (c) 2014 Vinyl Games Studio

	SystemInfo

		Stores the system information in a singleton.
		http://www.gamedev.net/topic/449965-detecting-cpu-clock-rate-total-ram-and-current-available-ram-in-c/

		Created by: Karl Merten
		Created on: 17/03/2014

========================
*/
#ifndef __SYSTEMINFO_H__
#define __SYSTEMINFO_H__

#include <string>
#include "VGSMacro.h"
typedef unsigned long DWORD;

namespace vgs {

class SystemInfo {
						CREATE_SINGLETON( SystemInfo )
public:
						~SystemInfo( void );
	void				WriteToLog( void );
private:

	void				InitializeAvailableRAM( void );
	void				InitializeOSVersion( void );
	double				GetCPUSpeed( DWORD dwCPU );
	void				InitializeCPUSpecs( void );

	unsigned long		totalRAM;
	unsigned long		availableRAM;
	double*				CPUSpeeds;

	std::string*		OSVersion;
	std::string*		buildVersion;
	std::string*		processorName;
	std::string*		processorFeatures;

	unsigned int		processorType;
	unsigned int		L1CacheSize;
	unsigned int		L2CacheSize;	
	unsigned int		numberOfProcessors;	
};

}

#endif //__SYSTEMINFO_H__