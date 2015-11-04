/*
========================

Copyright (c) 2014 Vinyl Games Studio

	InitializationCategory

		A group of values in an .ini file.

		Created by: Karl Merten
		Created on: 24/03/2014

========================
*/

#ifndef __INITIALIZATIONCATEGORY_H__
#define __INITIALIZATIONCATEGORY_H__

#include <map>
#include <glm\glm.hpp>

namespace vgs {

class InitializationCategory {
public:
									~InitializationCategory( void );
	static InitializationCategory*	CreateWithTitle( const char* title, unsigned int length );
	
	void							SetTitle( const char* title, unsigned int length );
	inline const char*				GetTitle( void ) const { return m_groupTitle; }

	bool							GetBooleanValueForKey( const char* key ) const;
	int								GetIntValueForKey( const char* key ) const;
	float							GetFloatValueForKey( const char* key ) const;
	const char*						GetStringValueForKey( const char* key ) const;

	glm::vec2						GetVec2ValueForKey( const char* key ) const;
	glm::vec3						GetVec3ValueForKey( const char* key ) const;
	glm::vec4						GetVec4ValueForKey( const char* key ) const;

	void							AddKeyValue( const char* key, unsigned int keyLength, const char* value, unsigned int valueLength );
private:
									InitializationCategory( void );
	bool							InitializeWithTitle( const char* title, unsigned int length );	
	const char*						ValueForKey( const char* keyStr ) const;

	char*							m_groupTitle;
	std::map<char*, char*>			m_values;
};

}

#endif //__INITIALIZATIONCATEGORY_H__