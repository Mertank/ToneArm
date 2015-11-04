#include "MessagingModule.h"
#include "Log.h"
#include "MessagingSystem.h"

namespace vgs {
/*
==============
MessagingModule::~MessagingModule

	MessagingModule destructor.
==============
*/
MessagingModule::~MessagingModule( void ) {
	std::unordered_map<const RTTI*, BaseMessagingSystem*>::iterator iter	= m_messageSystems.begin();
	std::unordered_map<const RTTI*, BaseMessagingSystem*>::iterator iterEnd = m_messageSystems.end();

	while ( iter != iterEnd ) {
		delete iter->second;
		++iter;
	}
}
/*
==============
MessagingModule::Create

	MessagingModule creation method.
==============
*/
MessagingModule* MessagingModule::Create( void ) {
	MessagingModule* module = new MessagingModule();
	if ( module && module->Initialize() ) {
		return module;
	}

	delete module;
	return NULL;
}
/*
==============
MessagingModule::Initialize

	MessagingModule initialization.
==============
*/
bool MessagingModule::Initialize( void ) {
	return true;
}
/*
==============
MessagingModule::GetMessageSystem

	Gets a message system.
==============
*/
BaseMessagingSystem* MessagingModule::GetMessageSystem( const RTTI& receiverRTTI ) {
	std::unordered_map<const RTTI*, BaseMessagingSystem*>::iterator messageSystem = m_messageSystems.find( &receiverRTTI );
	if ( messageSystem != m_messageSystems.end() ) {
		return messageSystem->second;
	}

	std::string logMsg( "Could not find message system for target: " );
	logMsg += receiverRTTI.GetClass();
	
	Log::GetInstance()->WriteToLog( "MessagingModule", logMsg.c_str() );

	return NULL;
}
/*
==============
MessagingModule::ProcessMessagesInSystem

	Processes messages for the system.
==============
*/
void MessagingModule::ProcessMessagesInSystem( const RTTI& receiverRTTI ) {
	BaseMessagingSystem* messageSystem = GetMessageSystem( receiverRTTI );
	if ( messageSystem ) {
		messageSystem->ProcessMessages();
	}
}
/*
==============
MessagingModule::DestroySystemForReceiver

	Destroys a message system.
==============
*/
void MessagingModule::DestroySystemForReceiver( const RTTI& receiverRTTI ) {
	std::unordered_map<const RTTI*, BaseMessagingSystem*>::iterator messageSystem = m_messageSystems.find( &receiverRTTI );
	if ( messageSystem != m_messageSystems.end() ) {
		delete messageSystem->second;
		m_messageSystems.erase( messageSystem );
	}
}

}