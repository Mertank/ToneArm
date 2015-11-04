/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RunningInfo

		Info about the engine instance.

		Created by: Karl Merten
		Created on: 25/04/2014

========================
*/

#ifndef __ENGINESTARTUPARGUMENTS_H__
#define __ENGINESTARTUPARGUMENTS_H__

#include <memory>

namespace vgs {

class InitializationFileResource;

namespace ContextStyle {
	enum Value {
		FULLSCREEN			= 0x00,
		WINDOWED			= 0x01,
		BORDERLESS_WINDOW	= 0x02
	};
};

namespace RendererType {
	enum Value {
		OPENGL	= 0x00
	};
};

struct RunningInfo {	
	unsigned int			contextHeight;
	unsigned int			contextWidth;
	unsigned int			bitsPerPixel;

	const char*				contextName;

	ContextStyle::Value		contextStyle;
	RendererType::Value		rendererType;

	unsigned int			targetFrameRate;

							RunningInfo( void );
							RunningInfo( unsigned int contextHeight, unsigned int contextWidth,
													unsigned int bitsPerPixel,	const char*	 contextName,
													ContextStyle::Value contextStyle,
													unsigned int targetFrameRate, RendererType::Value rendererType );

							~RunningInfo( void );

	static RunningInfo*		FromInitializationFile( std::shared_ptr<InitializationFileResource> file );
};

} //namespace vgs
#endif //__APPLICATIONSETTINGS_H__
