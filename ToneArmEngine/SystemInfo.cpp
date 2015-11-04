/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	SystemInfo

	Created by: Karl Merten
	Created on: 17/03/2014

========================
*/
#include "SystemInfo.h"
#include "Log.h"
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif
namespace vgs {

INITIALIZE_SINGLETON( SystemInfo )
/*
========
SystemInfo::SystemInfo

	SystemInfo default constructor
========
*/
SystemInfo::SystemInfo( void ) :
	totalRAM( 0 ),
	availableRAM( 0 ),
	CPUSpeeds( NULL ),
	numberOfProcessors( 0 ),
	L1CacheSize( 0 ),
	L2CacheSize( 0 ),
	processorType( PROCESSOR_ARCHITECTURE_UNKNOWN )	
{
	OSVersion = new std::string();
	buildVersion = new std::string();
	processorName = new std::string();
	processorFeatures = new std::string();

	InitializeAvailableRAM();
	InitializeOSVersion();
	InitializeCPUSpecs();
}
/*
========
SystemInfo::~SystemInfo

	SystemInfo destructor
========
*/
SystemInfo::~SystemInfo( void ) {
	delete OSVersion;
	delete buildVersion;
	delete processorName;
	delete processorFeatures;
	delete[] CPUSpeeds;
}
/*
========
SystemInfo::GetCPUSpeed

	Determines the available physical RAM.
========
*/
double SystemInfo::GetCPUSpeed( DWORD dwCPU ) {
#ifdef _WIN32
	HKEY	hKey;
	DWORD	speed;
	char	buffer[256];

	_snprintf_s( buffer, sizeof( buffer ) / sizeof( wchar_t ), 
		       "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d\\", dwCPU );

	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, ( char* )buffer, 0, KEY_QUERY_VALUE, &hKey ) != ERROR_SUCCESS ) {
		return 0;
	}

	DWORD length = 4;
	if ( RegQueryValueEx( hKey, "~MHz", NULL, NULL, ( LPBYTE )&speed, &length ) != ERROR_SUCCESS ) {
		RegCloseKey( hKey );
		return 0;
	}

	RegCloseKey( hKey );
	
	return speed / 1000.0;
#endif
}
/*
========
SystemInfo::InitializeCPUSpecs

	Initializes the specs of the CPU
========
*/
void SystemInfo::InitializeCPUSpecs( void ) {
#ifdef _WIN32
	int				iBuffer[4];
	char			cBuffer[13];
	char			features[256];
	unsigned int	highestFeature;
	unsigned int	highestFeatureEx;

	__cpuid( iBuffer, 0 );
	highestFeature = ( unsigned int )iBuffer[0];
	*( int* )&cBuffer[0] = iBuffer[1];
	*( int* )&cBuffer[4] = iBuffer[3];
	*( int* )&cBuffer[8] = iBuffer[2];
	cBuffer[12] = '\0';
	if ( strcmp( cBuffer, "AuthenticAMD" ) == 0 ) {
		processorType = PROCESSOR_AMD_X8664;
	} else if ( strcmp( cBuffer, "GenuineIntel" ) == 0 ) {
		processorType = PROCESSOR_ARCHITECTURE_INTEL;
	}

	__cpuid( iBuffer, 0x80000000 );
	highestFeatureEx = ( unsigned int )iBuffer[0];

	if ( highestFeatureEx >= 0x80000004 ) {
		char cpuName[49];
		cpuName[0] = '\0';
		__cpuid( ( int* )&cpuName[0], 0x80000002 );
		__cpuid( ( int* )&cpuName[16], 0x80000003 );
		__cpuid( ( int* )&cpuName[32], 0x80000004 );
		cpuName[48] = '\0';

		for ( unsigned int i = strlen( cpuName ) - 1; i >= 0; --i ) {
			if ( cpuName[i] == ' ' ) {
				cpuName[i] = '\0';
			} else {
				break;
			}
		}

		( *processorName ) = cpuName;
		( *processorName ) = processorName->substr( 0, strlen( cpuName ) );
		
		( *processorName ) += " ( ";
		( *processorName ) += cBuffer;
		( *processorName ) += " )";
	} else {
		( *processorName ) = cBuffer;
	}

	unsigned int firstChar = processorName->find_first_not_of( ' ' );
	( *processorName ) = processorName->substr( firstChar, processorName->length() - firstChar );

	features[0] = '\0';
	if ( highestFeature >= 1 ) {
		__cpuid( iBuffer, 1 );
		if ( iBuffer[3] & 1 << 0 ) {
			strcat_s( features, "FPU " );
		}
		if ( iBuffer[3] & 1 << 23 ) {
			strcat_s( features, "MMX " );
		}
		if ( iBuffer[3] & 1 << 25 ) {
			strcat_s( features, "SSE " );
		}
		if ( iBuffer[3] & 1 << 26 ) {
			strcat_s( features, "SSE2 " );
		}
		if ( iBuffer[2] & 1 << 0 ) {
			strcat_s( features, "SSE3 " );
		}
		//Intel specific
		if ( processorType == PROCESSOR_ARCHITECTURE_INTEL ) {
			if ( iBuffer[2] & 1 << 9 ) {
				strcat_s( features, "SSSE3 " );
			}
			if ( iBuffer[2] & 1 << 4 ) {
				strcat_s( features, "EST " );
			}
		}
		if ( iBuffer[3] & 1 << 28 ) {
			strcat_s( features, "HTT " );
		}
	}
	//AMD Specific
	if ( processorType == PROCESSOR_AMD_X8664 ) {
		__cpuid( iBuffer, 0x80000000 );
		if ( highestFeatureEx >= 0x80000001 ) {
			__cpuid( iBuffer, 0x80000001 );
			if ( iBuffer[3] & 1 << 31 ) {
				strcat_s( features, "3DNow! " );
			}	
			if ( iBuffer[3] & 1 << 30 ) {
				strcat_s( features, "Ex3DNow! " );
			}	
			if ( iBuffer[3] & 1 << 22 ) {
				strcat_s( features, "MmxExt " );
			}	
		}

		if ( highestFeatureEx >= 0x80000005 ) {
			__cpuid( iBuffer, 0x80000005 );
			L1CacheSize = ( ( unsigned int )iBuffer[2] >> 24 );
		}
	}

	( *processorFeatures ) = features;
	( *processorFeatures ) = processorFeatures->substr( 0, strlen( features ) );

	//L2Cache Size
	if ( highestFeatureEx >= 0x80000006 ) {
		__cpuid( iBuffer, 0x80000006 );
		L2CacheSize = ( ( unsigned int )iBuffer[2] >> 16 );
	}

	SYSTEM_INFO sysInfo;
	GetSystemInfo( &sysInfo );

	numberOfProcessors = sysInfo.dwNumberOfProcessors;
	if ( numberOfProcessors > 0 ) {
		CPUSpeeds = new double[numberOfProcessors];
		for ( unsigned int i = 0; i < numberOfProcessors; ++i ) {
			CPUSpeeds[i] = GetCPUSpeed( i );
		}
	}
#endif
}
/*
========
SystemInfo::InitializeAvailableRAM

	Determines the available physical RAM.
========
*/
void SystemInfo::InitializeAvailableRAM( void ) {
#ifdef _WIN32
	MEMORYSTATUS memStatus;
	ZeroMemory( &memStatus, sizeof( memStatus ) );
	memStatus.dwLength = sizeof( memStatus );
	GlobalMemoryStatus( &memStatus );
	
	ULONGLONG totRAM;
	GetPhysicallyInstalledSystemMemory( &totRAM );

	totalRAM		= ( unsigned int )( totRAM / 1024.0 );
	availableRAM	= ( memStatus.dwAvailPhys / ( 1024 * 1024 ) );
#endif
}
/*
========
SystemInfo::InitializeOSVersion

	Determines the OS Version, and the build version.
========
*/
void SystemInfo::InitializeOSVersion( void ) {
#ifdef _WIN32
	OSVERSIONINFOEX	osInfo;
	SYSTEM_INFO		sysInfo;

	ZeroMemory( &sysInfo, sizeof( sysInfo ) );
	GetSystemInfo( &sysInfo );

	//Get OS version
	ZeroMemory( &osInfo, sizeof( osInfo ) );
	osInfo.dwOSVersionInfoSize = sizeof( osInfo );
	/*if ( !GetVersionEx( ( OSVERSIONINFO* )&osInfo ) ) {
		ZeroMemory( &osInfo, sizeof( osInfo ) );
		osInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
		GetVersionEx( ( OSVERSIONINFO* )&osInfo );
	}*/

	if ( osInfo.dwMinorVersion == 5 ) {
		if ( osInfo.dwMinorVersion == 0 ) {
			( *OSVersion ) = "Windows 2000";
		} else if ( osInfo.dwMinorVersion == 1 ) {
			( *OSVersion ) = "Windows XP";
		} else if ( osInfo.dwMinorVersion == 2) {
			if ( GetSystemMetrics( SM_SERVERR2 ) ) {
				( *OSVersion ) = "Windows Server 2003 R2";
			} else {
				( *OSVersion ) = "Windows Server 2003";
			}
		} else {
			( *OSVersion ) = "Unknown Windows OS";
		}
	} else if ( osInfo.dwMajorVersion == 6 ) {
		if ( osInfo.dwMinorVersion == 0 ) {
			if ( osInfo.wProductType == VER_NT_WORKSTATION ) {
				( *OSVersion ) = "Windows Vista";
			} else {
				( *OSVersion ) = "Windows Server 2008";
			}
		} else if ( osInfo.dwMinorVersion == 1 ) {
			if ( osInfo.wProductType == VER_NT_WORKSTATION ) {
				( *OSVersion ) = "Windows 7";
			} else {
				( *OSVersion ) = "Windows Server 2008 R2";
			}
		} else if ( osInfo.dwMinorVersion == 2) {
			if ( osInfo.wProductType == VER_NT_WORKSTATION ) {
				( *OSVersion ) = "Windows 8";
			} else {
				( *OSVersion ) = "Windows Server 2012";
			}
		} else if ( osInfo.dwMinorVersion == 3) {
			if ( osInfo.wProductType == VER_NT_WORKSTATION ) {
				( *OSVersion ) = "Windows 8.1";
			} else {
				( *OSVersion ) = "Windows Server 2012 R2";
			}
		} else {
			( *OSVersion ) = "Unknown Windows OS";
		}
	} else {
		( *OSVersion ) = "Unknown Windows OS";
	}

	#ifdef BUILD_64
		( *buildVersion ) = "Win64 Version";
	#elif _WIN32
		( *buildVersion ) = "Win32 Version";
	#else
		( *buildVersion ) = "Unknown Win Version";
	#endif
#endif
}
/*
========
SystemInfo::WriteToLog

	Writes the system specs to the log.
========
*/
void SystemInfo::WriteToLog( void ) {
	Log* logger = Log::GetInstance();

	logger->WriteToLog( NULL, "System Specs", true );
	logger->WriteToLog( "OS Version", OSVersion->c_str(), true );
	logger->WriteToLog( "Build Version", buildVersion->c_str(), true );
	
	if ( L1CacheSize > 0 ) {
		logger->WriteToLog( "L1 Cache Size", std::to_string( L1CacheSize ).c_str(), true );
	}
	logger->WriteToLog( "L2 Cache Size", ( std::to_string( L2CacheSize ) + " KB" ).c_str(), true );

	logger->WriteToLog( "Processor", processorName->c_str(), true );
	logger->WriteToLog( "Processor Count", std::to_string( numberOfProcessors ).c_str(), true );

	std::string RAMStr = std::to_string( availableRAM );
	RAMStr += " MB / ";
	RAMStr += std::to_string( totalRAM );
	RAMStr += " MB";

	logger->WriteToLog( "Available RAM", RAMStr.c_str() );
}

}