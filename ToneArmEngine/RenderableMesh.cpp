/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	RenderableMesh

	Created by: Karl Merten	
	Created on: 01/05/2014

========================
*/
#include "RenderableMesh.h"
#include "StringUtils.h"
#include "Log.h"
#include "CachedResourceLoader.h"
#include "BinaryFileResource.h"
#include "StringUtils.h"
#include "Material.h"

namespace vgs {
/*
========
RenderableMesh::RenderableMesh

	RenderableMesh default constructor.
========
*/
RenderableMesh::RenderableMesh( void ) :
	m_vertexComponents( VertexComponents::POSITION ),
	m_material( NULL )
{}
/*
========
RenderableMesh::~RenderableMesh

	RenderableMesh destructor.
========
*/
RenderableMesh::~RenderableMesh( void ) 
{}
/*
========
RenderableMesh::SetDiffuseColor

	Sets the diffuse color of the mesh
========
*/
void RenderableMesh::SetDiffuseColor( const glm::vec3& diffuseColor ) {
	m_material->SetColor( diffuseColor );
}
/*
========
RenderableMesh::AttachTexture

	Adds a texture to the mesh
========
*/
void RenderableMesh::AttachTexture( Texture* tex ) {
	m_material->AddTexture( tex );
}
/*
========
RenderableMesh::SetOpacity

	Sets the opacity of the material
========
*/
void RenderableMesh::SetOpacity( float alpha ) {
	m_material->SetOpacity( alpha );
}
/*
========
RenderableMesh::GetOpacity

	Gets the opacity of the material
========
*/
float RenderableMesh::GetOpacity( void ) {
	if ( m_material ) {
		return m_material->GetOpacity();
	} else {
		return 1.0f;
	}
}
} //namespace vgs