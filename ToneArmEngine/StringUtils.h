#ifndef __STRINGUTILS_H__
#define __STRINGUTILS_H__

#include <vector>
#include <cstring>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <vector>

namespace vgs {

class StringUtils {
public:
	/*
	=========
	StringUtils::CopyCString

		Returns a copy of a c-style string.
		You must delete it.
	=========
	*/
	static char* CopyCString( const char* src ) {
		unsigned int strLen = strlen( src );
		char* buffer = new char[strLen + 1];
		memset( buffer, 0, strLen + 1 );
		memcpy( buffer, src, strLen );
		return buffer;
	}
	/*
	=========
	StringUtils::CopyCString

		Returns a copy of a c-style string with a set length.
		You must delete it.
	=========
	*/
	static char* CopyCString( const char* src, unsigned int length ) {
		char* buffer = new char[length + 1];
		memset( buffer, 0, length + 1 );
		memcpy( buffer, src, length );
		return buffer;
	}
	/*
	=========
	StringUtils::IsEqual

		Compares to c-style strings.
		Returns if they match or not.
	=========
	*/
	static bool IsEqual( const char* str1, const char* str2 ) {
		unsigned int strLen = strlen( str1 );			
		if ( strLen != strlen( str2 ) ) { return false; }

		unsigned int index = 0;
		while ( index <= strLen ) {
			if ( str1[index] != str2[index] ) {
				return false;
			}
			++index;
		}
		return true;
	}
	/*
	=========
	StringUtils::IsEqual

		Compares to c-style strings for a certain length.
		Returns if they match or not.
	=========
	*/
	static bool IsEqual( const char* str1, const char* str2, unsigned int length ) {
		unsigned int str1Len = strlen( str1 );			
		unsigned int str2Len = strlen( str2 );			

		if ( str1Len < length || str2Len < length ) {
			return false;
		}

		unsigned int index = 0;
		while ( index < length ) {
			if ( str1[index] != str2[index] ) {
				return false;
			}
			++index;
		}
		return true;
	}
	/*
	=========
	StringUtils::Length

		Returns the length of a string.
		Assumes it is null terminated.
	=========
	*/
	static unsigned int StringUtils::Length( char* str ) {
		return strlen( str );
	}
	/*
	=========
	StringUtils::Hash

		Hashes a string using djb2
		http://www.cse.yorku.ca/~oz/hash.html
	=========
	*/
	static const unsigned long Hash( const char* str ) {
		unsigned long hash = 5381;
		int c = *str;

		while ( c ) {
			hash = ( ( hash << 5 ) + hash ) + c; /* hash * 33 + c */
			++str;
			c = *str;
		}

		return hash;
	}
	/*
	=========
	StringUtils::Tokenize

		Tokenizes a string.
		You MUST delete the char* in the vector yourself when finished.
	=========
	*/
	static void Tokenize( const char* str, char token, std::vector<char*>& dest ) {
		unsigned int currentIndex = 0;
		unsigned int strLen = strlen( str );
		while ( currentIndex <= strLen ) {
			unsigned int indexOfToken = currentIndex;

			//Search for the index of the token
			while ( str[indexOfToken] != token ) {
				if ( !str[indexOfToken] ) { break; } //Reached end of the string
				++indexOfToken;
			}

			dest.push_back( CopyCString( &str[currentIndex], ( indexOfToken - currentIndex ) ) ); //Add token
			currentIndex = indexOfToken + 1;
		}
	}
	/*
	=========
	StringUtils::ToVec2

		Turns a string into a vec2.
	=========
	*/
	static void ToVec2( char* buffer, char* delimiter, glm::vec2& vector ) {
		char* nextToken = NULL;

		vector.s = ( float )std::atof( strtok_s( buffer, delimiter, &nextToken ) );
		vector.t = ( float )std::atof( strtok_s( NULL  , delimiter, &nextToken ) );
	}
	/*
	=========
	StringUtils::ToVec3

		Turns a string into a vec3.
	=========
	*/
	static void ToVec3( char* buffer, char* delimiter, glm::vec3& vector ) {
		char* nextToken = NULL;

		vector.x = ( float )std::atof( strtok_s( buffer, delimiter, &nextToken ) );
		vector.y = ( float )std::atof( strtok_s( NULL  , delimiter, &nextToken ) );
		vector.z = ( float )std::atof( strtok_s( NULL  , delimiter, &nextToken ) );
	}
	/*
	=========
	StringUtils::ToQuat

		Turns a string into a quaternion.
		Calculates w value.
	=========
	*/
	static void ToQuat( char* buffer, char* delimiter, glm::quat& quat ) {
		char* nextToken = NULL;

		quat.x = ( float )std::atof( strtok_s( buffer, " ", &nextToken ) );
		quat.y = ( float )std::atof( strtok_s( NULL  , " ", &nextToken ) );
		quat.z = ( float )std::atof( strtok_s( NULL  , " ", &nextToken ) );    

		float t = 1.0f - ( quat.x * quat.x ) - ( quat.y * quat.y ) - ( quat.z * quat.z );
		if ( t < 0.0f ) {
			quat.w = 0.0f;
		} else {
			quat.w = sqrtf( t );
		}
	}
	/*
	=========
	StringUtils::ToFloat

		Turns a 4 chars into a float
	=========
	*/
	static void ToFloat( char* buffer, float& dest ) {
		memcpy( &dest, buffer, sizeof( float ) );
	}
	/*
	=========
	StringUtils::ToULong

		Turns a 4 chars into an unsigned long
	=========
	*/
	static void ToULong( char* buffer, unsigned long long& dest ) {
		memcpy( &dest, buffer, sizeof( unsigned long long ) );
	}
};

}

#endif //__STRINGUTILS_H__