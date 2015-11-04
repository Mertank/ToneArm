/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RenderModule

		Base class for a module that renders the frame.

		Created by: Karl Merten
		Created on: 25/04/2014

========================
*/

#ifndef __RENDERMODULE_H__
#define __RENDERMODULE_H__

#include "EngineModule.h"
#include "RenderMessages.h"
#include "SmallBlockAllocator.h"
#include "Animator.h"
#include "NetConsts.h"

#include <unordered_map>
#include <set>
#include <glm\glm.hpp>
#include <SFML\System.hpp>

#define MESSAGE_PROCESS_FUNC( __msgName__ ) void Process##__msgName__##s( __msgName__* msgs );

namespace vgs {

	class Renderable;
	class RenderableMesh;
	class RenderableModel;
	class CameraProjectionData;
	class Texture;
	class Camera;
	class GUIManager;
	class MessagePoolManager;
	class RenderPass;
	class Animator;

	template <typename T>
	class SpacePartitionTree;

	class RenderModule : public EngineModule {

		typedef std::unordered_map<unsigned int, Renderable*>	RenderableMap;
		typedef std::multiset<RenderPass*, RenderPass::RenderPassComparator> PassesSet;

	public:
												RenderModule( void );
		virtual									~RenderModule( void );

		static RenderModule*					Create( void );

		void									Init( void );
		void									Shutdown( void );

		void									AddRenderable(const unsigned int id, Renderable* obj);
		void									RemoveRenderable(const unsigned int id);

		virtual RenderableMesh*					GenerateMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex ) = 0;
		virtual RenderableMesh*					GenerateDynamicMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex ) = 0;
		virtual RenderableModel*				GenerateModelFromMessage( CreateModelMessage* msg ) = 0;
		virtual Texture*						GenerateTextureFromMessage( CreateTextureMessage* msg ) = 0;
		virtual RenderableModel*				GenerateDuplicateModelFromMessage( DuplicateModelMessage* msg ) = 0;
		virtual ParticleEmitter*				GenerateEmitterFromMessage( CreateParticleEmitterMessage* msg ) = 0;
		virtual void							UpdateMeshData( UpdateMeshMessage* msg ) = 0;
		virtual Animator*						CreateAnimator( unsigned int jointCount ) const = 0;

		virtual void							SetClearColor( const glm::vec4& newColor ) = 0;
		virtual void							SetFrameRateLimit( unsigned int newLimit ) = 0;

		CameraProjectionData&					GetCameraData( void ) const { return *m_camera; }
		const Frustum&							GetCameraFrustum( void ) const { return ( *m_cameraFrustum ); }

		void									AttachPass( RenderPass* pass ) { m_passes.insert( pass ); }
		void									ProcessMessages( void );
		AnimationStatus::Value					QueryAnimationStatus( unsigned int id );
		bool									QueryEmittingStatus( unsigned int id );

		template <typename T> T*				GetPass( void );
		template <typename T> T*				RemovePass( void );

		template <typename T> T*				GetRenderMessage( void );
		void									SendRenderMessage( RenderMessage* msg );

#ifdef TONEARM_DEBUG
		virtual unsigned int					GetInstantaniousFrameRate( void ) { return m_instantaniousFrameRate; };
		virtual float							GetAverageFrameRate( void ) { return m_averageFrameRate; };
#endif

	protected:
		sf::Mutex*								m_renderObjectsMutex;
		RenderableMap							m_renderObjects;
		
		CameraProjectionData*					m_camera;
		Frustum*								m_cameraFrustum;

		unsigned int							m_frameRate;

		sf::Mutex								m_msgVectorMutex;
		std::vector<RenderMessage*>				m_messages;

		SmallBlockAllocator*					m_msgAllocators[2];
		unsigned char							m_allocatorIndex;
		sf::Mutex								m_allocatorMutex;

		SpacePartitionTree<RenderableModel>*	m_renderableQuadTree;

		//MessagePoolManager*				m_poolMgr;
		PassesSet								m_passes;

												MESSAGE_PROCESS_FUNC( TransformMessage )
												MESSAGE_PROCESS_FUNC( CreateMeshMessage )
												MESSAGE_PROCESS_FUNC( CreateDynamicMeshMessage )
												MESSAGE_PROCESS_FUNC( CreateModelMessage )
												MESSAGE_PROCESS_FUNC( UpdateMeshMessage )
												MESSAGE_PROCESS_FUNC( CreateTextureMessage )
												MESSAGE_PROCESS_FUNC( CameraTransformMessage )
												MESSAGE_PROCESS_FUNC( DuplicateModelMessage )
												MESSAGE_PROCESS_FUNC( VisibilityMessage )
												MESSAGE_PROCESS_FUNC( OutlineMessage )
												MESSAGE_PROCESS_FUNC( LightingMessage )
												MESSAGE_PROCESS_FUNC( UpdateDiffuseColorMessage )
												MESSAGE_PROCESS_FUNC( UpdateOpacityMessage )
												MESSAGE_PROCESS_FUNC( DestroyMessage )
												MESSAGE_PROCESS_FUNC( UpdateCameraProjectionMessage )
												MESSAGE_PROCESS_FUNC( UpdatePassTypeMessage )
												MESSAGE_PROCESS_FUNC( AddPassMessage )
												MESSAGE_PROCESS_FUNC( DeletePassMessage )
												MESSAGE_PROCESS_FUNC( SetParentMessage )
												MESSAGE_PROCESS_FUNC( AddAnimationMessage )
												MESSAGE_PROCESS_FUNC( InverseBoneMatriciesMessage )
												MESSAGE_PROCESS_FUNC( PlayAnimationMessage )
												MESSAGE_PROCESS_FUNC( CreateParticleEmitterMessage )
												MESSAGE_PROCESS_FUNC( EmitParticlesMessage )
												MESSAGE_PROCESS_FUNC( UpdateEmitterMessage )
												MESSAGE_PROCESS_FUNC( PlayBlendedAnimationsMessage )
												MESSAGE_PROCESS_FUNC( SetParentBoneMessage )
												MESSAGE_PROCESS_FUNC( PlayAdditiveAnimationMessage )
		
		void									FreeMessage( RenderMessage* msg );

#ifdef TONEARM_DEBUG
		unsigned int							m_instantaniousFrameRate;
		float									m_averageFrameRate;
		float									m_averageTimeElapsed;
		unsigned int							m_averageFrameRateSum;
		unsigned int							m_frameCount;
#endif
		Renderable*								FindRenderable( unsigned int id );

		Tick									m_tick;
		bool									m_captureInput;

	};

	/*template <typename T>
	T* RenderModule::SendMessage(const char* poolName)
	{
		return m_poolMgr->GetMessageByName<T>(poolName);
	}*/

	template <typename T>
	T* RenderModule::GetPass( void ) {
		for ( std::multiset<RenderPass*>::iterator iter = m_passes.begin();
			  iter != m_passes.end(); ++iter ) {
			if ( ( *iter )->GetRTTI() == T::rtti ) {
				return ( T* )( *iter );
			}
		}

		return NULL;
	}

	template <typename T>
	T* RenderModule::GetRenderMessage( void ) {
		m_allocatorMutex.lock();
		T* retPtr = m_msgAllocators[m_allocatorIndex]->Alloc<T>();
#ifdef TONEARM_DEBUG
		/*if ( !m_msgAllocators[m_allocatorIndex]->ContainsObject( retPtr ) ) {
			printf( "Allocator could not find space for %s!\n", retPtr->GetRTTI().GetClass() );
		}*/
#endif
		m_allocatorMutex.unlock();

		return retPtr;
	}
} //namespace vgs

#endif //__RENDERMODULE_H__