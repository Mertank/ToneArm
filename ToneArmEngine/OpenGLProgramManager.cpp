/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	OpenGLProgramManager

	Created by: Karl Merten
	Created on: 30/04/2014

========================
*/
#include "OpenGLProgramManager.h"
#include "OpenGLProgram.h"
#include "StringUtils.h"
#include "Log.h"

namespace vgs {

INITIALIZE_SINGLETON( OpenGLProgramManager )
/*
=======
OpenGLProgramManager::OpenGLProgramManager

	OpenGLProgramManager default constructor.
=======
*/
OpenGLProgramManager::OpenGLProgramManager( void ) :
	m_currentProgram( NULL )
{}
/*
=======
OpenGLProgramManager::~OpenGLProgramManager

	OpenGLProgramManager destructor.
=======
*/
OpenGLProgramManager::~OpenGLProgramManager( void ) {
	std::map<unsigned long, OpenGLProgram*>::iterator iter = m_programs.begin();
	while ( iter != m_programs.end() ) {
		delete iter->second;
		++iter;
	}
}
/*
=======
OpenGLProgramManager::UseProgram

	Uses a program by name.
	Returns the program it is using.
=======
*/
OpenGLProgram* const OpenGLProgramManager::UseProgram( const char* programName ) {
	UseProgram( FindProgram( programName ) );
	return GetActiveProgram();
}
/*
=======
OpenGLProgramManager::UseProgramWithFeatures

	Uses a program that supports the features requested/
	Returns the program it is using.
=======
*/
OpenGLProgram* const OpenGLProgramManager::UseProgramWithFeatures( ProgramFeatures::Value features ) {
	UseProgram( FindProgram( features ) );
	return GetActiveProgram();
}
/*
=======
OpenGLProgramManager::UseProgram

	Uses the passed in program.
=======
*/
void OpenGLProgramManager::UseProgram( OpenGLProgram* const program ) {
	if ( program == NULL ) {
		glUseProgram( 0 );
		m_currentProgram = NULL;
	} else {
		if ( !m_currentProgram ||
			 m_currentProgram->GetProgramId() != program->GetProgramId() ) {
			glUseProgram( program->GetProgramId() );
			m_currentProgram = program;
		}
	}
}
/*
=======
OpenGLProgramManager::AddProgram

	Adds a new program with the passed in shaders attached.
=======
*/
bool OpenGLProgramManager::AddProgram( std::vector<Shader*>& shaderList, const char* outVariable, const char* programName, ProgramFeatures::Value features ) {
	unsigned long nameHash = StringUtils::Hash( programName );
	OpenGLProgram* prog = OpenGLProgram::CreateProgram( programName, shaderList, outVariable, features );
	if ( prog ) {
		m_programs[nameHash] = prog;

#ifdef TONEARM_DEBUG
		char buffer[255];
		memset( buffer, 0, 255 );
		sprintf_s( buffer, "Created %s program", programName );
		Log::GetInstance()->WriteToLog( "OpenGLProgramManager", buffer );
#endif

		return true;
	} else {
		return false;
	}
}
/*
=======
OpenGLProgramManager::FindProgram

	Finds a program by name
=======
*/
OpenGLProgram* const OpenGLProgramManager::FindProgram( const char* name ) const {
	unsigned long nameHash = StringUtils::Hash( name );
	std::map<unsigned long, OpenGLProgram*>::const_iterator program = m_programs.find( nameHash );

	return ( program == m_programs.end() ) ? NULL : program->second;
}
/*
=======
OpenGLProgramManager::FindProgramWithFeatures

	Finds a program with the features requested
=======
*/
OpenGLProgram* const OpenGLProgramManager::FindProgram( ProgramFeatures::Value features ) const {
	OpenGLProgram* retProg = NULL;
	for ( std::map<unsigned long, OpenGLProgram*>::const_iterator program = m_programs.begin();
	      program != m_programs.end(); ++program ) {
		if ( program->second->GetFeatures() == features ) {
			retProg = program->second;
			break;
		}
	}

	return retProg;
}
/*
=======
OpenGLProgramManager::RegisterSharedUniform

	Registers a shader uniform variable.
=======
*/
void OpenGLProgramManager::RegisterSharedUniform( const char* name, unsigned int size ) {
	GLuint uniformBlockIndex = glGetUniformBlockIndex( m_programs.begin()->second->GetProgramId(), name );
	if ( uniformBlockIndex != GL_INVALID_INDEX ) {
		GLuint sharedBuffer = 0;
		glGenBuffers( 1, &sharedBuffer );
		if ( !sharedBuffer ) {
			Log::GetInstance()->WriteToLog( "OpenGLProgramManager", "Could not create a shared buffer" );
			return;
		}

		glBindBuffer( GL_UNIFORM_BUFFER, sharedBuffer );
		glBufferData( GL_UNIFORM_BUFFER, size, NULL, GL_STREAM_DRAW );
		glBindBuffer( GL_UNIFORM_BUFFER, 0 );

		m_sharedUniform[StringUtils::Hash( name )] = sharedBuffer;
	}
}
/*
=======
OpenGLProgramManager::SetSharedUniform

	Sets a shared uniforms value
=======
*/
void OpenGLProgramManager::SetSharedUniform( const char* name, void* data, unsigned int size ) {
	std::map<unsigned long, GLuint>::iterator uniformBuffer = m_sharedUniform.find( StringUtils::Hash( name ) );
	if ( uniformBuffer != m_sharedUniform.end() ) {
		glBindBuffer( GL_UNIFORM_BUFFER, uniformBuffer->second );
		glBufferData( GL_UNIFORM_BUFFER, size, data, GL_STREAM_DRAW );
		glBindBuffer( GL_UNIFORM_BUFFER, 0 );
	}
}
}