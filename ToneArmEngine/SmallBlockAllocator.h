/*
========================

Copyright (c) 2014 Vinyl Games Studio

	SmallBlockAllocator

		A small block allocator.
		It uses the buddy memory system.
		This class is NOT thread safe. Do it yourself.

		Created by: Karl Merten
		Created on: 08/07/2014

========================
*/
#ifndef __SMALLBLOCKALLOCATOR_H__
#define __SMALLBLOCKALLOCATOR_H__

#define MEMORY_ALIGNMENT	sizeof( void* )

#include <vector>
#include <utility>

namespace vgs {

class SmallBlockAllocator {
public:
								~SmallBlockAllocator( void );

	static SmallBlockAllocator*	CreateAllocatorWithSize( unsigned int allocatorSize );

	template <typename T>
	T*							Alloc( void );	
	template <typename T>
	void						Free( T* data );

	bool						ContainsObject( void* obj );
private:
	struct MemoryBlock;	
								SmallBlockAllocator( void );

	bool						InitializeAllocatorWithSize( unsigned int allocatorSize );
	MemoryBlock*				GetBlockWithSize( unsigned int size );
	MemoryBlock*				GetBuddy( MemoryBlock* orig );

	enum MemoryStatus : char {
		FREE	= 0x00,
		USED	= 0x01
	};

	struct BlockHeader {
		MemoryStatus	status;
		unsigned short	blockSize;

		BlockHeader( void ) :
			status( FREE ),
			blockSize( 0 )
		{}
	};

	struct MemoryBlock {
		BlockHeader		header;
	};


	MemoryBlock*				m_firstBlock;
	char*						m_memory;
	unsigned short				m_headerSize;
	unsigned int				m_allocatorSize;

#ifdef TONEARM_DEBUG
	unsigned int				m_spaceUsed;
#endif
};
/*
===========
SmallBlockAllocator::Alloc

	Allocates a pointer to the object.
	Will pass it to the OS if none are found.
===========
*/
template <typename T>
T* SmallBlockAllocator::Alloc( void ) {
	MemoryBlock* block = GetBlockWithSize( sizeof( T ) );
	if ( block ) {
		block->header.status = USED;		
		//memset( ( char* )block + m_headerSize, 0, block->header.blockSize );

		//Call constructor
		new ( ( char* )block + m_headerSize )T();
#ifdef TONEARM_DEBUG
		m_spaceUsed += ( block->header.blockSize + m_headerSize );
#endif
		return ( T* )( ( char* )block + m_headerSize );
	} else {
#ifdef TONEARM_DEBUG
		unsigned int memoryAlignedSize = ( unsigned int )( std::ceil( ( float )sizeof( T ) / MEMORY_ALIGNMENT ) * MEMORY_ALIGNMENT );
		//printf( "Failed to find memory for %i byte ( actual size:%i ) allocation\nAllocator using %i/%i bytes\n", sizeof( T ), memoryAlignedSize + m_headerSize, m_spaceUsed, m_allocatorSize );
		GetBlockWithSize( sizeof( T ) );
#endif
		return new T();
	}
}
/*
===========
SmallBlockAllocator::Free

	Frees the pointer
===========
*/
template <typename T>
void SmallBlockAllocator::Free( T* data ) {
	MemoryBlock* block = ( MemoryBlock* )( ( unsigned int )data - m_headerSize ); //Offset some memory to get the actual block.
	if ( block < m_firstBlock || ( char* )block > ( ( char* )m_firstBlock + m_allocatorSize ) ) {
		delete data;
	} else {
		
		data->~T(); //Call destructor
		block->header.status = FREE; //Set memory as free
#ifdef TONEARM_DEBUG
		m_spaceUsed -= ( block->header.blockSize + m_headerSize );
#endif
		
		//Combine memory together again, as required.
		MemoryBlock* buddyBlock = GetBuddy( block );

		while ( buddyBlock->header.status == FREE && buddyBlock->header.blockSize == block->header.blockSize ) {
			if ( buddyBlock < block ) {
				block = buddyBlock;	
			}

			block->header.blockSize = block->header.blockSize * 2 + m_headerSize;
			buddyBlock = GetBuddy( block );
			
			if ( ( unsigned int )( block->header.blockSize + m_headerSize ) >= m_allocatorSize ) { //No more, done merging.
				break;
			}
		}
	}
}

}

#endif //__SMALLBLOCKALLOCATOR_H__