/*
========================

	MessagingModule

		The module that creates and manages messageSystem.

		Created by: Karl Merten
		Created on: 24/09/2014

========================
*/

#ifndef __MESSAGINGMODULE_H__
#define __MESSAGINGMODULE_H__

#include "EngineModule.h"
#include <unordered_map>
#include "RTTI.h"

namespace vgs {

template <typename T>
class MessagingSystem;

class BaseMessagingSystem;
class RTTI;

class MessagingModule : public EngineModule  {
public:
																~MessagingModule( void );

	static MessagingModule*										Create( void );

	template <typename T> MessagingSystem<T>*					CreateMessageSystemForReceiver( T* receiver, unsigned int systemSize );
	void														DestroySystemForReceiver( const RTTI& receiverRTTI );

	template <typename T> MessagingSystem<T>*					GetMessageSystemForTarget( void );	
	void														ProcessMessagesInSystem( const RTTI& receiverRTTI );

	virtual void												Startup( void ) {}
	virtual void												Update( float dt ) {}
	virtual void												Shutdown( void ) {}
private:
	bool														Initialize( void );

	BaseMessagingSystem*										GetMessageSystem( const RTTI& receiver );

	std::unordered_map<const RTTI*, BaseMessagingSystem*>		m_messageSystems;
};
/*
==============
MessagingModule::CreateMessageSystemForReceiver

	Creates a messaging system.
==============
*/
template <typename T> 
MessagingSystem<T>*	MessagingModule::CreateMessageSystemForReceiver( T* receiver, unsigned int systemSize ) {
	MessagingSystem<T>* messagingSystem = MessagingSystem<T>::CreateForReceiver( receiver, systemSize );
	m_messageSystems[&T::rtti]			= messagingSystem;

	return messagingSystem;
}

/*
==============
MessagingModule::GetMessageSystemForTarget

	Gets a message system and casts to it.
==============
*/
template <typename T>
MessagingSystem<T>* MessagingModule::GetMessageSystemForTarget( void ) {
	BaseMessagingSystem* messageSystem = GetMessageSystem( T::rtti );
	if ( messageSystem ) {
		return ( ( MessagingSystem<T>* )messageSystem );
	}

	return NULL;
}

}

#endif //__MESSAGINGMODULE_H__