/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Program

	A program with all of it's uniform attributes.
	Keeps track of current values to avoid redundant state changes.

========================
*/
#include "OpenGLProgram.h"
#include "Log.h"
#include "CachedResourceLoader.h"
#include "BinaryFileResource.h"
#include "StringUtils.h"
#include "PathUtils.h"
#include "Engine.h"

namespace vgs {
/*
=============
OpenGLProgram::OpenGLProgram

	OpenGLProgram Constructor.
=============
*/
OpenGLProgram::OpenGLProgram( void ) :
	m_programID( -1 ),
	m_programName( NULL ),
	m_shadersAvailable( ShaderType::NO_SHADER ),
	m_programReady( false ),
	m_features( ProgramFeatures::NONE )
{}
/*
=============
OpenGLProgram::~OpenGLProgram

	OpenGLProgram Destructor.
=============
*/
OpenGLProgram::~OpenGLProgram( void ) {
	for ( std::unordered_map<unsigned int, Uniform*>::iterator iter = m_uniforms.begin(); iter != m_uniforms.end(); ++iter ) {
		delete iter->second;
	}

	m_attributes.clear();
	
	delete[] m_programName;

	glDeleteProgram( m_programID );
    glUseProgram( 0 );
}
/*
=============
OpenGLProgram::OpenGLProgram

	Creates a shader program.
=============
*/
OpenGLProgram* OpenGLProgram::CreateProgram( const char* name, const std::vector<Shader*>& shaderList, const char* outVariable, ProgramFeatures::Value features ) {
	OpenGLProgram* program = new OpenGLProgram();
    
	if ( program == NULL ||	!program->InitializeProgram( name, shaderList, outVariable, features ) ) {
		delete program;		
		return NULL;
	}

	return program;
}
/*
=============
OpenGLProgram::InitializeProgram

	OpenGLProgram Initialization.
=============
*/
bool OpenGLProgram::InitializeProgram( const char* name, const std::vector<Shader*>& shaderList, const char* outVariable, ProgramFeatures::Value features ) {
	m_programID = glCreateProgram();
	SetProgramName( name );

	for( std::vector<Shader*>::const_iterator iter = shaderList.begin(); iter != shaderList.end(); ++iter ) { 
		if( AttachShader( ( *iter ) ) == 0 ) {
			char buff[256];
			sprintf_s( buff, "Failed to compile %s program", name );
			Log::GetInstance()->WriteToLog( "OpenGLProgram", buff );

			return false;
		}
	}
		
	SetOutVariable( outVariable );
	LinkProgram();
	SetFeatures( features );

	return m_programID > 0;
}
/*
=============
OpenGLProgram::CompileShader

	Compiles the shader from a file.
=============
*/
GLuint OpenGLProgram::CompileShader( const char* path, GLenum shaderType ) {
	std::shared_ptr<BinaryFileResource> shader = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>( EngineModuleType::RESOURCELOADER )->LoadResource<BinaryFileResource>( path );
	
	if ( !shader.get() ) { 
		char buffer[255];
		memset( buffer, 0, 255 );
		sprintf_s( buffer, 255, "Could not find shader at path %s", path );
		Log::GetInstance()->WriteToLog( "OpenGLProgram", buffer );
		return 0; 
	}
	
	char* fileData = shader->ReadFile();
	
	GLuint shaderHandle = CompileShader( fileData, ( unsigned int )shader->GetFileLength(), shaderType );

	delete[] fileData;

	return shaderHandle;
}
/*
=============
OpenGLProgram::CompileShader

	Compiles the shader from a buffer
=============
*/
GLuint OpenGLProgram::CompileShader( const char* buffer, unsigned int length, GLenum shaderType ) {
	GLuint shaderHandle = glCreateShader( shaderType );
	
	glShaderSource( shaderHandle, 1, ( const GLchar** )&buffer, ( GLint* )&length );
	glCompileShader( shaderHandle );

	GLint status;
	glGetShaderiv( shaderHandle, GL_COMPILE_STATUS, &status );
	
	if ( status != GL_TRUE ) {
		char buffer[512];
		glGetShaderInfoLog( shaderHandle, 512, NULL, buffer );

		Log::GetInstance()->WriteToLog( "OpenGLProgram", buffer );
		return 0;
	}

	return shaderHandle;
}
/*
=============
OpenGLProgram::Use

	Uses the program.
=============
*/
void OpenGLProgram::Use( void ) {
	if ( m_programReady ) {
		glUseProgram( m_programID );
	} else {
		char buffer[255];
		sprintf_s( buffer, "Program: %s was not linked successfully before use", m_programName );
		Log::GetInstance()->WriteToLog( "OpenGLProgram", buffer );
	}
}
/*
=============
OpenGLProgram::AttachShader

	Attachs a shader to the program.
=============
*/
GLuint OpenGLProgram::AttachShader( Shader* shaderInfo ) {
	GLuint shaderHandle		= 0;
	GLenum shaderEnum		= GL_NONE;

	if ( shaderInfo->shaderType == ShaderType::VERTEX_SHADER ) {
		m_shadersAvailable = ( ShaderType::Value )( m_shadersAvailable | ShaderType::VERTEX_SHADER );
		shaderEnum = GL_VERTEX_SHADER;
	} else if ( shaderInfo->shaderType == ShaderType::FRAGMENT_SHADER ) {
		m_shadersAvailable = ( ShaderType::Value )( m_shadersAvailable | ShaderType::FRAGMENT_SHADER );
		shaderEnum = GL_FRAGMENT_SHADER;
	} else if ( shaderInfo->shaderType == ShaderType::GEOMETRY_SHADER ) {
		m_shadersAvailable = ( ShaderType::Value )( m_shadersAvailable | ShaderType::GEOMETRY_SHADER );
		shaderEnum = GL_GEOMETRY_SHADER;
	} 

	if ( shaderInfo->shaderLength == 0 ) {
		shaderHandle = CompileShader( shaderInfo->shaderLocation, shaderEnum );
	} else {
		shaderHandle = CompileShader( shaderInfo->shaderLocation, shaderInfo->shaderLength, shaderEnum );
	}

	if ( shaderHandle > 0 ) {
		glAttachShader( m_programID, shaderHandle );
		m_shadersAvailable = ( ShaderType::Value )( m_shadersAvailable | shaderInfo->shaderType );
	} else {
		char buffer[512];
		sprintf_s( buffer, "Failled to attach %s shader to program", ( ( shaderInfo->shaderType == ShaderType::VERTEX_SHADER ) ? "vertex" : 
																     ( shaderInfo->shaderType == ShaderType::FRAGMENT_SHADER ) ? "fragment" : "geometry" ) );
		Log::GetInstance()->WriteToLog( "OpenGLProgram", buffer );
	}

	glDeleteShader( shaderHandle );

	return shaderHandle;
}
/*
=============
OpenGLProgram::HasShader

	Attachs a shader to the program.
=============
*/
bool OpenGLProgram::HasShader( ShaderType::Value shaderType ) {
	return ( ShaderType::Value )( shaderType & m_shadersAvailable ) == shaderType;
}
/*
=============
OpenGLProgram::LinkProgram

	Links the program, and sets it as ready to use.
=============
*/
void OpenGLProgram::LinkProgram( void ) {
	glLinkProgram( m_programID );

	GLint status;
	glGetProgramiv( m_programID, GL_LINK_STATUS, &status );
	
	if ( status != GL_TRUE ) {
		char buffer[512];
		glGetProgramInfoLog( m_programID, 512, NULL, buffer );

		Log::GetInstance()->WriteToLog( "OpenGLProgram", buffer );
		return;
	}

	m_programReady = true;
}
/*
=============
OpenGLProgram::SetUniform

	Sets the a uniform float value.
=============
*/
void OpenGLProgram::SetUniform( const char* name, const float* value, unsigned size ) {	
	Uniform* cachedUniform = GetCachedUniform( name );
    if ( cachedUniform ) {
        cachedUniform->Set( value );
    } else {
	    //Not found
        GLint location = glGetUniformLocation( m_programID, ( const GLchar* )name );
	    if ( location > -1 ) {
		    Uniform* newUniform = new Uniform( location, name, size );
		    newUniform->Set( value );
        
	        m_uniforms[StringUtils::Hash( name )] = ( newUniform );
	    } else {
			printf( "Could not find float uniform: %s\n", name );
		}
    }
}
/*
=============
OpenGLProgram::SetUniform

	Sets a uniform int value.
=============
*/
void OpenGLProgram::SetUniform( const char* name, const int* value, unsigned size ) {	
	Uniform* cachedUniform = GetCachedUniform( name );
    if ( cachedUniform ) {
        cachedUniform->Set( value );
    } else {
	    //Not found
        GLint location = glGetUniformLocation( m_programID, name );
	    if ( location > -1 ) {
		    Uniform* newUniform = new Uniform( location, name, size );
		    newUniform->Set( value );
        
            m_uniforms[StringUtils::Hash( name )] = ( newUniform );
	    } else {
			printf( "Could not find int uniform: %s\n", name );
        }
    }
}
/*
=============
OpenGLProgram::SetUniform

	Sets a single int uniform value
=============
*/
void OpenGLProgram::SetUniform( const char* name, const int value ) {	
	SetUniform( name, &value, 1 );
}
/*
=============
OpenGLProgram::SetUniform

	Sets a single float uniform value
=============
*/
void OpenGLProgram::SetUniform( const char* name, const float value ) {	
	SetUniform( name, &value, 1 );
}
/*
=============
OpenGLProgram::SetUniform

	Sets a uniform vec3 value.
=============
*/
void OpenGLProgram::SetUniform( const char* name, const glm::vec3& value ) {	
	SetUniform( name, &value[0], 3 );
}
/*
=============
OpenGLProgram::SetUniform

	Sets a uniform vec4 value.
=============
*/
void OpenGLProgram::SetUniform( const char* name, const glm::vec4& value ) {	
	SetUniform( name, &value[0], 4 );
}
/*
=============
OpenGLProgram::SetUniform

	Sets a uniform mat3 value.
=============
*/
void OpenGLProgram::SetUniform( const char* name, const glm::mat3& value ) {	
	SetUniform( name, &value[0][0], 12 );
}
/*
=============
OpenGLProgram::SetUniform

	Sets a uniform mat4 value.
=============
*/
void OpenGLProgram::SetUniform( const char* name, const glm::mat4& value ) {	
	SetUniform( name, &value[0][0], 16 );
}
/*
=============
OpenGLProgram::GetAttributeLocation

	Gets the location of an attribute.
=============
*/
GLuint OpenGLProgram::GetAttributeLocation( const char* name ) {
	unsigned long nameHash = StringUtils::Hash( name );
	std::map<unsigned long, GLuint>::const_iterator attrib = m_attributes.find( nameHash );
	if ( attrib == m_attributes.end() ) {
		GLint index = glGetAttribLocation( m_programID, name );
		if ( index != -1 ) {
			m_attributes[nameHash] = index;
			glEnableVertexAttribArray( index );
		} else {
			printf( "Could not find Attribute %s", name );
			return 0;
		}
	}

	return m_attributes[nameHash];
}
/*
=============
OpenGLProgram::GetUniformLocation

	Gets the location of a uniform.
=============
*/
GLint OpenGLProgram::GetUniformLocation( const char* name ) {
	Uniform* uniform = GetCachedUniform( name );
	if ( !uniform ) {
		return glGetUniformLocation( m_programID, name );		
	}
	return uniform->location;	
}
/*
=============
OpenGLProgram::GetCachedUniform

	Gets a cached uniform.
    Returns null if it's not cached.
=============
*/
OpenGLProgram::Uniform* OpenGLProgram::GetCachedUniform( const char* name ) {	
	std::unordered_map<unsigned int, Uniform*>::iterator iter = m_uniforms.find( StringUtils::Hash( name ) );
	if ( iter != m_uniforms.end() ) {
		return iter->second;
	}

    return NULL;
}
/*
=============
OpenGLProgram::SetProgramName

	Sets the programs name.
=============
*/
void OpenGLProgram::SetProgramName( const char* newName ) {
	delete[] m_programName;	
	m_programName = ( newName == NULL ) ? NULL : StringUtils::CopyCString( newName );
}
/*
=============
OpenGLProgram::SetOutVariable

	Sets the out variable on the program
=============
*/
void OpenGLProgram::SetOutVariable( const char* outVar ) {
	glBindFragDataLocation( m_programID, 0, outVar );
}

}