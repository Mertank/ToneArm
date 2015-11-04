/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	SmallBlockAllocator

	Created by: Karl Merten
	Created on: 08/07/2014

========================
*/
#include "SmallBlockAllocator.h"

namespace vgs {
/*
===========
SmallBlockAllocator::SmallBlockAllocator

	SmallBlockAllocator default constructor
===========
*/
SmallBlockAllocator::SmallBlockAllocator( void ) :
	m_firstBlock( NULL ),
	m_memory( NULL ),
	m_allocatorSize( 0 ),
	m_headerSize( ( unsigned int )( std::ceil( ( float )sizeof( BlockHeader ) / MEMORY_ALIGNMENT ) * MEMORY_ALIGNMENT ) ) //Make sure it's memory aligned
#ifdef TONEARM_DEBUG
	,
	m_spaceUsed( 0 )
#endif
{}
/*
===========
SmallBlockAllocator::SmallBlockAllocator

	SmallBlockAllocator destructor
===========
*/
SmallBlockAllocator::~SmallBlockAllocator( void ) {
	delete[] m_memory;
}
/*
===========
SmallBlockAllocator::CreateAllocatorWithSize

	Creates an allocator with the required available memory.
===========
*/
SmallBlockAllocator* SmallBlockAllocator::CreateAllocatorWithSize( unsigned int allocatorSize ) {
	SmallBlockAllocator* allocator = new SmallBlockAllocator();
	if ( allocator && allocator->InitializeAllocatorWithSize( allocatorSize ) ) {
		return allocator;
	} else {
		delete allocator;
		return NULL;
	}
}
/*
===========
SmallBlockAllocator::InitializeAllocatorWithSize

	Initializes the allocator.
===========
*/
bool SmallBlockAllocator::InitializeAllocatorWithSize( unsigned int allocatorSize ) {
	if ( !allocatorSize || ( allocatorSize & ( allocatorSize - 1 ) ) ) {
		printf( "SmallBlockAllocator size must be a power of 2" );
		return false;
	}

	m_allocatorSize = allocatorSize;
	m_memory		= new char[allocatorSize];
	m_firstBlock	= ( MemoryBlock* )m_memory;
	
	m_firstBlock->header.status = FREE;
	m_firstBlock->header.blockSize = allocatorSize - m_headerSize;

	return true;
}
/*
===========
SmallBlockAllocator::GetBlockWithSize

	Finds a block to hold that size class.
	Returns NULL if it cannot find one.
===========
*/
SmallBlockAllocator::MemoryBlock* SmallBlockAllocator::GetBlockWithSize( unsigned int size ) {
	unsigned int memoryAlignedSize = ( unsigned int )( std::ceil( ( float )size / MEMORY_ALIGNMENT ) * MEMORY_ALIGNMENT );

	MemoryBlock* possibleBlock = m_firstBlock;	
	
	while ( possibleBlock ) {
		if ( possibleBlock->header.status == FREE && possibleBlock->header.blockSize >= memoryAlignedSize ) {
			unsigned int splitSize = ( unsigned int )( ( possibleBlock->header.blockSize - m_headerSize ) * 0.5f );	//Calculate new size
			while ( splitSize >= memoryAlignedSize ) { //We can split it, and still have room
				possibleBlock->header.blockSize = splitSize;

				MemoryBlock* nextBlock = ( MemoryBlock* )( ( char* )possibleBlock + ( splitSize + m_headerSize ) );
				nextBlock->header.status = FREE;
				nextBlock->header.blockSize = splitSize;

				splitSize = ( unsigned int )( ( possibleBlock->header.blockSize - m_headerSize ) * 0.5f );	//Calculate new size
			}

			return possibleBlock;
		} else {
			possibleBlock = ( MemoryBlock* )( ( char* )possibleBlock + ( m_headerSize + possibleBlock->header.blockSize ) );

			if ( ( char* )possibleBlock >= ( ( char* )m_firstBlock + m_allocatorSize ) ) { //Out of our bounds. No space
				break;
			}
		}		
	}

	return NULL;
}
/*
===========
SmallBlockAllocator::GetBuddy

	Returns the buddy of a block.
===========
*/
SmallBlockAllocator::MemoryBlock* SmallBlockAllocator::GetBuddy( SmallBlockAllocator::MemoryBlock* orig ) {
	return ( MemoryBlock* )( ( char* )m_firstBlock + ( ( ( char* )orig - ( char* )m_firstBlock ) ^ ( orig->header.blockSize + m_headerSize ) ) );
}
/*
===========
SmallBlockAllocator::ContainsObject

	Returns whether or not the object is in this pool.
===========
*/
bool SmallBlockAllocator::ContainsObject( void* obj ) {
	return ( obj >= ( char* )m_firstBlock ) && ( obj < ( char* )m_firstBlock + m_allocatorSize );
}
}