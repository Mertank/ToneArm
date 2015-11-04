/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Renderable

	Created by: Karl Merten
	Created on: 01/05/2014

========================
*/
#include "Renderable.h"
#include "RenderMessages.h"

namespace vgs {

IMPLEMENT_BASE_RTTI( Renderable )

Renderable::Renderable( void ) :
	m_nodeID( 0 ),
	m_drawOutlineOnly( false ),
	m_modelMatrix( 1.0 ),
	m_useLights( false ),
	m_lightingType( LightingType::VERTEX_LIT ),
	m_visible( true ),
	m_requiredFeatures( RenderableFeatures::NONE ),
	m_attachedPass( PassType::DIFFUSE ),
	m_renderer( NULL ),
	m_parent( NULL ),
	m_transformChanged( false )
{}

Renderable::~Renderable( void ) {
	DELETE_STD_VECTOR_POINTER( Renderable, m_children );
}

void Renderable::UpdateTransform( TransformMessage* msg ) {
	UpdateTransform( msg->rotationMatrix, msg->position );
}

void Renderable::UpdateTransform( const glm::mat3& rot, const glm::vec3& loc ) {
	memcpy( &m_modelMatrix[0][0], &rot[0][0], sizeof( float ) * 3 );
	memcpy( &m_modelMatrix[1][0], &rot[1][0], sizeof( float ) * 3 );
	memcpy( &m_modelMatrix[2][0], &rot[2][0], sizeof( float ) * 3 );
	memcpy( &m_modelMatrix[3][0], &loc[0]	, sizeof( float ) * 3 );

	m_transformChanged = true;
}

bool Renderable::DidTransformChange( void ) const {
	if ( m_transformChanged ) {
		return true;
	} else {
		if ( m_parent ) {
			return m_parent->DidTransformChange();
		} else {
			return m_transformChanged;
		}
	}
}

const glm::mat4& Renderable::GetCompositeMatrix( void ) {
	return m_compositeMatrix;
}

void Renderable::AddChild( Renderable* renderable ) {
	m_children.push_back( renderable );
	renderable->SetParentRenderable( this );
}

void Renderable::RemoveChild( unsigned int id ) {
	for ( std::vector<Renderable*>::iterator iter = m_children.begin();
		  iter != m_children.end(); ++iter ) {
		if ( ( *iter )->GetNodeID() == id ) {
			std::iter_swap( iter, m_children.end() - 1 );
			m_children.pop_back();
			return;
		}
	}
}

Renderable* Renderable::GetChildRenderable( unsigned int nodeID ) {
	for ( std::vector<Renderable*>::iterator iter = m_children.begin();
		  iter != m_children.end(); ++iter ) {
		if ( ( *iter )->GetNodeID() == nodeID ) {
			return ( *iter );
		}

		Renderable* child = ( *iter )->GetChildRenderable( nodeID );
		if ( child ) {
			return child;
		}
	}

	return NULL;
}

void Renderable::Update( float dt ) {
	UpdateCompositeMatrix();

	for ( std::vector<Renderable*>::iterator iter = m_children.begin();
		  iter != m_children.end(); ++iter ) {
		( *iter )->Update( dt );
	}	

	m_transformChanged = false;
}

void Renderable::UpdateCompositeMatrix( void ) {
	if ( DidTransformChange() ) {
		if ( m_parent ) {
			m_compositeMatrix = m_parent->GetCompositeMatrix() * m_modelMatrix;
		} else {
			m_compositeMatrix = m_modelMatrix;
		}

		UpdatedCompositeMatrix();
	}
}

} //namespace vgs