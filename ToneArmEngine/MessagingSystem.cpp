#include "MessagingSystem.h"
#include "MathHelper.h"
#include "VGSAssert.h"
#include "Log.h"
#include <sstream>

namespace vgs {
/*
=============
BaseMessagingSystem::BaseMessagingSystem

	BaseMessagingSystem default constructor.
=============
*/
BaseMessagingSystem::BaseMessagingSystem( void ) :
	m_allocatorIndex( 0 )
{
	m_msgAllocators[0] = m_msgAllocators[1] = NULL;
}
/*
=============
BaseMessagingSystem::~BaseMessagingSystem

	BaseMessagingSystem destructor.
=============
*/
BaseMessagingSystem::~BaseMessagingSystem( void ) {
	std::vector<BasicMessage*>::iterator iter = m_pendingMessages.begin();
	std::vector<BasicMessage*>::iterator end = m_pendingMessages.end();
	while ( iter != end ) {		
		FreeMessage<BasicMessage>( *iter );
		++iter;
	}

	delete m_msgAllocators[0];
	delete m_msgAllocators[1];
}
/*
=============
BaseMessagingSystem::InitializeWithSize

	Initializes the message system.
=============
*/
bool BaseMessagingSystem::InitializeWithSize( unsigned int bytes ) {
	unsigned int powerBytes = MathHelper::NextPowerOfTwo<unsigned int>( bytes );

	m_msgAllocators[0] = SmallBlockAllocator::CreateAllocatorWithSize( powerBytes );
	m_msgAllocators[1] = SmallBlockAllocator::CreateAllocatorWithSize( powerBytes );

	return ( m_msgAllocators[0] != NULL ) && ( m_msgAllocators[1] != NULL );
}
/*
=============
BaseMessagingSystem::SendMessage

	Sends a message through the system.
=============
*/
void BaseMessagingSystem::SendMessage( BasicMessage* msg ) {
	m_msgVectorMutex.lock();
	m_pendingMessages.push_back( msg );
	m_msgVectorMutex.unlock();
}
/*
=============
BaseMessagingSystem::ProcessMessages

	Processes all pending messages.
=============
*/
void BaseMessagingSystem::ProcessMessages( void ) {
	std::vector<BasicMessage*> pendingMessages;

	m_msgVectorMutex.lock();
	m_allocatorMutex.lock();

	pendingMessages.swap( m_pendingMessages );
	m_allocatorIndex = m_allocatorIndex == 0 ? 1 : 0;

	m_allocatorMutex.unlock();
	m_msgVectorMutex.unlock();

	std::vector<BasicMessage*>::iterator iter		= pendingMessages.begin();
	std::vector<BasicMessage*>::iterator iterEnd	= pendingMessages.end();

	while ( iter != iterEnd ) {
		ProcessMessage( ( *iter ) );
		++iter;
	}
}
}