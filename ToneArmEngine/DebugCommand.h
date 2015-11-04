/*
========================

Copyright (c) 2014 Vinyl Games Studio

	DebugCommand

		Classes to allow for hooking up command handling in the debug console.

		Created by: Karl Merten
		Created on: 02/07/2014

========================
*/

#ifndef __DEBUGCOMMAND_H__
#define __DEBUGCOMMAND_H__

#include <vector>
#include "Engine.h"
#include "DebugConsoleModule.h"

//This define thing is the greatest/worst thing I've ever done.
//I will never write anything better than this
#ifdef TONEARM_DEBUG

	#ifndef MERRY_SERVER

		#define DECLARE_DEBUG_COMMAND_CALLBACK( __className__, __cmdName__ )		\
		private:																	\
			static CommandInitializer<__className__> __cmdName__##Initializer;		\
			static void __cmdName__##Callback( const std::vector<char*>& );			\

		#define INITIALIZE_DEBUG_COMMAND_CALLBACK( __className__, __cmdName__ )													\
			CommandInitializer<__className__> __className__::__cmdName__##Initializer( #__cmdName__, &__cmdName__##Callback );	\

		#define BEGIN_CALLBACK_FUNCTION( __className__, __cmdName__ )						\
			void __className__::__cmdName__##Callback( const std::vector<char*>& args ) {	\
		
		#define END_CALLBACK_FUNCTION }

	#else

		#define DECLARE_DEBUG_COMMAND_CALLBACK( __className__, __cmdName__ )		\
		private:																	\
			template <typename T>													\
			static void __cmdName__##Callback( const std::vector<char*>& );			\

		#define INITIALIZE_DEBUG_COMMAND_CALLBACK( __className__, __cmdName__ )	

		#define BEGIN_CALLBACK_FUNCTION( __className__, __cmdName__ )						\
			template <typename T>															\
			void __className__::__cmdName__##Callback( const std::vector<char*>& args ) {	\
		
		#define END_CALLBACK_FUNCTION }

	#endif

#else

	#define DECLARE_DEBUG_COMMAND_CALLBACK( __className__, __cmdName__ )		\
	private:																	\
		template <typename T>													\
		static void __cmdName__##Callback( const std::vector<char*>& );			\

	#define INITIALIZE_DEBUG_COMMAND_CALLBACK( __className__, __cmdName__ )	

	#define BEGIN_CALLBACK_FUNCTION( __className__, __cmdName__ )						\
		template <typename T>															\
		void __className__::__cmdName__##Callback( const std::vector<char*>& args ) {	\
		
	#define END_CALLBACK_FUNCTION }

#endif

namespace vgs {

template <typename T>
class CommandInitializer {
public:
	CommandInitializer( const char* cmd, void ( *func )( const std::vector<char*>& ) ) {
		DebugConsoleModule::RegisterCommand<T>( cmd, func );
	}
};

}

#endif