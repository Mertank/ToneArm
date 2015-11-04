/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ProgramFileResource

		File contating a list of shaders.

		Created by: Karl Merten
		Created on: 15/05/2014

========================
*/

#ifndef __PROGRAMSFILERESOURCE_H__
#define __PROGRAMSFILERESOURCE_H__

#include "BinaryFileResource.h"
#include "OpenGLProgramManager.h"
#include <memory>

namespace vgs {

class BinaryFileResource;

struct ShaderInfo {
	ShaderType::Value	shaderType;
	unsigned int		shaderStart;
	unsigned int		shaderLength;

	ShaderInfo( void ) :
		shaderType( ShaderType::VERTEX_SHADER ),
		shaderStart( 0 ),
		shaderLength( 0 )
	{}

	static unsigned int GetDataSize( void ) {
		//	  Type	 Start and Length          
		return 1 + ( sizeof( unsigned int ) * 2 );
	}
};

struct ProgramInfo {
	char*					programName;
	unsigned char			shaderCount;
	ShaderInfo**			shaders;
	ProgramFeatures::Value	features;

	ProgramInfo( void ) :
		programName( NULL ),
		shaderCount( 0 ),
		shaders( NULL ),
		features( ProgramFeatures::NONE )
	{}

	~ProgramInfo( void ) {
		for ( int i = 0; i < shaderCount; ++i ) {
			delete shaders[i];
		}

		delete[] shaders;
		delete[] programName;
	}

	unsigned int GetDataSize( void ) {
		       //Name                 NULL  Features            Shader Count      Shader Data
		return strlen( programName ) + 1 + sizeof( unsigned int ) + 1 + ( ShaderInfo::GetDataSize() * shaderCount );
	}
};

class ProgramsFileResource : public BinaryFileResource {
friend class CachedResourceLoader;
DECLARE_PROTOTYPE(ProgramsFileResource)
public:
										~ProgramsFileResource( void );
	bool								Load( const char* path, const char key = 0x00 );

	unsigned int						GetProgramCount( void ) const { return m_programs.size(); }
	const ProgramInfo*					GetProgram( unsigned int index ) const { return m_programs[index]; }
	char*								GetProgramData( unsigned int index );
private:
	std::vector<ProgramInfo*>			m_programs;
								
										ProgramsFileResource( void );
	ProgramInfo*						ReadProgram( char* programHeader );
	ShaderInfo*							ReadShaderInfo( char* shaderInfoData );

};

}

#endif //__PROGRAMSFILERESOURCE_H__