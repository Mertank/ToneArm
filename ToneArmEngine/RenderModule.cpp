/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	RenderModule

	Created by: Karl Merten
	Created on: 25/04/2014

========================
*/
#include "RenderModule.h"
#include "InitializationFileResource.h"
#include "OpenGLRenderModule.h"
#include "Engine.h"
#include "RunningInfo.h"
#include "GUIManager.h"
#include "FreeLookCamera.h"
#include "RenderMessages.h"
#include "OpenGLModel.h"
#include "Log.h"
#include "CameraProjectionData.h"
#include "MessagePoolManager.h"
#include "RenderPass.h"
#include "SpacePartitionTree.h"
#include "Animator.h"

#define MESSAGE_POOL_ALLOCATOR_SIZE		16384

#define PROCESS_MESSAGE( __msgName__ )							\
	if ( ( *iter )->GetRTTI() == __msgName__::rtti ) {			\
		Process##__msgName__##s( ( __msgName__* )( *iter ) );	\
		FreeMessage( ( *iter ) );								\
		continue;												\
	}															\

namespace vgs {
/*
=============
RenderModule::RenderModule

	RenderModule default constructor.
=============
*/
RenderModule::RenderModule( void ) : 
	m_renderObjectsMutex(nullptr),
	m_frameRate( 30 ),
	m_allocatorIndex( 0 ),
	m_captureInput( true )
#ifdef TONEARM_DEBUG
	,
	m_instantaniousFrameRate( 30 ),
	m_averageFrameRate( 0 ),
	m_averageFrameRateSum( 0 ),
	m_averageTimeElapsed( 0.0f ),
	m_frameCount( 0 )
#endif
{
	m_msgAllocators[0] = NULL;
	m_msgAllocators[1] = NULL;
}
/*
=============
RenderModule::~RenderModule

	RenderModule destructor.
=============
*/
RenderModule::~RenderModule( void ) {
	for ( std::unordered_map<unsigned int, Renderable*>::iterator iter = m_renderObjects.begin();
		  iter != m_renderObjects.end(); ++iter ) {
		delete iter->second;
	}

	delete m_renderObjectsMutex;
	delete m_camera;

	for ( std::vector<RenderMessage*>::iterator iter = m_messages.begin();
		  iter != m_messages.end(); ++iter ) {
		if ( m_msgAllocators[0]->ContainsObject( ( *iter ) ) ) {
			m_msgAllocators[0]->Free<RenderMessage>( ( *iter ) );
		} else {
			m_msgAllocators[1]->Free<RenderMessage>( ( *iter ) );
		}
	}

	delete m_msgAllocators[0];
	delete m_msgAllocators[1];

	delete m_cameraFrustum;
	//delete m_renderableQuadTree;
}
/*
=============
RenderModule::RenderModule

	Creates a render module based on what typeof renderer the engine requires.
=============
*/
RenderModule* RenderModule::Create( void ) {
	RunningInfo* args = Engine::GetInstance()->GetRunningInfo();

	RendererType::Value renderer = args->rendererType;
	
	if ( renderer == RendererType::OPENGL ) {
		return OpenGLRenderModule::CreateOpenGLRenderer();
	}

	return NULL;
}

void RenderModule::Init( void ) {	
	m_camera = new CameraProjectionData();
	m_renderObjectsMutex = new sf::Mutex();

	m_cameraFrustum = new Frustum();
	//m_renderableQuadTree = SpacePartitionTree<RenderableModel>::CreateEmptyTree( &RenderableModel::GetBoundingBox );

	m_msgAllocators[0] = SmallBlockAllocator::CreateAllocatorWithSize( MESSAGE_POOL_ALLOCATOR_SIZE );
	m_msgAllocators[1] = SmallBlockAllocator::CreateAllocatorWithSize( MESSAGE_POOL_ALLOCATOR_SIZE );
}
/*
=============
RenderModule::SendRenderMessage

	Sends a render message to the renderer
=============
*/
void RenderModule::SendRenderMessage( RenderMessage* msg ) {
	m_msgVectorMutex.lock();
	m_messages.push_back( msg );
	m_msgVectorMutex.unlock();
}
/*
=============
RenderModule::ProcessMessages

	Processes all of the messages that were sent.
=============
*/
void RenderModule::ProcessMessages( void ) {
	std::vector<RenderMessage*> messages;
	m_msgVectorMutex.lock();
	messages.swap( m_messages );
	m_msgVectorMutex.unlock();

	m_allocatorMutex.lock();
	m_allocatorIndex = m_allocatorIndex == 0 ? 1 : 0;
	m_allocatorMutex.unlock();

	RenderMessage* msg = NULL;

	for ( std::vector<RenderMessage*>::iterator iter = messages.begin();
		  iter != messages.end(); ++iter ) {

		PROCESS_MESSAGE( TransformMessage )
		PROCESS_MESSAGE( SetParentMessage )
		PROCESS_MESSAGE( CreateMeshMessage )
		PROCESS_MESSAGE( CreateDynamicMeshMessage )
		PROCESS_MESSAGE( CreateModelMessage )
		PROCESS_MESSAGE( UpdateMeshMessage )
		PROCESS_MESSAGE( CreateTextureMessage )
		PROCESS_MESSAGE( CameraTransformMessage )
		PROCESS_MESSAGE( DuplicateModelMessage )
		PROCESS_MESSAGE( VisibilityMessage )
		PROCESS_MESSAGE( OutlineMessage )
		PROCESS_MESSAGE( LightingMessage )
		PROCESS_MESSAGE( UpdateDiffuseColorMessage )
		PROCESS_MESSAGE( UpdateOpacityMessage )
		PROCESS_MESSAGE( DestroyMessage )
		PROCESS_MESSAGE( UpdateCameraProjectionMessage )
		PROCESS_MESSAGE( UpdatePassTypeMessage )
		PROCESS_MESSAGE( AddPassMessage )
		PROCESS_MESSAGE( DeletePassMessage )
		PROCESS_MESSAGE( AddAnimationMessage )
		PROCESS_MESSAGE( InverseBoneMatriciesMessage )
		PROCESS_MESSAGE( PlayAnimationMessage )
		PROCESS_MESSAGE( CreateParticleEmitterMessage )
		PROCESS_MESSAGE( EmitParticlesMessage )
		PROCESS_MESSAGE( UpdateEmitterMessage )
		PROCESS_MESSAGE( SetParentBoneMessage )
		PROCESS_MESSAGE( PlayBlendedAnimationsMessage )
		PROCESS_MESSAGE( PlayAdditiveAnimationMessage )

		char buffer[512];
		sprintf_s( buffer, "Unprocessed message of type %s", ( *iter )->GetRTTI().GetClass() );
		Log::GetInstance()->WriteToLog( "RenderModule", buffer );
		break;
	}
}
/*
=============
RenderModule::FreeMessage

	Frees a message.
=============
*/
void RenderModule::FreeMessage( RenderMessage* msg ) {
	unsigned char freeIndex = m_allocatorIndex == 0 ? 1 : 0;
	if ( m_msgAllocators[freeIndex]->ContainsObject( msg ) ) {
		m_msgAllocators[freeIndex]->Free<RenderMessage>( msg );
	} else {
		m_allocatorMutex.lock();
		m_msgAllocators[m_allocatorIndex]->Free<RenderMessage>( msg );
		m_allocatorMutex.unlock();
	}
}
/*
=============
RenderModule::AddRenderable

	Adds a renderable to the map.
=============
*/
void RenderModule::AddRenderable( const unsigned int id, Renderable* obj ) {
	obj->SetRenderer( this );
	obj->SetNodeID( id );

	m_renderObjectsMutex->lock();
	m_renderObjects[id] = obj;
	m_renderObjectsMutex->unlock();
}
/*
=============
RenderModule::RemoveRenderable

	Removes a renderable from the map.
=============
*/
void RenderModule::RemoveRenderable( const unsigned int id ) {
	m_renderObjectsMutex->lock();
	m_renderObjects.erase(id);
	m_renderObjectsMutex->unlock();
}
/*
=============
RenderModule::ProcessTransformMessages

	Changes a transform
=============
*/
void RenderModule::ProcessTransformMessages( TransformMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		( ( Renderable* )renderable )->UpdateTransform( msg );
	}
}
/*
=============
RenderModule::ProcessCreateMeshMessages

	Creates a mesh
=============
*/
void RenderModule::ProcessCreateMeshMessages( CreateMeshMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	
	if ( renderable ) {
		RenderableModel* model = ( RenderableModel* )renderable;
		model->AttachMesh( GenerateMeshFromMessage( msg, model->GetNextMeshIndex() ) );
	} else {
		Log::GetInstance()->WriteToLog( "RenderModule", "Could not attach mesh to model" );
	}
}
/*
=============
RenderModule::RemoveRenderable

	Removes a renderable from the map.
=============
*/
void RenderModule::ProcessCreateDynamicMeshMessages( CreateDynamicMeshMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		RenderableModel* model = ( RenderableModel* )renderable;
		model->AttachMesh( GenerateDynamicMeshFromMessage( msg, model->GetNextMeshIndex() ) );
	} else {
		Log::GetInstance()->WriteToLog( "RenderModule", "Could not attach mesh to model" );
	}
}
/*
=============
RenderModule::RemoveRenderable

	Removes a renderable from the map.
=============
*/
void RenderModule::ProcessUpdateCameraProjectionMessages( UpdateCameraProjectionMessage* msg ) {
	m_camera->GetProjection().farClipping	 = msg->projection.farClipping;
	m_camera->GetProjection().nearClipping	 = msg->projection.nearClipping;
	m_camera->GetProjection().height		 = msg->projection.height;
	m_camera->GetProjection().width			 = msg->projection.width;
	m_camera->GetProjection().FOV			 = msg->projection.FOV;
	m_camera->GetProjection().projectionType = msg->projection.projectionType;

	m_cameraFrustum->UpdateFrustumInternals( msg->projection.FOV, msg->projection.width / msg->projection.height, msg->projection.nearClipping, msg->projection.farClipping );	
}
/*
=============
RenderModule::RemoveRenderable

	Removes a renderable from the map.
=============
*/
void RenderModule::ProcessUpdateMeshMessages( UpdateMeshMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		UpdateMeshData( msg );
	}
}
/*
=============
RenderModule::RemoveRenderable

	Removes a renderable from the map.
=============
*/
void RenderModule::ProcessCreateTextureMessages( CreateTextureMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		( ( RenderableModel* )renderable )->AttachTexture( GenerateTextureFromMessage( msg ), 0 );
	} else {
		Log::GetInstance()->WriteToLog( "RenderModule", "Could not attach texture to model" );
	}
}
/*
=============
RenderModule::RemoveRenderable

	Removes a renderable from the map.
=============
*/
void RenderModule::ProcessCreateModelMessages( CreateModelMessage* msg ) {
	RenderableModel* model = GenerateModelFromMessage( msg );
	AddRenderable( msg->nodeID, model );
}
/*
=============
RenderModule::RemoveRenderable

	Removes a renderable from the map.
=============
*/
void RenderModule::ProcessDuplicateModelMessages( DuplicateModelMessage* msg ) {
	RenderableModel* duplicate = GenerateDuplicateModelFromMessage( msg );
	if ( duplicate ) {
		AddRenderable( msg->nodeID, duplicate );
	}
}
/*
=============
RenderModule::RemoveRenderable

	Removes a renderable from the map.
=============
*/
void RenderModule::ProcessCameraTransformMessages( CameraTransformMessage* msg ) {
	m_camera->SetViewMatrix( msg->view );
	m_camera->SetProjectionMatrix( msg->projection );
	m_camera->SetPosition( msg->position );
	
	m_cameraFrustum->UpdateFrustum( msg->view, msg->projection, msg->position );
}
/*
=============
RenderModule::RemoveRenderable

	Removes a renderable from the map.
=============
*/
void RenderModule::ProcessVisibilityMessages( VisibilityMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		renderable->SetVisible( msg->visible );
	}
}
/*
=============
RenderModule::RemoveRenderable

	Removes a renderable from the map.
=============
*/
void RenderModule::ProcessOutlineMessages( OutlineMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );

	if ( renderable ) {
		renderable->SetDrawingOutlineOnly( msg->outlineOnly );
	}
}
/*
=============
RenderModule::RemoveRenderable

	Removes a renderable from the map.
=============
*/
void RenderModule::ProcessUpdateOpacityMessages( UpdateOpacityMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		( ( RenderableModel* )renderable )->SetOpacity( msg->opacity );
	}
}
/*
=============
RenderModule::ProcessLightingMessages

	Removes a renderable from the map.
=============
*/
void RenderModule::ProcessLightingMessages( LightingMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		renderable->SetLightingType( msg->lightingType );
		renderable->IsLit( msg->lightingEnabled );
	}
}
/*
=============
RenderModule::ProcessUpdateDiffuseColorMessages

	Updates the diffuse color of a renderable.
=============
*/
void RenderModule::ProcessUpdateDiffuseColorMessages( UpdateDiffuseColorMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		( ( RenderableModel* )renderable )->SetDiffuseColor( msg->diffuseColor );
	}
}
/*
=============
RenderModule::ProcessDestroyMessages

	Destroys a renderable.
=============
*/
void RenderModule::ProcessDestroyMessages( DestroyMessage* msg ) {
	Renderable* r = FindRenderable( msg->nodeID );

	//In render objects
	if ( r ) {
		if ( r->GetParentRenderable() == NULL ) {
			RemoveRenderable( r->GetNodeID() );
		} else {
			r->GetParentRenderable()->RemoveChild( r->GetNodeID() );
		}
	}

	delete r;
}
/*
=============
RenderModule::ProcessSetParentMessages

	Sets the parent of the model.
=============
*/
void RenderModule::ProcessSetParentMessages( SetParentMessage* msg ) {
	Renderable* parentRenderable = FindRenderable( msg->parentID );
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( parentRenderable && renderable ) {
		if ( msg->parentID != 0 ) {
			if ( !renderable->GetParentRenderable() ) {
				RemoveRenderable( renderable->GetNodeID() );
			}
			parentRenderable->AddChild( renderable );
		} else {
			renderable->SetParentRenderable( NULL );
			AddRenderable( msg->nodeID, renderable );
		}
	}
}
/*
=============
RenderModule::ProcessUpdatePassTypeMessages

	Updates which passes should be used for a renderable.
=============
*/
void RenderModule::ProcessUpdatePassTypeMessages( UpdatePassTypeMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		renderable->SetAttachedPass( msg->passes );
	}
}
/*
=============
RenderModule::ProcessAddPassMessages
	
	Adds a pass to the renderer.
=============
*/
void RenderModule::ProcessAddPassMessages( AddPassMessage* msg ) {
	if ( msg->pass->InitializePass( msg->passData ) ) {
		m_passes.insert( msg->pass );
	} else {
		delete msg->pass;
	}

	delete[] msg->passData;
}
/*
=============
RenderModule::ProcessDeletePassMessages

	Deletes a pass.
=============
*/
void RenderModule::ProcessDeletePassMessages( DeletePassMessage* msg ) {
	for ( PassesSet::iterator iter = m_passes.begin();
		    iter != m_passes.end(); ++iter ) {
		if ( ( *iter )->GetRTTI() == *msg->passRtti )  {
			delete ( *iter );
			m_passes.erase( iter );
			break;
		} 
	}
}
/*
=============
RenderModule::ProcessAddAnimationMessages

	Processes an animation message.
=============
*/
void RenderModule::ProcessAddAnimationMessages( AddAnimationMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		( ( RenderableModel* )renderable )->AddAnimation( msg->animation );
	}
}
/*
=============
RenderModule::ProcessAddAnimationMessages

	Processes an animation message.
=============
*/
void RenderModule::ProcessInverseBoneMatriciesMessages( InverseBoneMatriciesMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		( ( RenderableModel* )renderable )->SetBindMatricies( msg->inverseBoneMatricies, msg->matrixCount );
	}
}
/*
=============
RenderModule::ProcessPlayAnimationMessages

	Processes a play animation message.
=============
*/
void RenderModule::ProcessPlayAnimationMessages( PlayAnimationMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		Animator* animator = ( ( RenderableModel* )renderable )->GetAnimator();
		if ( animator ) {
			animator->PlayAnimation( msg->animationNameHash, msg->loopAmount, msg->transitionTime, msg->playbackSpeed );
		}
	}
}
/*
=============
RenderModule::ProcessPlayAdditiveAnimationMessages

	Processes a play additive animation message.
=============
*/
void RenderModule::ProcessPlayAdditiveAnimationMessages( PlayAdditiveAnimationMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		Animator* animator = ( ( RenderableModel* )renderable )->GetAnimator();
		if ( animator ) {
			animator->PlayAdditiveAnimation( msg->animationNameHash, msg->loopAmount, msg->influence, msg->playbackSpeed );
		}
	}
}
/*
=============
RenderModule::ProcessCreateParticleEmitterMessages

	Processes a create emmitter message
=============
*/
void RenderModule::ProcessCreateParticleEmitterMessages( CreateParticleEmitterMessage* message ) {
	AddRenderable( message->nodeID, GenerateEmitterFromMessage( message ) );
}
/*
=============
RenderModule::EmitParticlesMessage

	Starts an emitter
=============
*/
void RenderModule::ProcessEmitParticlesMessages( EmitParticlesMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		( ( ParticleEmitter* )renderable )->Emit( true, msg->loopAmount );
	}
}
/*
=============
RenderModule::ProcessUpdateEmitterMessage

	Processes an update emitter message.
=============
*/
void RenderModule::ProcessUpdateEmitterMessages( UpdateEmitterMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		( ( ParticleEmitter* )renderable )->UpdateSettings( msg->settings );
	}
}
/*
=============
RenderModule::ProcessPlayBlendedAnimationsMessages

	Processes an play blended animation message.
=============
*/
void RenderModule::ProcessPlayBlendedAnimationsMessages( PlayBlendedAnimationsMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		( ( RenderableModel* )renderable )->GetAnimator()->PlayBlendedAnimations( msg );
	}
}
/*
=============
RenderModule::ProcessPlayBlendedAnimationsMessages

	Processes an play blended animation message.
=============
*/
void RenderModule::ProcessSetParentBoneMessages( SetParentBoneMessage* msg ) {
	Renderable* renderable = FindRenderable( msg->nodeID );
	if ( renderable ) {
		( ( RenderableModel* )renderable )->SetMatchBoneHash( msg->nameHash );
	}
}
/*
=============
RenderModule::Shutdown

	Deletes all passes.
	Shuts down.
=============
*/
void RenderModule::Shutdown( void ) {
	for ( std::multiset<RenderPass*>::iterator iter = m_passes.begin();
		  iter != m_passes.end(); ++iter ) {
		delete ( *iter );
	}
	
	m_passes.clear(); 
}
/*
=============
RenderModule::QueryAnimationStatus

	Querys the status of an animation.
=============
*/
AnimationStatus::Value RenderModule::QueryAnimationStatus( unsigned int id ) {
	Renderable* renderable = FindRenderable( id );
	if ( renderable ) {
		Animator* animator = ( ( RenderableModel* )renderable )->GetAnimator();
		if ( animator ) {
			return animator->GetAnimationStatus();
		}
	}

	return AnimationStatus::STOPPED;
}
/*
=============
RenderModule::QueryEmittingStatus

	Querys the status of an animation.
=============
*/
bool RenderModule::QueryEmittingStatus( unsigned int id ) {
	Renderable* renderable = FindRenderable( id );
	if ( renderable ) {
		return ( ( ParticleEmitter* )renderable )->IsEmitting();
	}

	return false;
}
/*
=============
RenderModule::FindRenderable

	Finds a renderable
=============
*/
Renderable* RenderModule::FindRenderable( unsigned int id ) {
	m_renderObjectsMutex->lock();

	RenderableMap::iterator iter = m_renderObjects.find( id );
	if ( iter != m_renderObjects.end() ) {
		Renderable* ret = iter->second;
		m_renderObjectsMutex->unlock();
		return ret;
	}
	
	Renderable* retPtr = NULL;

	for ( RenderableMap::iterator iter = m_renderObjects.begin();
		  iter != m_renderObjects.end(); ++iter ) {
		retPtr = iter->second->GetChildRenderable( id );
		if ( retPtr ) {
			break;
		}
	}

	m_renderObjectsMutex->unlock();

	return retPtr;
}

}