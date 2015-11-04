/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Program

		A program with all of it's uniform attributes.
		Keeps track of current values to avoid redundant state changes.

		Created by: Karl Merten
		Created on: 23/06/2014

========================
*/
#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <map>
#include <unordered_map>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include "OpenGLProgramManager.h"

namespace vgs {

class OpenGLProgram {
	friend class OpenGLProgramManager;
public:
									~OpenGLProgram( void );

	inline GLuint					GetProgramId( void ) const { return m_programID; }
	inline bool						IsReady( void ) const { return m_programReady; }

	GLuint							GetAttributeLocation( const char* name );
	GLint							GetUniformLocation( const char* name );
	
	ProgramFeatures::Value			GetFeatures( void ) { return m_features; }
	void							SetFeatures( ProgramFeatures::Value features ) { m_features = features; }

	bool							HasShader( ShaderType::Value shader );

	void							SetUniform( const char* name, const glm::vec3& value );
	void							SetUniform( const char* name, const glm::vec4& value );
	void							SetUniform( const char* name, const glm::mat3& value );
	void							SetUniform( const char* name, const glm::mat4& value );

	void							SetUniform( const char* name, const float* value, unsigned size );
	void							SetUniform( const char* name, const int* value, unsigned size );
	void							SetUniform( const char* name, const float value );
	void							SetUniform( const char* name, const int value );

	void							SetProgramName( const char* newName );
	void							SetOutVariable( const char* newName );

	GLuint							AttachShader( Shader* shaderInfo );
	void							LinkProgram( void );

	void                            Use( void );									
private:
    /*
    ========================

	    Uniform

		    Helper struct to store current value on CPU side.
            This will avoid redundant state changes

    ========================
    */
    struct Uniform {
    private:
	    void*		currentValues;	

    public:
	    unsigned 	size;
	    GLint		location;
	    std::string name;

	    Uniform( GLint location, const char* name, unsigned valueSize ) :
		    location( location ),
		    name( name ),
		    size( valueSize )
	    {
		    currentValues = new float[valueSize];
		    memset( currentValues, 0, valueSize * sizeof( float ) );
	    }

		~Uniform( void ) {
			delete[] currentValues;
		}
        /*
        ==============
	        Set

		        Will set float values.
        ==============
        */
	    void Set( const float* newValue ) {
			if ( memcmp( currentValues, newValue, sizeof( float ) * size ) != 0 ) {
				if ( size == 1 ) {
					glUniform1fv( location, 1, &newValue[0] );
				} else if ( size == 2 ) {
					glUniform2fv( location, 1, &newValue[0] );
				} else if ( size == 3 ) {
					glUniform3fv( location, 1, &newValue[0] );
				} else if ( size == 4 ) {
					glUniform4fv( location, 1, &newValue[0] );
				} else if ( size == 12 ) {
					glUniformMatrix3fv( location, 1, GL_FALSE, &newValue[0] );
				} else if ( size == 16 ) {
					glUniformMatrix4fv( location, 1, GL_FALSE, &newValue[0] );
				}

				memcpy( currentValues, newValue, size * sizeof( float ) );
			}
	    }
        /*
        ==============
	        Set

		        Will set int values.
        ==============
        */
        void Set( const int* newValue ) {
		   if ( memcmp( currentValues, newValue, sizeof( int ) * size ) != 0 ) {
			    if ( size == 1 ) {
					glUniform1i( location, *newValue );
				} else if ( size == 2 ) {
					glUniform2iv( location, 1, &newValue[0] );
				} else if ( size == 3 ) {
					glUniform3iv( location, 1, &newValue[0] );
				} else if ( size == 4 ) {
					glUniform4iv( location, 1, &newValue[0] );
				} 

				memcpy( currentValues, newValue, size * sizeof( int ) );
		    }
	    }
    };
									OpenGLProgram( void );

	static OpenGLProgram*			CreateProgram( const char* name, const std::vector<Shader*>& shaderList, const char* outVariable, ProgramFeatures::Value features );

	bool							InitializeProgram( const char* name, const std::vector<Shader*>& shaderList, const char* outVariable, ProgramFeatures::Value features );		
	
	GLuint							CompileShader( const char* path, GLenum shaderType );
	GLuint							CompileShader( const char* buffer, unsigned int length, GLenum shaderType );
    
	Uniform*                        GetCachedUniform( const char* name );

	GLint							m_programID;
	char*							m_programName;
	bool							m_programReady;
	ShaderType::Value				m_shadersAvailable;

	ProgramFeatures::Value			m_features;

	typedef std::unordered_map<unsigned int, Uniform*> Uniforms;
    Uniforms						m_uniforms;  

	std::map<unsigned long, GLuint> m_attributes;
};

}

#endif __PROGRAM_H__