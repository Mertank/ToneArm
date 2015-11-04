/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	RenderableModel

	Created by: Karl Merten
	Created on: 06/05/2014

========================
*/
#include "RenderableModel.h"
#include "OpenGLMesh.h"
#include "CachedResourceLoader.h"
#include "BinaryFileResource.h"
#include "Log.h"
#include "MeshStructs.h"
#include "RenderMessages.h"
#include "RenderableMesh.h"
#include "Animator.h"
#include "RenderModule.h"
#include "Animation.h"

namespace vgs {
IMPLEMENT_RTTI( RenderableModel, Renderable )
/*
========
RenderableModel::RenderableModel

	RenderableModel default constructor
========
*/
RenderableModel::RenderableModel( void ) :	
	m_meshCount( 0 ),
	m_meshes( NULL ),
	m_animator( NULL ),
	m_matchBoneHash( 0 )
{}
/*
========
RenderableModel::RenderableModel

	RenderableModel destructor
========
*/
RenderableModel::~RenderableModel( void ) {
	for ( unsigned int i = 0; i < m_meshCount; ++i ) {
		delete m_meshes[i];
	}
	
	delete[] m_meshes;
	delete m_animator;
}
/*
========
OpenGLModel::AttachMesh

	Attachs a mesh to the model
========
*/
void RenderableModel::AttachMesh( RenderableMesh* mesh ) {
	m_meshes[GetNextMeshIndex()] = mesh;

	glm::vec3 minVert;
	glm::vec3 maxVert;

	for ( unsigned int i = 0; i < m_meshCount; ++i ) {
		if ( m_meshes[i] == NULL ) { continue; }

		glm::vec3 boxMin = m_meshes[i]->GetBoxOffset() - ( ( ( BoxShape& )m_meshes[i]->GetBoundingBox() ).GetBoundingDimensions() * 0.5f );
		glm::vec3 boxMax = m_meshes[i]->GetBoxOffset() + ( ( ( BoxShape& )m_meshes[i]->GetBoundingBox() ).GetBoundingDimensions() * 0.5f );

		if ( boxMin.x < minVert.x ) {
			minVert.x = boxMin.x;
		} else if ( boxMax.x < minVert.x ) {
			minVert.x = boxMax.x;
		}

		if ( boxMin.x > maxVert.x ) {
			maxVert.x = boxMin.x;
		} else if ( boxMax.x > maxVert.x ) {
			maxVert.x = boxMax.x;
		}

		if ( boxMin.y < minVert.y ) {
			minVert.y = boxMin.y;
		} else if ( boxMax.y < minVert.y ) {
			minVert.y = boxMax.y;
		}

		if ( boxMin.y > maxVert.y ) {
			maxVert.y = boxMin.y;
		} else if ( boxMax.y > maxVert.y ) {
			maxVert.y = boxMax.y;
		}

		if ( boxMin.z < minVert.z ) {
			minVert.z = boxMin.z;
		} else if ( boxMax.z < minVert.z ) {
			minVert.z = boxMax.z;
		}

		if ( boxMin.z > maxVert.z ) {
			maxVert.z = boxMin.z;
		} else if ( boxMax.z > maxVert.z ) {
			maxVert.z = boxMax.z;
		}
	}

	m_boxOffset = ( minVert + maxVert ) * 0.5f;
	m_boundingBox.SetDimensions( maxVert - minVert );
}
/*
========
OpenGLModel::AttachTexture

	Attachs a texture to the model
========
*/
void RenderableModel::AttachTexture( Texture* tex, unsigned int meshIndex ) {
	if ( m_meshes[meshIndex] ) {
		m_meshes[meshIndex]->AttachTexture( tex );	
		if ( tex->HasAlphaChannel() ) {
			m_attachedPass = PassType::TRANSPARENCY;
		}
	} else {
		delete tex;
	}
}
/*
========
OpenGLModel::IsLit

	Sets if the model is lit.
	Passes value to all meshes.
========
*/
void RenderableModel::IsLit( bool lit ) {
	Renderable::IsLit( lit );
	for ( unsigned int i = 0; i < m_meshCount; ++i ) {
		if (m_meshes[i] == nullptr)
		{
			continue;
		}
		m_meshes[i]->IsLit( lit );
	}
}
/*
========
OpenGLModel::SetDrawingOutlineOnly

	Sets if the model is only drawing its outline.
	Passes value to all meshes.
========
*/
void RenderableModel::SetDrawingOutlineOnly( bool outline ) {
	Renderable::SetDrawingOutlineOnly( outline );
	for ( unsigned int i = 0; i < m_meshCount; ++i ) {
		if (m_meshes[i] == nullptr)
		{
			continue;
		}
		m_meshes[i]->SetDrawingOutlineOnly( outline );
	}
}
/*
========
OpenGLModel::SetDiffuseColor

	Sets the diffuse color
========
*/
void RenderableModel::SetDiffuseColor( const glm::vec3& diffColor ) {
	for ( unsigned int i = 0; i < m_meshCount; ++i ) {
		if (m_meshes[i] == nullptr)
		{
			continue;
		}
		m_meshes[i]->SetDiffuseColor( diffColor );
	}
}
/*
========
RenderableModel::SetOpacity

	Sets the opacity.
========
*/
void RenderableModel::SetOpacity( float alpha ) {
	for ( unsigned int i = 0; i < m_meshCount; ++i ) {
		if ( m_meshes[i] == nullptr )	{
			continue;
		}
		m_meshes[i]->SetOpacity( alpha );
	}

	if ( GetOpacity() < ( 1.0f - FLOAT_EPSILON ) ) {
		if ( m_attachedPass & PassType::DIFFUSE ) {
			m_attachedPass = ( PassType::Value )( m_attachedPass & ~PassType::DIFFUSE );
			m_attachedPass = ( PassType::Value )( m_attachedPass | PassType::TRANSPARENCY );
		}
	} else {
		if ( m_attachedPass & PassType::TRANSPARENCY ) {
			m_attachedPass = ( PassType::Value )( m_attachedPass & ~PassType::TRANSPARENCY );
			m_attachedPass = ( PassType::Value )( m_attachedPass | PassType::DIFFUSE );
		}
	}
}
/*
========
RenderableModel::GetOpacity

	Gets the opacity.
========
*/
float RenderableModel::GetOpacity( void ) {
	float opacity = 1.0f;
	for ( unsigned int i = 0; i < m_meshCount; ++i ) {
		if ( m_meshes[i] ) {
			opacity = std::min( opacity, m_meshes[i]->GetOpacity() );
		}
	}

	return opacity;
}
/*
========
RenderableModel::GetMesh

	Returns the mesh at index
========
*/
RenderableMesh* RenderableModel::GetMesh( unsigned int index ) {
	if ( m_meshes ) {
		return m_meshes[index];
	} else {
		return NULL;
	}
}
/*
========
RenderableModel::UpdateTransform

	Update the transform
========
*/
void RenderableModel::UpdateTransform( TransformMessage* msg ) {
	UpdateTransform( msg->rotationMatrix, msg->position );	
}
/*
========
RenderableModel::UpdateTransform

	Update the transform
========
*/
void RenderableModel::UpdateTransform( const glm::mat3& rot, const glm::vec3& loc ) {
	Renderable::UpdateTransform( rot, loc );
	UpdatedCompositeMatrix();
}
/*
========
RenderableModel::AddAnimation

	Adds a animation.
========
*/
void RenderableModel::AddAnimation( Animation* anim ) {
	if ( !m_animator ) {
		m_animator = m_renderer->CreateAnimator( anim->GetJointCount() );
	}

	m_animator->AddAvailableAnimation( anim );
}
/*
========
RenderableModel::SetBindMatricies

	Adds a animation.
========
*/
void RenderableModel::SetBindMatricies( glm::mat4* matricies, unsigned int count ) {
	if ( !m_animator ) {
		m_animator = m_renderer->CreateAnimator( count );
	}

	m_animator->SetBindMatricies( matricies );
}
/*
========
RenderableModel::Update

	Updates the animator.
========
*/
void RenderableModel::Update( float dt ) {
	if ( m_animator ) {
		m_animator->UpdateAnimation( dt );
	}

	if ( m_matchBoneHash > 0 ) {
		if ( m_parent ) {
			Animator* parentAnimator = ( ( RenderableModel* )m_parent )->GetAnimator();
			if ( parentAnimator ) {
				const SkeletonJoint* boneInfo = parentAnimator->GetJointNamed( m_matchBoneHash );
				if ( boneInfo ) {
					UpdateTransform( glm::toMat3( boneInfo->orientation ), boneInfo->position );
				} else {
					UpdateTransform( glm::mat3( 1.0 ), glm::vec3( 0.0 ) );
				}
			}
		}
	}

	Renderable::Update( dt );
}

void RenderableModel::UpdatedCompositeMatrix( void ) {
	if ( DidTransformChange() ) {
		GetCompositeMatrix();
		float angle = glm::degrees( atan2(m_compositeMatrix[0][2], m_compositeMatrix[0][0] ) );
		m_boundingBox.SetRotation( glm::vec3( 0.0f, angle, 0.0f ) );
		m_boundingBox.SetPosition( glm::vec3( m_compositeMatrix[3] ) + m_boxOffset );
	}
}

unsigned int RenderableModel::GetNextMeshIndex( void ) {
	unsigned int nextMesh = 0;
	
	while ( nextMesh < m_meshCount ) {
		if ( m_meshes[nextMesh] == NULL ) { break; }
		++nextMesh;
	}

	return nextMesh;
}
}