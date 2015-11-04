/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RenderableMesh

		Base class for meshes on the renderer.

		Created by: Karl Merten	
		Created on: 01/05/2014

========================
*/

#ifndef __RENDERABLEMESH_H__
#define __RENDERABLEMESH_H__

#include "MeshStructs.h"
#include "Renderable.h"
#include "BoxShape.h"

namespace vgs {

class Material;
class Texture;

namespace VertexComponents {
	enum Value {
		POSITION		= 0x01,
		NORMAL			= 0x02,
		TEXTURE			= 0x04,
		WEIGHTS			= 0x08,
		MATRIX_INDEX	= 0x10
	};
}

class RenderableMesh : public Renderable {
public:
	virtual					~RenderableMesh( void );

	virtual void			Render( void ) = 0;

	void					SetDiffuseColor( const glm::vec3& diffuseColor );
	
	void					SetOpacity( float alpha );
	float					GetOpacity( void );

	virtual void			AttachTexture( Texture* tex );	

	const Shape&			GetBoundingBox( void ) const { return m_boundingBox; }
	const glm::vec3&		GetBoxOffset( void ) const { return m_boxOffset; }
protected:
							RenderableMesh( void );

	Material*				m_material;
	VertexComponents::Value	m_vertexComponents;

	glm::vec3				m_boxOffset;
	BoxShape				m_boundingBox;
};

} //namespace vgs

#endif //__RENDERABLEMESH_H__