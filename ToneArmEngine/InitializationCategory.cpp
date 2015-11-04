/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	InitializationCategory

	Created by: Karl Merten
	Created on: 24/03/2014

========================
*/
#include "InitializationCategory.h"
#include <vector>
#include "StringUtils.h"
#include "VGSMacro.h"

namespace vgs {
/*
========
InitializationCategory::InitializationCategory

	InitializationCategory default constructor
========
*/
InitializationCategory::InitializationCategory( void ) :
	m_groupTitle( NULL )
{}
/*
========
InitializationCategory::~InitializationCategory

	InitializationCategory destructor
========
*/
InitializationCategory::~InitializationCategory( void ) {
	delete[] m_groupTitle;

	for ( std::map<char*, char*>::iterator key = m_values.begin();
		  key != m_values.end(); ++key ) {
		delete[] ( *key ).first;
		delete[] ( *key ).second;
	}
}
/*
========
InitializationCategory::InitializeWithTitle

	Creates an InitializationCategory
========
*/
InitializationCategory* InitializationCategory::CreateWithTitle( const char* title, unsigned int length ) {
	InitializationCategory* retPtr = new InitializationCategory();
	if ( retPtr && retPtr->InitializeWithTitle( title, length ) ) {
		return retPtr;
	} else {
		delete retPtr;
		return NULL;
	}
}
/*
========
InitializationCategory::InitializeWithTitle

	Initializes the InitializationCategory with a title
========
*/
bool InitializationCategory::InitializeWithTitle( const char* title, unsigned int length ) {
	m_groupTitle = new char[length + 1];
	memcpy( m_groupTitle, title, length );
	m_groupTitle[length] = '\0'; //NULL terminate

	return true;
}
/*
========
InitializationCategory::AddKeyValue

	Adds a key/value pair to the map.
========
*/
void InitializationCategory::AddKeyValue( const char* key, unsigned int keyLength, const char* value, unsigned int valueLength ) {
	char* keyPtr = new char[keyLength + 1];
	memcpy( keyPtr, key, keyLength );
	keyPtr[keyLength] = '\0';

	char* valuePtr = new char[valueLength + 1];
	memcpy( valuePtr, value, valueLength );
	valuePtr[valueLength] = '\0';

	m_values[keyPtr] = valuePtr;
}
/*
========
InitializationCategory::ValueForKey

	Returns the value for the key.
	NULL if the key doesn't exist.
========
*/
const char* InitializationCategory::ValueForKey( const char* keyStr ) const {
	for ( std::map<char*, char*>::const_iterator key = m_values.begin();
		  key != m_values.end(); ++key ) {
		if ( strcmp( keyStr, ( *key ).first ) == 0 ) {
			return ( *key ).second;
		}
	}

	return NULL;
}
/*
========
InitializationCategory::GetIntValueForKey

	Returns the boolean value for a key.
	Guarentees a value even if the key doesn't exist.
========
*/
bool InitializationCategory::GetBooleanValueForKey( const char* key ) const
{
	const char* value = ValueForKey( key );
	if ( value ) 
	{
		return strcmp(value, "true") == 0;
	} 
	else
	{
		return false;
	}
}
/*
========
InitializationCategory::GetIntValueForKey

	Returns the int value for a key.
	Guarentees a value even if the key doesn't exist.
========
*/
int InitializationCategory::GetIntValueForKey( const char* key ) const {
	const char* value = ValueForKey( key );
	if ( value ) {
		return atoi( value );
	} else {
		return 0;
	}
}
/*
========
InitializationCategory::GetFloatValueForKey

	Returns the float value for a key.
	Guarentees a value even if the key doesn't exist.
========
*/
float InitializationCategory::GetFloatValueForKey( const char* key ) const {
	const char* value = ValueForKey( key );
	if ( value ) {
		return ( float )atof( value );
	} else {
		return 0.0f;
	}
}
/*
========
InitializationCategory::GetStringValueForKey

	Returns the float value for a key.
	Returns NULL if the key doesn't exist.
========
*/
const char* InitializationCategory::GetStringValueForKey( const char* key ) const {
	const char* value = ValueForKey( key );
	if ( value ) {
		return value;
	} else {
		return NULL;
	}
}
/*
========
InitializationCategory::GetVec2ValueForKey

	Returns the vec2 value for a key.
	Guarentees a value even if the key doesn't exist.
========
*/
glm::vec2 InitializationCategory::GetVec2ValueForKey( const char* key ) const {
	const char* value = ValueForKey( key );
	glm::vec2 retVec( 0.0f, 0.0f );

	if ( value ) {
		std::vector<char*>values;
		StringUtils::Tokenize( value, ',', values );
		unsigned int index = 0;

		BEGIN_STD_VECTOR_ITERATOR( char*, values );
			if ( index < 2 ) {
				retVec[index] = ( float )atof( currentItem );
			}
			++index;
			delete[] currentItem;
		END_STD_VECTOR_ITERATOR
	} 

	return retVec;
}
/*
========
InitializationCategory::GetVec3ValueForKey

	Returns the vec3 value for a key.
	Guarentees a value even if the key doesn't exist.
========
*/
glm::vec3 InitializationCategory::GetVec3ValueForKey( const char* key ) const {
	const char* value = ValueForKey( key );
	glm::vec3 retVec( 0.0f, 0.0f, 0.0f );

	if ( value ) {
		std::vector<char*>values;
		StringUtils::Tokenize( value, ',', values );
		unsigned int index = 0;

		BEGIN_STD_VECTOR_ITERATOR( char*, values );
			if ( index < 3 ) {
				retVec[index] = ( float )atof( currentItem );
			}
			++index;
			delete[] currentItem;
		END_STD_VECTOR_ITERATOR
	} 

	return retVec;
}
/*
========
InitializationCategory::GetVec4ValueForKey

	Returns the vec4 value for a key.
	Guarentees a value even if the key doesn't exist.
========
*/
glm::vec4 InitializationCategory::GetVec4ValueForKey( const char* key ) const {
	const char* value = ValueForKey( key );
	glm::vec4 retVec( 0.0f, 0.0f, 0.0f, 0.0f );

	if ( value ) {
		std::vector<char*>values;
		StringUtils::Tokenize( value, ',', values );
		unsigned int index = 0;

		BEGIN_STD_VECTOR_ITERATOR( char*, values );
			if ( index < 4 ) {
				retVec[index] = ( float )atof( currentItem );
			}
			++index;
			delete[] currentItem;
		END_STD_VECTOR_ITERATOR
	} 

	return retVec;
}
}