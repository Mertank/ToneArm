/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	RunningInfo

	Created by: Karl Merten
	Created on: 25/04/2014

========================
*/
#include "RunningInfo.h"
#include "InitializationFileResource.h"

namespace vgs {
/*
============
RunningInfo::RunningInfo

	RunningInfo default constructor
============
*/
RunningInfo::RunningInfo( void ) :
	contextHeight( 800 ),
	contextWidth( 600 ),
	bitsPerPixel( 32 ),
	contextStyle( ContextStyle::WINDOWED ),
	targetFrameRate( 30 ),
	rendererType( RendererType::OPENGL )
{
	int length = strlen( "ToneArmEngine Application" );
	this->contextName = new char[length + 1];
	memset( ( void* )this->contextName, 0, length + 1 );
	memcpy( ( void* )this->contextName, "ToneArmEngine Application", length );
}
/*
============
RunningInfo::RunningInfo

	RunningInfo constructor
============
*/
RunningInfo::RunningInfo( unsigned int		contextHeight,
												unsigned int		contextWidth,
												unsigned int		bitsPerPixel,
												const char*			contextName,
												ContextStyle::Value contextStyle,
												unsigned int		targetFrameRate,
												RendererType::Value rendererType ) :
	contextHeight( contextHeight ),
	contextWidth( contextWidth ),
	bitsPerPixel( bitsPerPixel ),
	contextStyle( contextStyle ),
	targetFrameRate( targetFrameRate ),
	rendererType( rendererType )
{
	int length = strlen( contextName );
	this->contextName = new char[length + 1];
	memset( ( void* )this->contextName, 0, length + 1 );
	memcpy( ( void* )this->contextName, contextName, length );
} 
/*
============
RunningInfo::~RunningInfo

	RunningInfo destructor
============
*/
RunningInfo::~RunningInfo( void ) {
	delete[] contextName;
}
/*
============
RunningInfo::FromInitializationFile

	Loads the engine arguments from an initialization file
============
*/
RunningInfo* RunningInfo::FromInitializationFile( std::shared_ptr<InitializationFileResource> file ) {
	RunningInfo* retSettings = new RunningInfo();
	const InitializationCategory* appSettings = file->GetCategory( "Application" );

	if ( appSettings ) {
		retSettings->contextWidth		= appSettings->GetIntValueForKey( "Width" );
		retSettings->contextHeight		= appSettings->GetIntValueForKey( "Height" );
		retSettings->bitsPerPixel		= appSettings->GetIntValueForKey( "BitsPerPixel" );
		retSettings->targetFrameRate	= appSettings->GetIntValueForKey( "FrameRate" );
		
		const char* style				= appSettings->GetStringValueForKey( "Style" );
		if ( strcmp( style, "Windowed" ) == 0 ) {
			retSettings->contextStyle = ContextStyle::WINDOWED;
		} else if ( strcmp( style, "BorderlessWindowed" ) == 0 ) {
			retSettings->contextStyle = ContextStyle::BORDERLESS_WINDOW;
		} else {
			retSettings->contextStyle = ContextStyle::FULLSCREEN;
		}

		const char* name = appSettings->GetStringValueForKey( "Title" );
		if ( name ) {
			delete[] retSettings->contextName;

			int length = strlen( name );
			retSettings->contextName = new char[length + 1];
			memset( ( void* )retSettings->contextName, 0, length + 1 );
			memcpy( ( void* )retSettings->contextName, name, length );
		}
	}

	const char* type = appSettings->GetStringValueForKey( "Renderer" );
	if ( strcmp( type, "OpenGL" ) == 0 ) {
		retSettings->rendererType = RendererType::OPENGL;
	}

	return retSettings;
}

}