/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLProgramManager

		A singleton class that initializes programs.
		It can switch between them based on name of ID.

		Created by: Karl Merten
		Created on: 30/04/2014

========================
*/

#ifndef __OPENGLPROGRAMMANAGER_H__
#define __OPENGLPROGRAMMANAGER_H__

#include "VGSMacro.h"
#include <GL\glew.h>
#include <vector>
#include <map>

namespace vgs {

namespace ShaderType {
	enum Value : unsigned char {
		VERTEX_SHADER		= 0x00,
		FRAGMENT_SHADER		= 0x01,
		GEOMETRY_SHADER		= 0x02,
		NO_SHADER			= 0xFF
	};
}

namespace ProgramFeatures {
	enum Value {
		NONE					= 0x00, //This is really a plain diffuse shader
		TEX_COORDS				= 0x01,
		NORMALS					= 0x02,
		VERTEX_LIGHTING			= 0x04,
		FRAGMENT_LIGHTING		= 0x08,
		DIFFUSE_TEXTURE			= 0x10,
		NORMAL_MAP				= 0x20,
		SPEC_MAP				= 0x40,
		ALPHA_MASK				= 0x80,
		GPU_SKINNING			= 0x100,
		PARTICLE_BILLBOARDING	= 0x200
	};
}

class OpenGLProgram;

struct Shader {
	ShaderType::Value	shaderType;
	char*				shaderLocation;	
	
	//NOTE: If this length is 0 the shaderLocation is a path.
	// If length is non-zero it is a pointer to a byte buffer.
	// This may be a bad idea, but I have no idea how to do it better.
	unsigned int		shaderLength;	

	Shader( void ) :
		shaderType( ShaderType::VERTEX_SHADER ),
		shaderLocation( NULL ),
		shaderLength( 0 )
	{}
};

class OpenGLProgramManager {
											CREATE_SINGLETON( OpenGLProgramManager )	
public:
											~OpenGLProgramManager( void );

	OpenGLProgram* const 					UseProgram( const char* programName );
	void									UseProgram( OpenGLProgram* const program );	
	OpenGLProgram* const 					UseProgramWithFeatures( ProgramFeatures::Value features );	
	void									ClearProgram( void ) { UseProgram( ( OpenGLProgram* )NULL ); }
	
	bool									AddProgram( std::vector<Shader*>& shaderList, const char* outVariableName, const char* programName = NULL, ProgramFeatures::Value features = ProgramFeatures::NONE );	
	void									RegisterSharedUniform( const char* name, unsigned int size );
	void									SetSharedUniform( const char* name, void* data, unsigned int size );

	OpenGLProgram* const					GetActiveProgram( void ) const { return m_currentProgram; }
	OpenGLProgram* const					GetProgram( const char* name ) const { return FindProgram( name ); }
	OpenGLProgram* const					GetProgramWithFeatures( ProgramFeatures::Value features ) const { return FindProgram( features ); }
private:
	OpenGLProgram* const					FindProgram( const char* name ) const;
	OpenGLProgram* const					FindProgram(ProgramFeatures::Value features ) const;

	OpenGLProgram*							m_currentProgram;
	std::map<unsigned long, OpenGLProgram*>	m_programs;
	std::map<unsigned long, GLuint>			m_sharedUniform;
};

}
#endif //__OPENGLPROGRAMMANAGER_H__