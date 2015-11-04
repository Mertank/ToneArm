/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RTTI

		Class for runtime type information.

		Created by: Karl Merten
		Created on: 09/05/2014

========================
*/

#ifndef __RTTI_H__
#define __RTTI_H__

#include <cstdlib>

#define DECLARE_RTTI													\
	public:																\
		virtual const vgs::RTTI& GetRTTI( void ) const { return rtti; }	\
		static const vgs::RTTI rtti;									\

#define IMPLEMENT_BASE_RTTI( name )										\
	const vgs::RTTI name::rtti( #name );								\

#define IMPLEMENT_RTTI( name, parent )									\
	const vgs::RTTI name::rtti( #name, parent::rtti );					\

namespace vgs {

class RTTI {
public:
	RTTI( const char* className ) :
		m_className( className ),
		m_baseRTTI( NULL )
	{}

	RTTI( const char* className, const RTTI& baseRTTI ) :
		m_className( className ),
		m_baseRTTI( &baseRTTI )
	{}

	~RTTI( void ) 
	{}

	const char* GetClass( void ) const { return m_className; }

	bool DerivesFrom( const RTTI& other ) const {
		const RTTI* comp = this;

		while ( comp ) {
			if ( comp == &other ) {
				return true;
			}

			comp = comp->m_baseRTTI;
		}

		return false;
	}

	bool operator==( const RTTI& other ) const {
		return ( this == &other );
	}

	bool operator!=( const RTTI& other ) const {
		return !( *this == other );
	}

private:
	const char*		m_className;
	const RTTI*		m_baseRTTI;
};

}

#endif //__RTTI_H__