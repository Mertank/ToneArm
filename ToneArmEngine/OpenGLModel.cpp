/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	OpenGLModel

	Created by: Karl Merten
	Created on: 13/05/2014

========================
*/
#include "OpenGLModel.h"
#include "RenderMessages.h"
#include "OpenGLMesh.h"
#include "OpenGLProgramManager.h"
#include "OpenGLProgram.h"
#include "OpenGLTexture.h"
#include "OpenGLRenderModule.h"
#include "OpenGLAnimator.h"
#include "CameraProjectionData.h"

#include <SFML\Window.hpp>

namespace vgs {
/*
========
OpenGLModel::OpenGLModel

	OpenGLModel constructor
========
*/
OpenGLModel::OpenGLModel( void ) :
	m_requiredFeatures( ProgramFeatures::NONE )
{}
/*
========
OpenGLModel::CreateModelFromMessage

	OpenGLModel Create function from a message.
========
*/
OpenGLModel* OpenGLModel::CreateModelFromMessage( const CreateModelMessage* msg ) {
	OpenGLModel* retPtr = new OpenGLModel();
	if ( retPtr && retPtr->InitializeFromMessage( msg ) ) {
		retPtr->IsLit( true );
		return retPtr;
	} else {
		delete retPtr;
		return NULL;
	}

	return retPtr;
}
/*
========
OpenGLModel::DuplicateModel

	OpenGLModel Create function from a message.
========
*/
OpenGLModel* OpenGLModel::DuplicateModel( const OpenGLModel* other, DuplicateModelMessage* msg ) {
	OpenGLModel* retPtr = new OpenGLModel();
	if ( retPtr && retPtr->InitializeFromModel( other, msg ) ) {
		return retPtr;
	} else {
		delete retPtr;
		return NULL;
	}

	return retPtr;
}
/*
========
OpenGLModel::InitializeFromMessage

	OpenGLModel initialization function from a message.
========
*/
bool OpenGLModel::InitializeFromMessage( const CreateModelMessage* msg ) {
	m_nodeID = msg->nodeID;
	m_meshes = new RenderableMesh*[msg->meshCount];
	m_meshCount = msg->meshCount;

	for ( unsigned int i = 0; i < m_meshCount; ++i ) {
		m_meshes[i] = NULL;
	}

	return m_meshes != NULL;
}
/*
========
OpenGLModel::InitializeFromModel

	OpenGLModel initialization function from a model.
========
*/
bool OpenGLModel::InitializeFromModel( const OpenGLModel* other, DuplicateModelMessage* msg ) {
	m_nodeID = msg->nodeID;
	
	m_meshes = new RenderableMesh*[other->m_meshCount];
	m_meshCount = other->m_meshCount;

	for ( unsigned int i = 0; i < m_meshCount; ++i ) {
		if ( other->m_meshes[i] != NULL ) {
			m_meshes[i] = OpenGLMesh::CreateDuplicate( ( OpenGLMesh* )other->m_meshes[i], i );
			m_meshes[i]->SetNodeID( m_nodeID );
		} else {
			m_meshes[i] = NULL;
		}
	}

	m_boundingBox.SetDimensions( other->m_boundingBox.GetDimensions() );
	m_boxOffset = other->m_boxOffset;

	m_requiredFeatures = other->m_requiredFeatures;
	m_renderer = other->m_renderer;

	if ( other->m_animator ) {
		m_animator = OpenGLAnimator::CreateFromAnimator( ( OpenGLAnimator* )other->m_animator );
	}

	IsLit( true );

	return m_meshes != NULL;	
}
/*
=============
OpenGLModel::PreRender

	Uses the appropriate program for the model
=============
*/
void OpenGLModel::PreRender( void ) {
	if ( !m_visible ) { return; }

	OpenGLProgramManager::GetInstance()->UseProgramWithFeatures( m_requiredFeatures );
	OpenGLProgram* prog = OpenGLProgramManager::GetInstance()->GetActiveProgram();
	if ( !prog ) { 
		return; 
	}

	if ( m_animator ) {
		m_animator->PreRender();
	}

	for ( unsigned int i = 0; i < m_meshCount; ++i ) {
		if ( m_meshes[i] ) {
			m_meshes[i]->PreRender();
		}
	}

	if ( m_attachedPass & PassType::TRANSPARENCY ) {
		prog->SetUniform( "u_Opacity", GetOpacity() );
	}
	
	if ( m_requiredFeatures & ProgramFeatures::NORMALS ) {
		prog->SetUniform( "u_NormalMatrix", glm::inverse( glm::transpose( glm::mat3( m_renderer->GetCameraData().GetViewMatrix() * GetCompositeMatrix() ) ) ) );
	}
	
	prog->SetUniform( "u_ModelMatrix", GetCompositeMatrix() );
	( ( const OpenGLRenderModule* )m_renderer )->SendGlobalUniforms();
}	
/*
=============
OpenGLModel::Render

	Renders the model
=============
*/
void OpenGLModel::Render( void ) {
	if ( !m_visible || !OpenGLProgramManager::GetInstance()->GetActiveProgram() ) { return; }

	for ( unsigned int i = 0; i < m_meshCount; ++i ) {
		if ( m_meshes[i] ) {
			m_meshes[i]->Render();
		}
	}
}
/*
=============
OpenGLModel::PostRender

	GL state cleanup if necessary
=============
*/
void OpenGLModel::PostRender( void ) {
	if ( !m_visible || !OpenGLProgramManager::GetInstance()->GetActiveProgram() ) { return; }

	if ( m_animator ) {
		m_animator->PostRender();
	}

	for ( unsigned int i = 0; i < m_meshCount; ++i ) {
		if ( m_meshes[i] ) {
			m_meshes[i]->PostRender();
		}
	}
}
/*
=============
OpenGLModel::AttachTexture

	Attaches a texture to the model
=============
*/
void OpenGLModel::AttachTexture( Texture* tex, unsigned int meshIndex ) {
	RenderableModel::AttachTexture( tex, meshIndex );
	if ( m_meshes[meshIndex] ) {
		m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ( ( OpenGLMesh* )m_meshes[meshIndex] )->GetRequiredFeatures() );
	}
}
/*
=============
OpenGLModel::AttachMesh

	Attaches a mesh to the model
=============
*/
void OpenGLModel::AttachMesh( RenderableMesh* mesh ) {
	RenderableModel::AttachMesh( mesh );
	mesh->SetNodeID( m_nodeID );
	m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ( ( OpenGLMesh* )mesh )->GetRequiredFeatures() );
}
/*
=============
OpenGLModel::IsLit

	Sets if the model is lit or not
=============
*/
void OpenGLModel::IsLit( bool lit ) {
	Renderable::IsLit( lit );
	if ( lit ) {
		m_requiredFeatures = ( ProgramFeatures::Value )( ( ( m_lightingType == LightingType::VERTEX_LIT ) ? ProgramFeatures::VERTEX_LIGHTING : ProgramFeatures::FRAGMENT_LIGHTING ) | m_requiredFeatures );		
	} else {
		m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures & ~ProgramFeatures::VERTEX_LIGHTING );
		m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures & ~ProgramFeatures::FRAGMENT_LIGHTING );
	}
}
/*
=============
OpenGLModel::SetLightingType

	Sets the lighting type of the model
=============
*/
void OpenGLModel::SetLightingType( LightingType::Value lightingType ) {
	if ( m_lightingType != lightingType ) {
		m_lightingType = lightingType;

		if ( lightingType == LightingType::VERTEX_LIT ) {
			m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ProgramFeatures::VERTEX_LIGHTING );		
			m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures & ~ProgramFeatures::FRAGMENT_LIGHTING );		
		} else if ( lightingType == LightingType::FRAGMENT_LIT ) {
			m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures & ~ProgramFeatures::VERTEX_LIGHTING );		
			m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ProgramFeatures::FRAGMENT_LIGHTING );		
		}
	}
}
/*
=============
OpenGLModel::AddAnimation

	Adds an animation to the model
=============
*/
void OpenGLModel::AddAnimation( Animation* anim ) {
	RenderableModel::AddAnimation( anim );
	m_requiredFeatures = ( ProgramFeatures::Value )( m_requiredFeatures | ProgramFeatures::GPU_SKINNING );
}
/*
=============
OpenGLModel::AttachAnimator

	Attaches an animator to the model.
=============
*/
void OpenGLModel::AttachAnimator( Animator* anim ) {
	RenderableModel::AttachAnimator( anim );
}

}