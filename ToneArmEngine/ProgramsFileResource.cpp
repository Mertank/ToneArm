/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	ProgramFileResource

	Created by: Karl Merten
	Created on: 15/05/2014

========================
*/
#include "ProgramsFileResource.h"
#include "CachedResourceLoader.h"
#include "Engine.h"
#include "Log.h"

namespace vgs {
IMPLEMENT_PROTOTYPE(ProgramsFileResource)
/*
==========
ProgramsFileResource::ProgramsFileResource

	ProgramsFileResource default constructor
==========
*/
ProgramsFileResource::ProgramsFileResource( void )
{
	REGISTER_PROTOTYPE(ProgramsFileResource, "sh")
}
/*
==========
ProgramsFileResource::~ProgramsFileResource

	ProgramsFileResource destructor
==========
*/
ProgramsFileResource::~ProgramsFileResource( void ) {
	DELETE_STD_VECTOR_POINTER( ProgramInfo, m_programs )
}
/*
==========
ProgramsFileResource::Load

	Resource Load override.
	Loads the program info into the class.
==========
*/
bool ProgramsFileResource::Load( const char* path, char key ) {
	if (BinaryFileResource::Load(path, key))
	{
		char* fileData				= ReadFile();
		unsigned char programCount	= fileData[0];

		char* programHeader			= &fileData[1];
		for ( unsigned char i = 0; i < programCount; ++i ) {	
			ProgramInfo* program = ReadProgram( programHeader ); 
			m_programs.push_back( program );

			programHeader += program->GetDataSize();
		}

		delete[] fileData;

		return true;
	}
	return false;
}
/*
==========
ProgramsFileResource::ReadProgram

	Reads a program info from the file.
	Returns if one was read.
==========
*/
ProgramInfo* ProgramsFileResource::ReadProgram( char* programHeader ) {
	char* progHeaderDataPtr = programHeader;
	char* programName		= StringUtils::CopyCString( progHeaderDataPtr );
	
	progHeaderDataPtr += StringUtils::Length( programName ) + 1;

	int programFeatures = 0;
	memcpy( &programFeatures, progHeaderDataPtr, sizeof( int ) );

	progHeaderDataPtr += sizeof( int );

	ProgramInfo* progInfo = new ProgramInfo();
	progInfo->programName = programName;
	progInfo->features = ( ProgramFeatures::Value )programFeatures;
	progInfo->shaderCount = *( progHeaderDataPtr++ );

	progInfo->shaders = new ShaderInfo*[progInfo->shaderCount];

	for ( unsigned char i = 0; i < progInfo->shaderCount; ++i ) {
		progInfo->shaders[i] = ReadShaderInfo( progHeaderDataPtr );
		progHeaderDataPtr += ShaderInfo::GetDataSize();
	}

	return progInfo;
}
/*
==========
ProgramsFileResource::ReadShaderInfo

	Returns the next shader info from the file.
	NULL if it's invalid.
==========
*/
ShaderInfo* ProgramsFileResource::ReadShaderInfo( char* shaderInfoData ) {
	ShaderInfo* shader = new ShaderInfo();

	shader->shaderType = ( ShaderType::Value )shaderInfoData[0];
	memcpy( &shader->shaderStart, &shaderInfoData[1], 4 );
	memcpy( &shader->shaderLength, &shaderInfoData[5], 4 );

	return shader;
}
/*
==========
ProgramsFileResource::GetProgramData

	Returns the program data
==========
*/
char* ProgramsFileResource::GetProgramData( unsigned int index ) {
	const ProgramInfo* prog = GetProgram( index );

	SeekToIndex( prog->shaders[0]->shaderStart );
	
	unsigned int programLength = 0;
	for ( int i = 0; i < prog->shaderCount; ++i ) {
		programLength += prog->shaders[i]->shaderLength;
	}

	return Read( programLength );
}
}