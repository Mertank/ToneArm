/*
========================

	BaseMessagingSystem

		A message systems for sending messages between 2 objects.
		This sub-system is thread-safe.

		Created by: Karl Merten
		Created on: 09/09/2014

========================
*/
#ifndef __BaseMessagingSystem_H__
#define __BaseMessagingSystem_H__

#include "RTTI.h"
#include "BasicMessage.h"
#include "SmallBlockAllocator.h"

#include <SFML\System.hpp>
#include <vector>
#include <string>
#include <unordered_map>

#undef SendMessage //FUCK YOUR SHIT WINDOWS!!!!!!!!!!!!!!!!
#undef DispatchMessage //FUCK YOUR SHIT WINDOWS!!!!!!!!!!!!!!!!

//Below are macros to make the sending of messages easier.
//Example use:
//
//BEGIN_SEND_MESSAGE_BLOCK( m_msgPool, ExampleMessage )
//	SET_MESSAGE_VALUE( Storage1, 1.0f )
//  SET_MESSAGE_VALUE( Storage2, "test" )
//END_SEND_MESSAGE_BLOCK

#define BEGIN_SEND_MESSAGE_BLOCK( __msgTarget__, __msgModule__, __msgType__ )								\
		MessagingSystem<__msgTarget__>* system = __msgModule__->GetMessageSystemForTarget<__msgTarget__>();	\
	    system->SendMessage( system->GetEmptyMessage<__msgType__>()											\

#define SET_MESSAGE_VALUE( __valueStorage__, __value__ )						\
		->Set##__valueStorage__##( __value__ )									\

#define END_SEND_MESSAGE_BLOCK													\
		);																		\

namespace sf {
class Mutex;
}

namespace vgs {

template <typename T>
class MessagingSystemReceiver;

class RTTI;

/*
===================

	BaseMessagingSystem

		The base of the messaging system class.
		Will create and free messages using small block allocators.
		Will store pending messages for retrieval.

===================
*/
class BaseMessagingSystem {
public:
	virtual							~BaseMessagingSystem( void );

	template <typename T> T*		GetEmptyMessage( void );
	template <typename T> T*		SendMessage( void );
	void							SendMessage( BasicMessage* msg );
	template <typename T> void		FreeMessage( T* msg );
	
	void							ProcessMessages( void );
	virtual void					ProcessMessage( BasicMessage* message ) = 0;
protected:
									BaseMessagingSystem( void );

	bool							InitializeWithSize( unsigned int bytes );

	SmallBlockAllocator*			m_msgAllocators[2];
	unsigned char					m_allocatorIndex;

	sf::Mutex						m_allocatorMutex;
	sf::Mutex						m_msgVectorMutex;

	std::vector<BasicMessage*>		m_pendingMessages;
};
/*
===================

	MessagingSystem

		This will store a receiver, and dispatch messages to it when requested.

===================
*/
template <typename T>
class MessagingSystem : public BaseMessagingSystem {
public:
	virtual							~MessagingSystem( void );

	static MessagingSystem<T>*		CreateForReceiver( T* receiver, unsigned int systemSize );
	
	virtual void					ProcessMessage( BasicMessage* message );
	template <typename R> void		RegisterMessageCallback( void ( T::*messageCallbackFunc )( R* ) );
private:
									MessagingSystem( void );

	bool							InitializeForReceiver( T* receiver, unsigned int systemSize );

	MessagingSystemReceiver<T>*		m_messageReceiver;
};
/*
===================

	MessagingSystemReceiver

		This is the objects that will process messages upon request.
		Has a series to functions hooked up to data types, and will call the function based on the message type.

===================
*/
template <typename T>
class MessagingSystemReceiver {
public:
	virtual															~MessagingSystemReceiver( void );

	static MessagingSystemReceiver<T>*								CreateForObject( T* obj, MessagingSystem<T>* messageSystem );

	virtual void													ProcessMessage( BasicMessage* message );
	template <typename R> void										RegisterMessageCallback( void ( T::*messageCallbackFunc )( R* ) );
private:
																	MessagingSystemReceiver( void );

	bool															InitializeForObject( T* obj, MessagingSystem<T>* messageSystem );

	struct BaseMessagingSystemCallback {
		BaseMessagingSystemCallback( void )  :
			m_messageSystem( NULL ),
			m_receiverObject( NULL )
		{}

		BaseMessagingSystemCallback( T* object, MessagingSystem<T>* system )  :
			m_messageSystem( system ),
			m_receiverObject( object )
		{}

		virtual void DispatchMessage( BasicMessage* message ) = 0;

		MessagingSystem<T>*	m_messageSystem;
		T*					m_receiverObject;
	};	

	template <typename R>
	struct MessagingSystemCallback : public BaseMessagingSystemCallback {
		MessagingSystemCallback( T* object, MessagingSystem<T>* system, void ( T::*func )( R* ) ) :
			BaseMessagingSystemCallback( object, system ),
			messageCallbackFunc( func )
		{}

		virtual void DispatchMessage( BasicMessage* message ) {
			( m_receiverObject->*( messageCallbackFunc ) )( ( R* )message );
			m_messageSystem->FreeMessage<R>( ( R* )message );
		}

		void ( T::*messageCallbackFunc )( R* );
	};

	MessagingSystem<T>*												m_messageSystem;
	T*																m_receiverObject;
	std::unordered_map<const RTTI*, BaseMessagingSystemCallback*>	m_messageCallbacks;
};

#pragma region BASEMESSAGINGSYSTEM
/*
========
BaseMessagingSystem::GetEmptyMessage

	Gets an empty message.
	Guarantees a value even if the pool is full.
========
*/
template <typename T>
T* BaseMessagingSystem::GetEmptyMessage( void ) {
	m_allocatorMutex.lock();
	T* retPtr = m_msgAllocators[m_allocatorIndex]->Alloc<T>();
	m_allocatorMutex.unlock();

	return retPtr;
}
/*
========
BaseMessagingSystem::SendMessage

	Sends an empty message.
	Laziness method.
========
*/
template <typename T>
T* BaseMessagingSystem::SendMessage( void ) {
	SendMessage( GetEmptyMessage<T>() );
}
/*
========
BaseMessagingSystem::FreeMessage

	Frees this message from the system.
========
*/
template <typename T>
void BaseMessagingSystem::FreeMessage( T* msg ) {
	m_allocatorMutex.lock();

	unsigned char freeIndex = m_allocatorIndex == 0 ? 1 : 0;
	if ( m_msgAllocators[freeIndex]->ContainsObject( msg ) ) {
		m_msgAllocators[freeIndex]->Free<T>( msg );
	} else {
		m_msgAllocators[m_allocatorIndex]->Free<T>( msg );
	}

	m_allocatorMutex.unlock();
}

#pragma endregion BaseMessagingSystems templated function implementation

#pragma region MESSAGINGSYSTEM
/*
========
MessagingSystem::MessagingSystem

	MessagingSystem default constructor.
========
*/
template <typename T>
MessagingSystem<T>::MessagingSystem( void ) :
	m_messageReceiver( NULL )
{}
/*
========
MessagingSystem::~MessagingSystem

	MessagingSystem destructor.
========
*/
template <typename T>
MessagingSystem<T>::~MessagingSystem( void ) {
	delete m_messageReceiver;
}
/*
========
MessagingSystem::CreateForReceiver

	MessagingSystem creation.
========
*/
template <typename T>
MessagingSystem<T>* MessagingSystem<T>::CreateForReceiver( T* receiver, unsigned int systemSize ) {
	MessagingSystem<T>* messageSystem = new MessagingSystem<T>();
	if ( messageSystem && messageSystem->InitializeForReceiver( receiver, systemSize ) ) {
		return messageSystem;
	}

	delete messageSystem;
	return NULL;
}
/*
========
MessagingSystem::RegisterMessageCallback

	MessagingSystem creation.
========
*/
template <typename T>
template <typename R>
void MessagingSystem<T>::RegisterMessageCallback( void ( T::*messageCallbackFunc )( R* ) ) {
	m_messageReceiver->RegisterMessageCallback<R>( messageCallbackFunc );
}
/*
========
MessagingSystem::InitializeForReceiver

	MessagingSystem initialization.
========
*/
template <typename T>
bool MessagingSystem<T>::InitializeForReceiver( T* receiver, unsigned int systemSize ) {
	if ( !InitializeWithSize( systemSize ) ) {
		return false;
	}

	m_messageReceiver = MessagingSystemReceiver<T>::CreateForObject( receiver, this );
	return ( m_messageReceiver != NULL );
}
/*
========
MessagingSystem::ProcessMessage

	Process a single message
========
*/
template <typename T>
void MessagingSystem<T>::ProcessMessage( BasicMessage* message ) {
	m_messageReceiver->ProcessMessage( message );
}
#pragma endregion MessagingSystems templated function implementation

#pragma region MESSAGINGSYSTEMRECEIVER
/*
========
MessagingSystemReceiver::MessagingSystemReceiver

	MessagingSystemReceiver default constructor.
========
*/
template <typename T>
MessagingSystemReceiver<T>::MessagingSystemReceiver( void ) :
	m_receiverObject( NULL ),
	m_messageSystem( NULL )
{}
/*
========
MessagingSystemReceiver::~MessagingSystemReceiver

	MessagingSystemReceiver destructor.
========
*/
template <typename T>
MessagingSystemReceiver<T>::~MessagingSystemReceiver( void ) {
	std::unordered_map<const RTTI*, BaseMessagingSystemCallback*>::iterator iter	= m_messageCallbacks.begin();
	std::unordered_map<const RTTI*, BaseMessagingSystemCallback*>::iterator iterEnd	= m_messageCallbacks.end();

	while ( iter != iterEnd ) {
		delete iter->second;
		++iter;
	}
}
/*
========
MessagingSystemReceiver::CreateForObject

	MessagingSystemReceiver creation.
========
*/
template <typename T>
MessagingSystemReceiver<T>* MessagingSystemReceiver<T>::CreateForObject( T* receiverObject, MessagingSystem<T>* messageSystem ) {
	MessagingSystemReceiver<T>* receiver = new MessagingSystemReceiver<T>();
	if ( receiver && receiver->InitializeForObject( receiverObject, messageSystem ) ) {
		return receiver;
	}

	delete receiver;
	return NULL;
}
/*
========
MessagingSystemReceiver::InitializeForObject

	MessagingSystemReceiver intialize for objects.
========
*/
template <typename T>
bool MessagingSystemReceiver<T>::InitializeForObject( T* receiverObject, MessagingSystem<T>* messageSystem ) {
	m_receiverObject	= receiverObject;
	m_messageSystem		= messageSystem;

	return ( m_receiverObject != NULL );
}
/*
========
MessagingSystemReceiver::RegisterMessageCallback

	Registers a message callback
========
*/
template <typename T>
template <typename R>
void MessagingSystemReceiver<T>::RegisterMessageCallback( void ( T::*messageCallbackFunc )( R* ) ) {
	if ( m_messageCallbacks.count( &R::rtti ) == 0 ) {
		MessagingSystemCallback<R>* callback	= new MessagingSystemCallback<R>( m_receiverObject, m_messageSystem, messageCallbackFunc );
		m_messageCallbacks[&R::rtti]			= callback;
	}
}
/*
========
MessagingSystemReceiver::ProcessMessage

	Process a message
========
*/
template <typename T>
void MessagingSystemReceiver<T>::ProcessMessage( BasicMessage* message ) {
	std::unordered_map<const RTTI*, BaseMessagingSystemCallback*>::iterator iter = m_messageCallbacks.find( &message->GetRTTI() );

	if ( iter != m_messageCallbacks.end() ) {
		iter->second->DispatchMessage( message );
	} else {
		std::string logMsg( "Message was not handled: " );
		logMsg += message->GetRTTI().GetClass();

		Log::GetInstance()->WriteToLog( "MessageSystem", logMsg.c_str() );
	}	
}

#pragma endregion MessagingSystemReceivers templated function implementation

}

#endif //__BaseMessagingSystem_H__