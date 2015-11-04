/*
========================

Copyright (c) 2014 Vinyl Games Studio

	EngineModule

		Abstract base for engine modules.

		Created by: Karl Merten
		Created on: 25/04/2014

========================
*/

#ifndef __ENGINEMODULE_H__
#define __ENGINEMODULE_H__

namespace vgs {
	class EngineModule {
	public:
		virtual			~EngineModule( void ) {}

		virtual void	Startup( void ) = 0;
		virtual void	Update( float dt ) = 0;
		virtual void	Shutdown( void ) = 0;
	};
} //namespace vgs

#endif //__ENGINEMODULE_H__