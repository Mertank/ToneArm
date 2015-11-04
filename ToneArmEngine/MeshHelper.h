/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MeshHelper

		Basic fuctions for generating generic meshes.

		Created by: Karl Merten
		Created on: 12/05/2014

========================
*/

#ifndef __MESHHELPER_H__
#define __MESHHELPER_H__

#include "RenderableMesh.h"

namespace vgs {

namespace MeshHelper {

	void MakeLine( float length, float** vertDataDest, unsigned short** indexDataDest, unsigned int& vertexBufferSize, unsigned int& indexCount );
	void MakeLine( float length, const glm::vec3& direction, float** vertDataDest, unsigned short** indexDataDest, unsigned int& vertexBufferSize, unsigned int& indexCount );
	void MakeRectMesh( float width, float height, float depth, VertexComponents::Value components, float** vertDataDest, unsigned short** indexDataDest, unsigned int& vertexBufferSize, unsigned int& indexCount );
	void MakeCylinderMesh( float radius, float height, unsigned int sections, VertexComponents::Value components, float** vertexDataDest, unsigned short** indexDataDest, unsigned int& vertexCount, unsigned int& indexCount );
	void MakeCircleMesh( float radius, unsigned int sections, VertexComponents::Value components, float** vertexDataDest, unsigned short** indexDataDest, unsigned int& vertexCount, unsigned int& indexCount );

	void MakeMeshFromBuffer( const Joint* joints, unsigned int jointCount, char* buffer, float** vertDataDest, unsigned int** indexDataDest, unsigned int& vertexBufferSize, unsigned int& indexCount, VertexComponents::Value& components, char** endPosition, glm::vec3& minVertex, glm::vec3& maxVertex );
	 
	void ReadMesh( char* bufferStart, const Joint* joints, unsigned int jointCount, unsigned int& bytesRead );
	void ReadTriangle( char* buffer, Triangle* triangles );
	void ReadWeight( char* buffer, Weight* weights );
	 
	void ReadVertex( char* buffer, Vertex* vert );	
	void GenerateMeshData( float** vertDataDest, unsigned int** indexDataDest, unsigned int& vertexBufferSize, unsigned int& indexCount, 
	 							const Joint* joints, unsigned int jointCount, Vertex* verticies, unsigned int vertexCount, Weight* weights, unsigned int weightCount,
	 							Triangle* triangles, unsigned int triangleCount, glm::vec3& minVertex, glm::vec3& maxVertex );
	 
	void ComputeVerticies( float** vertexDataDest, Vertex* verticies, unsigned int vertexCount, const Joint* joints, unsigned int jointCount, Weight* weights, unsigned int weightCount, glm::vec3& minVertex, glm::vec3& maxVertex );
	void ComputeIndicies( unsigned int** indexDataDest, Triangle* triangles, unsigned int triangleCount );
	void ComputeNormals( float** vertexDataDest, Vertex* verticies, unsigned int vertexCount, Weight* weights, unsigned int weightCount, Triangle* triangles, unsigned int triangleCount, const Joint* joints );

}

}

#endif //__MESHHELPER_H__