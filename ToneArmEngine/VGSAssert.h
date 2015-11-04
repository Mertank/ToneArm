/*
========================

Copyright (c) 2014 Vinyl Games Studio

	VGSAssert

		A macro to allow assert in debug, but not release builds

		Created by: Karl Merten
		Created on: 17/03/2014

========================
*/
#ifndef __VGSASSERT_H__
#define __VGSASSERT_H__

#ifdef TONEARM_DEBUG 
	#include <cassert>
#endif

#ifdef TONEARM_DEBUG
	#define VGSAssert( cond, msg ) assert( ( cond, msg ) );
#else
	#define VGSAssert( cond, msg ) while ( 0 ) {};
#endif

#endif //__VGSASSERT_H__