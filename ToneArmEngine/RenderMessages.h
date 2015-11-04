/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RenderMessages

		Messages used for sending information between threads.

		Created by: Karl Merten
		Created on: 5/8/2014 12:59:03 PM

========================
*/

#ifndef __RENDERMESSAGES_H__
#define __RENDERMESSAGES_H__

#include "MathHelper.h"
#include "Renderable.h"
#include "RenderableMesh.h"
#include "Material.h"
#include "RenderPass.h"
#include "Camera.h"
#include "RTTI.h"
#include "ParticleEmitter.h"
#include <GL\glew.h>
#include <cstdarg>

#define SEND_RENDER_MESSAGE( __msgType__, __args__ )																	\
	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );		\
	__msgType__* msg = renderer->GetRenderMessage<__msgType__>();														\
	msg->Init( __args__ );															\
	renderer->SendRenderMessage( msg );																					\

namespace vgs
{
	class Animation;
	
	/*
	========================

		RenderMessage

			Base class for render messages

			Created by: Karl Merten
			Created on: 5/8/2014 12:59:03 PM

	========================
	*/
	struct RenderMessage {
		DECLARE_RTTI
		
		RenderMessage( void ) :
			nodeID( 0 )
		{}

		RenderMessage( const unsigned int id ) : 
			nodeID( id )
		{}

		RenderMessage( const RenderMessage& other ) : 
			nodeID( other.nodeID )
		{}

		virtual void Init( unsigned int id ) {
			nodeID = id;
		}
		
		unsigned int nodeID;
	};
	/*
	========================

		SetParentMessage

			Parents nodes together to allow for composite transforms.

			Created by: Karl Merten
			Created on: 18/07/2014

	========================
	*/
	struct SetParentMessage : public RenderMessage {
		SetParentMessage( void ) :
			parentID( 0 )
		{}

		SetParentMessage( const unsigned int id, unsigned int parentID ) : 
			RenderMessage( id ),
			parentID( parentID )
		{}

		SetParentMessage( const SetParentMessage& other ) : 
			RenderMessage( other ),
			parentID( other.nodeID )
		{}

		virtual void Init( unsigned int id, unsigned int parentID ) {
			nodeID = id;
			this->parentID = parentID;
		}
		
		unsigned int parentID;
	};
	/*
	========================

		SetInverseBoneMatricies

			Set the inverseBoneMatricies of a mode.

			Created by: Karl Merten
			Created on: 21/07/2014

	========================
	*/
	struct InverseBoneMatriciesMessage : public RenderMessage {
		DECLARE_RTTI

		InverseBoneMatriciesMessage( void ) :
			inverseBoneMatricies( NULL )
		{}

		void Init( unsigned int id, glm::mat4* matricies, unsigned int count ) {
			nodeID = id;
			inverseBoneMatricies = matricies;
			matrixCount = count;
		}

		glm::mat4* inverseBoneMatricies;
		unsigned int matrixCount;
	};
	/*
	========================

		TransformMessage

			Message contains node id, type of transform and value of change.

			Created by: Karl Merten
			Created on: 5/8/2014 12:59:03 PM

	========================
	*/
	struct TransformMessage : public RenderMessage
	{
		DECLARE_RTTI
		
		TransformMessage( void ) : 
			rotationMatrix( 0.0f ),
			position( 0.0f )
		{}

		TransformMessage( const unsigned int id, const glm::mat3& val, const glm::vec3& position ) : 
			RenderMessage( id ),
			rotationMatrix( val ),
			position( position )
		{}

		TransformMessage( const TransformMessage& other ) : 
			RenderMessage( other ),
			rotationMatrix( other.rotationMatrix ),
			position( other.position )
		{}

		virtual void Init( unsigned int id, const glm::mat3& rotMat, const glm::vec3& pos ) {
			nodeID = id;
			rotationMatrix = rotMat;
			position = pos;
		}

		glm::mat3	rotationMatrix;
		glm::vec3	position;
	};
	/*
	========================

		VisibilityMessage

			Message contains node id, and visibility

			Created by: Karl Merten.
			Created on: 28/05/2014

	========================
	*/
	struct VisibilityMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		VisibilityMessage( void ) : 
			visible( true )
		{}

		VisibilityMessage( unsigned int id, bool visible ) : 
			RenderMessage( id ),
			visible( visible )
		{}

		VisibilityMessage( const VisibilityMessage& other ) : 
			RenderMessage( other ),
			visible( other.visible )
		{}

		virtual void Init( unsigned int id, bool visible ) {
			nodeID = id;
			this->visible = visible;
		}

		bool visible;
	};
	/*
	========================

		UpdateOpacityMessage

			Message contains node id, and opacity

			Created by: Karl Merten.
			Created on: 06/11/2014

	========================
	*/
	struct UpdateOpacityMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		UpdateOpacityMessage( void ) : 
			opacity( 1.0f )
		{}

		UpdateOpacityMessage( const unsigned int id, float alpha ) : 
			RenderMessage( id ),
			opacity( alpha )
		{}

		UpdateOpacityMessage( const UpdateOpacityMessage& other ) : 
			RenderMessage( other ),
			opacity( other.opacity )
		{}

		virtual void Init( const unsigned int id, float alpha ) {
			nodeID = id;
			opacity = alpha;
		}

		float opacity;
	};
	/*
	========================

		UpdateDiffuseColorMessage

			Message contains node id, and color

			Created by: Karl Merten.
			Created on: 06/06/2014

	========================
	*/
	struct UpdateDiffuseColorMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		UpdateDiffuseColorMessage( void ) : 
			diffuseColor( 1.0f, 0.0f, 0.0f )
		{}

		UpdateDiffuseColorMessage( const unsigned int id, const glm::vec3& color ) : 
			RenderMessage( id ),
			diffuseColor( color )
		{}

		UpdateDiffuseColorMessage( const UpdateDiffuseColorMessage& other ) : 
			RenderMessage( other ),
			diffuseColor( other.diffuseColor )
		{}

		virtual void Init( const unsigned int id, const glm::vec3& color ) {
			nodeID = id;
			diffuseColor = color;
		}

		glm::vec3 diffuseColor;
	};
	/*
	========================

		OutlineMessage

			Message contains node id, and outine

			Created by: Karl Merten.
			Created on: 28/05/2014

	========================
	*/
	struct OutlineMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		OutlineMessage( void ) : 
			outlineOnly( true )
		{}

		OutlineMessage( unsigned int id, bool outlineOnly ) : 
			RenderMessage( id ),
			outlineOnly( outlineOnly )
		{}

		OutlineMessage( const OutlineMessage& other ) : 
			RenderMessage( other ),
			outlineOnly( other.outlineOnly )
		{}

		virtual void Init( unsigned int id, bool outlineOnly ) {
			nodeID = id;
			this->outlineOnly = outlineOnly;
		}

		bool outlineOnly;
	};
	/*
	========================

		LightingMessage

			Message contains node id, and lighting enabled

			Created by: Karl Merten.
			Created on: 28/05/2014

	========================
	*/
	struct LightingMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		LightingMessage( void ) : 
			lightingEnabled( true ),
			lightingType( LightingType::VERTEX_LIT )
		{}

		LightingMessage( unsigned int nodeID, bool lighting, LightingType::Value type ) : 
			RenderMessage( nodeID ),
			lightingEnabled( lighting ),
			lightingType( type )
		{}

		LightingMessage( const LightingMessage& other ) : 
			RenderMessage( other ),
			lightingEnabled( other.lightingEnabled ),
			lightingType( other.lightingType )
		{}

		virtual void Init( unsigned int id, bool lighting, LightingType::Value type ) {
			nodeID = id;
			lightingEnabled = lighting;
			lightingType = type;
		}

		bool				lightingEnabled;
		LightingType::Value lightingType;
	};
	/*
	========================

		CameraTransformMessage

			Message contains view and projection matricies of a camera.

			Created by: Karl Merten
			Created on: 5/8/2014 12:59:03 PM

	========================
	*/
	struct CameraTransformMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		CameraTransformMessage() : 
			view( glm::mat4( 0.0f ) ), 
			projection( glm::mat4( 0.0f ) )
		{}

		CameraTransformMessage( unsigned int id, const glm::mat4& viewM, const glm::mat4& projM, const glm::vec3& pos ) :
			RenderMessage( id ),
			view( viewM ),
			projection( projM ),
			position( pos )
		{}

		CameraTransformMessage( const CameraTransformMessage& other ) :
			RenderMessage( other ),
			view( other.view ),
			projection( other.projection ),
			position( other.position )
		{}			

		virtual void Init( unsigned int id, const glm::mat4& viewM, const glm::mat4& projM, const glm::vec3& pos ) {
			nodeID = id;
			view = viewM;
			projection	 = projM;
			position = pos;
		}

		glm::mat4 view;
		glm::mat4 projection;
		glm::vec3 position;
	};

	/*
	========================

		DuplicateModelMessage

			Duplicates a model

			Created by: Karl Merten.
			Created on: 26/05/2014

	========================
	*/
	struct DuplicateModelMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		DuplicateModelMessage() :
			originalModelID( 0 )
		{}

		DuplicateModelMessage( unsigned int id, unsigned int originalID ) :
			RenderMessage( id ),
			originalModelID( originalID )
		{}

		DuplicateModelMessage( const DuplicateModelMessage& other ) :
			RenderMessage( other ),
			originalModelID( other.originalModelID )
		{}

		virtual void Init( unsigned int id, unsigned int originalID ) {
			nodeID = id;
			originalModelID = originalID;
		}

		unsigned int originalModelID;
	};

	/*
	========================

		CreateMessage

			Message contains type of resource and name of it.

			Created by: Karl Merten
			Created on: 5/8/2014 12:59:03 PM

	========================
	*/
	struct CreateMeshMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		CreateMeshMessage( void ) :
			verticies( NULL ),
			vertexCount( 0 ),
			indicies( NULL ),
			indexCount( 0 ),
			indexType( GL_UNSIGNED_SHORT ),
			vertexComponents( VertexComponents::POSITION ),
			diffuseColor( 1.0f, 1.0f, 1.0f )
		{}

		CreateMeshMessage( unsigned int id, float* verticies, unsigned int vertexCount, void* indicies, unsigned int indexCount, GLenum indexType, const glm::vec3& diffColor, VertexComponents::Value vertexComps ) :
			RenderMessage( id ),
			verticies( verticies ),
			vertexCount( vertexCount ),
			indicies( indicies ),
			indexCount( indexCount ),
			indexType( indexType ),
			vertexComponents( vertexComps ),
			diffuseColor( diffuseColor )
		{}

		CreateMeshMessage( const CreateMeshMessage& other ) :
			RenderMessage( other ),
			verticies( other.verticies ),
			vertexCount( other.vertexCount ),
			indicies( other.indicies ),
			indexCount( other.indexCount ),
			indexType( other.indexType ),
			vertexComponents( other.vertexComponents ),
			diffuseColor( other.diffuseColor )
		{}

		virtual void Init( unsigned int id, float* verticies, unsigned int vertexCount, void* indicies, unsigned int indexCount, GLenum indexType, VertexComponents::Value vertexComps, const glm::vec3& diffColor, const glm::vec3& bbOff, const glm::vec3& bbSize ) {
			nodeID = id;
			this->verticies = verticies;
			this->vertexCount = vertexCount;
			this->indicies = indicies;
			this->indexCount = indexCount;
			this->indexType = indexType;
			diffuseColor = diffColor;
			vertexComponents = vertexComps;
			
			boundingBoxOffset = bbOff;
			boundingBoxSize = bbSize;
		}

		float*						verticies;
		unsigned int				vertexCount;

		void*						indicies;
		unsigned int				indexCount;
		GLenum						indexType;

		glm::vec3					diffuseColor;
		VertexComponents::Value		vertexComponents;

		glm::vec3					boundingBoxOffset;
		glm::vec3					boundingBoxSize;

	};
	/*
	========================

		CreateDynamicMeshMessage

			Creates a dynamic mesh.

			Created by: Karl Merten
			Created on: 10/07/2014

	========================
	*/
	struct CreateDynamicMeshMessage : public CreateMeshMessage {
		DECLARE_RTTI
	};
	/*
	========================

		UpdateMeshMessage

			Message  for updating mesh data

			Created by: Karl Merten.
			Created on: 04/06/2014

	========================
	*/
	struct UpdateMeshMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		UpdateMeshMessage( void ) :
			verticies( NULL ),
			vertexCount( 0 ),
			indicies( NULL ),
			indexCount( 0 )
		{}

		UpdateMeshMessage( unsigned int id, float* verts, unsigned int vertCount, void* indicies, unsigned int iCount ) :
			RenderMessage( id ),
			verticies( verts ),
			vertexCount( vertCount ),
			indicies( indicies ),
			indexCount( iCount )
		{}

		UpdateMeshMessage( const UpdateMeshMessage& other ) :
			RenderMessage( other ),
			verticies( other.verticies ),
			vertexCount( other.vertexCount ),
			indicies( other.indicies ),
			indexCount( other.indexCount )
		{}
		
		virtual void Init( unsigned int id, float* verts, unsigned int vertCount, void* indicies, unsigned int iCount, const glm::vec3& bbPos, const glm::vec3& bbSize ) {
			nodeID = id;
			verticies = verts;
			vertexCount = vertCount;
			this->indicies = indicies;
			indexCount = iCount;

			boundBoxOffset = bbPos;
			boundingBoxSize = bbSize;
		}

		float*			verticies;
		unsigned int	vertexCount;

		void*			indicies;
		unsigned int	indexCount;
		
		glm::vec3		boundBoxOffset;
		glm::vec3		boundingBoxSize;
	};
	/*
	========================

		CreateModelMessage

			Message contains type of resource and name of it.

			Created by: Karl Merten
			Created on: 5/8/2014 12:59:03 PM

	========================
	*/
	struct CreateModelMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		CreateModelMessage( void ) :
			meshCount( 0 )
		{}

		CreateModelMessage( unsigned int id, unsigned int meshCount ) :
			RenderMessage( id ),
			meshCount( meshCount )
		{}

		CreateModelMessage( const CreateModelMessage& other ) :
			RenderMessage( other ),
			meshCount( other.meshCount )
		{}

		virtual void Init( unsigned int id, unsigned int meshCount ) {
			nodeID = id;
			this->meshCount = meshCount;
		}

		unsigned int meshCount;
	};

	/*
	========================

		CreateTextureMessage

			Tells a model to attach a texture with the components and byte buffer

			Created by: Karl Merten.
			Created on: 19/05/14

	========================
	*/
	struct CreateTextureMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		CreateTextureMessage( void ) :
			data( NULL ),
			bytesPerPixel( 0 ),
			textureHeight( 0 ),
			textureWidth( 0 ),
			textureType( TextureType::DIFFUSE )
		{}

		CreateTextureMessage( unsigned int id, char* data, char bpp, unsigned int w, unsigned int h, TextureType::Value texType ) :
			RenderMessage( id ),
			data( data ),
			bytesPerPixel( bpp ),
			textureWidth( w ),
			textureHeight( h ),
			textureType( texType )
		{}

		CreateTextureMessage( const CreateTextureMessage& other ) :
			RenderMessage( other ),
			data( other.data ),
			bytesPerPixel( other.bytesPerPixel ),
			textureWidth( other.textureWidth ),
			textureHeight( other.textureHeight ),
			textureType( other.textureType )
		{}

		virtual void Init( unsigned int id, char* data, char bpp, unsigned int w, unsigned int h, TextureType::Value texType ) {
			nodeID = id;
			this->data = data;
			bytesPerPixel = bpp;
			textureWidth = w;
			textureHeight = h;
			textureType = texType;
		}

		char*				data;

		char				bytesPerPixel;

		unsigned int		textureWidth;
		unsigned int		textureHeight;

		TextureType::Value	textureType;
	};

	/*
	========================

		DestroyMessage

			Message contains id of resource to destroy.

			Created by: Karl Merten
			Created on: 5/8/2014 12:59:03 PM

	========================
	*/
	struct DestroyMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		DestroyMessage( void )
		{}

		DestroyMessage( const DestroyMessage& other ) :
			RenderMessage( other )
		{}
	};
	/*
	========================

		UpdatePassTypeMessage

			Message to set which passes affect an object

			Created by: Karl Merten.
			Created on: 01/07/2014

	========================
	*/
	struct UpdatePassTypeMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		UpdatePassTypeMessage( void ) : 
			passes( PassType::DIFFUSE )
		{}

		UpdatePassTypeMessage( unsigned int id, PassType::Value pass ) : 
			RenderMessage( id ),
			passes( pass )
		{}

		UpdatePassTypeMessage( const UpdatePassTypeMessage& other ) : 
			RenderMessage( other ),
			passes( other.passes )
		{}

		virtual void Init( unsigned int id, PassType::Value pass ) {
			nodeID = id;
			passes = pass;
		}

		PassType::Value	passes;
	};
	/*
	========================

		AddPassMessage

			Adds a render pass

			Created by: Karl Merten.
			Created on: 01/07/2014

	========================
	*/
	struct AddPassMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		AddPassMessage( void ) : 
			pass( NULL ),
			passData( NULL )
		{}

		AddPassMessage( unsigned int id, RenderPass* pass ) : 
			RenderMessage( id ),
			pass( pass )
		{}

		AddPassMessage( const AddPassMessage& other ) : 
			RenderMessage( other ),
			pass( other.pass )
		{}

		virtual void Init( RenderPass* pass, void* passData = NULL ) {
			this->pass = pass;
			this->passData = passData;
		}

		RenderPass* pass;
		void*		passData;
	};
	/*
	========================

		DeletePassMessage

			Deletes a render pass

			Created by: Karl Merten.
			Created on: 01/07/2014

	========================
	*/
	struct DeletePassMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		DeletePassMessage( void ) : 
			passRtti( NULL )
		{}

		DeletePassMessage( unsigned int id, RTTI* pass ) : 
			RenderMessage( id ),
			passRtti( pass )
		{}

		DeletePassMessage( const DeletePassMessage& other ) : 
			RenderMessage( other ),
			passRtti( other.passRtti )
		{}

		virtual void Init( unsigned int id, const RTTI* pass ) {
			nodeID = id;
			passRtti = pass;
		}

		const RTTI*	passRtti;
	};
	/*
	========================

		UpdateCameraProjectionMessage

			Updates the camera's projection

			Created by: Karl Merten.
			Created on: 04/07/2014

	========================
	*/
	struct UpdateCameraProjectionMessage : public RenderMessage
	{
		DECLARE_RTTI
	
		UpdateCameraProjectionMessage( void )
		{}

		UpdateCameraProjectionMessage( const Projection& projection ) : 
			projection( projection )
		{}

		UpdateCameraProjectionMessage( unsigned int id, float w, float h, float n, float f, float fov, CameraProjection::Value p ) :
			RenderMessage( id ),
			projection( w, h, n, f, fov, p )
		{}


		UpdateCameraProjectionMessage( const UpdateCameraProjectionMessage& other ) : 
			RenderMessage( other ),
			projection( other.projection )
		{}

		virtual void Init( float w, float h, float n, float f, float fov, CameraProjection::Value p ) {
			projection.width = w;
			projection.height = h;
			projection.farClipping = f;
			projection.nearClipping = n;
			projection.FOV = fov;
			projection.projectionType = p;
		}

		virtual void Init( const Projection& proj ) {
			projection = proj;
		}

		Projection	projection;
	};
	/*
	========================

		AddAnimationMessage

			Adds an animation to the node.

			Created by: Karl Merten.
			Created on: 18/07/2014

	========================
	*/
	struct AddAnimationMessage : public RenderMessage {
		DECLARE_RTTI
	
		AddAnimationMessage( void ) :
			animation( NULL )
		{}

		AddAnimationMessage( unsigned int id, Animation* anim ) : 
			RenderMessage( id ),
			animation( anim )
		{}

		AddAnimationMessage( const AddAnimationMessage& other ) : 
			RenderMessage( other ),
			animation( other.animation )
		{}

		virtual void Init( unsigned int id, Animation* anim ) {
			nodeID = id;
			animation = anim;
		}

		Animation* animation;
	};
	/*
	========================

		PlayAnimationMessage

			Plays an animation by name.

			Created by: Karl Merten
			Created on: 07/22/2014

	========================
	*/
	struct PlayAnimationMessage : public RenderMessage {
		DECLARE_RTTI

		PlayAnimationMessage( void ) :
			animationNameHash( 0 )
		{}

		PlayAnimationMessage( unsigned int id, unsigned int nameHash ) :
			RenderMessage( id ),
			animationNameHash( nameHash )
		{}

		PlayAnimationMessage( const PlayAnimationMessage& other ) :
			RenderMessage( other ),
			animationNameHash( other.animationNameHash )
		{}

		virtual void Init( unsigned int id, unsigned long nameHash, unsigned int loopAmount, float transitionTime, float pbSpeed ) {
			nodeID					= id;
			animationNameHash		= nameHash;
			this->loopAmount		= loopAmount;
			this->transitionTime	= transitionTime;
			playbackSpeed			= pbSpeed;
		}

		virtual void Init( unsigned int id, const char* animation, unsigned int loopAmount = -1, float transitionTime = 0.2f, float playbackSpeed = 1.0f ) {
			Init( id, StringUtils::Hash( animation ), loopAmount, transitionTime, playbackSpeed );
		}

		unsigned long	animationNameHash;
		unsigned int	loopAmount;
		float			transitionTime;
		float			playbackSpeed;
	};
	/*
	========================

		PlayAdditiveAnimation

			Plays an additive animation by name.

			Created by: Karl Merten
			Created on: 07/22/2014

	========================
	*/
	struct PlayAdditiveAnimationMessage : public RenderMessage {
		DECLARE_RTTI

		PlayAdditiveAnimationMessage( void ) :
			animationNameHash( 0 )
		{}

		PlayAdditiveAnimationMessage( unsigned int id, unsigned int nameHash ) :
			RenderMessage( id ),
			animationNameHash( nameHash )
		{}

		PlayAdditiveAnimationMessage( const PlayAdditiveAnimationMessage& other ) :
			RenderMessage( other ),
			animationNameHash( other.animationNameHash )
		{}

		virtual void Init( unsigned int id, unsigned long nameHash, int loopAmount, float influence, float speed ) {
			nodeID					= id;
			animationNameHash		= nameHash;
			this->loopAmount		= loopAmount;
			this->influence			= influence;
			playbackSpeed			= speed;
		}

		virtual void Init( unsigned int id, const char* animation, int loopAmount = -1, float influence = 0.2f, float speed = 1.0f ) {
			Init( id, StringUtils::Hash( animation ), loopAmount, influence, speed );
		}

		unsigned long	animationNameHash;
		int				loopAmount;
		float			playbackSpeed;
		float			influence;
	};
	/*
	========================

		PlayBlendedAnimationsMessage

			Plays blended animation by name( up to 5 ).

			Created by: Karl Merten
			Created on: 07/22/2014

	========================
	*/
	struct PlayBlendedAnimationsMessage : public RenderMessage {
		DECLARE_RTTI

		PlayBlendedAnimationsMessage( void ) :
			transitionTime( 0.0f ),
			animationCount( 0 )
		{
			for ( int i = 0; i < 5; ++i ) {
				animHash[i] = 0;
				loopAmount[i] = 0;
				influences[i] = 0.0f;
			}
		}

		PlayBlendedAnimationsMessage( const PlayBlendedAnimationsMessage& other ) :
			RenderMessage( other ),
			animationCount( other.animationCount ),
			transitionTime( other.transitionTime )
		{
			for ( int i = 0; i < 5; ++i ) {
				animHash[i] = other.animHash[i];
				loopAmount[i] = other.loopAmount[i];
				influences[i] = other.influences[i];
			}
		}

		/*
		===========
		PlayBlendedAnimationsMessage::Init

			FORMAT: 1st id 
					2nd transitionTime
					3rd animationCount
					
					( ulong hash, float influence, int loopCount, float speed )
		===========
		*/
		virtual void Init( unsigned int id, float transitionTime, unsigned int count, va_list args ) {
			nodeID = id;
			this->transitionTime = transitionTime;
			animationCount = count;

			for ( unsigned int i = 0; i < count && i < 5; ++i ) {
				animHash[i] = va_arg( args, unsigned long );
				influences[i] = ( float )va_arg( args, double );
				loopAmount[i] = va_arg( args, int );
				playbackSpeeds[i] = ( float )va_arg( args, double );
			}
		}

		unsigned int	animationCount;
		
		unsigned long	animHash[5];
		float			influences[5];
		float			playbackSpeeds[5];
		int				loopAmount[5];

		float			transitionTime;
	};
	/*
	========================

		CreateParticleEmitterMessage

			Creates a particle emitter.

			Created by: Karl Merten
			Created on: 24/07/2014

	========================
	*/
	struct CreateParticleEmitterMessage : public RenderMessage {
		DECLARE_RTTI

		CreateParticleEmitterMessage( void ) :
			RenderMessage()
		{}

		CreateParticleEmitterMessage( unsigned int id, const ParticleEmitterSettings& settings ) :
			RenderMessage( id ),
			settings( settings )
		{}

		CreateParticleEmitterMessage( const CreateParticleEmitterMessage& other ) :
			RenderMessage( other ),
			settings( other.settings )
		{}

		virtual void Init( unsigned int id, const ParticleEmitterSettings& settings ) {
			nodeID			= id;
			this->settings  = settings;
		}

		ParticleEmitterSettings settings;
	};
	/*
	========================

		UpdateEmitterMessage

			Updates a particle emitter.

			Created by: Karl Merten
			Created on: 24/07/2014

	========================
	*/
	struct UpdateEmitterMessage : public RenderMessage {
		DECLARE_RTTI

		UpdateEmitterMessage( void ) :
			RenderMessage()
		{}

		UpdateEmitterMessage( unsigned int id, const ParticleEmitterSettings& settings ) :
			RenderMessage( id ),
			settings( settings )
		{}

		UpdateEmitterMessage( const UpdateEmitterMessage& other ) :
			RenderMessage( other ),
			settings( other.settings )
		{}

		virtual void Init( unsigned int id, const ParticleEmitterSettings& settings ) {
			nodeID			= id;
			this->settings  = settings;
		}

		ParticleEmitterSettings settings;
	};
	/*
	========================

		EmitParticles

			Emmits a set looped amount of particles.

			Created by: Karl Merten
			Created on: 24/07/2014

	========================
	*/
	struct EmitParticlesMessage : public RenderMessage {
		DECLARE_RTTI

		EmitParticlesMessage( void ) :
			loopAmount( 0 )
		{}

		EmitParticlesMessage( unsigned int id, int amount ) :
			RenderMessage( id ),
			loopAmount( amount )
		{}

		EmitParticlesMessage( const EmitParticlesMessage& other ) :
			RenderMessage( other ),
			loopAmount( other.loopAmount )
		{}

		virtual void Init( unsigned int id, int loop ) {
			nodeID		= id;
			loopAmount  = loop;
		}

		int loopAmount;
	};
	/*
	========================

		SetParentBoneMessage

			Sets the parent bone of a renderable

			Created by: Karl Merten
			Created on: 05/08/2014

	========================
	*/
	struct SetParentBoneMessage : public RenderMessage {
		DECLARE_RTTI

		SetParentBoneMessage( void ) :
			nameHash( 0 )
		{}

		SetParentBoneMessage( unsigned int id, unsigned long hash ) :
			RenderMessage( id ),
			nameHash( hash )
		{}

		SetParentBoneMessage( const SetParentBoneMessage& other ) :
			RenderMessage( other ),
			nameHash( other.nameHash )
		{}

		virtual void Init( unsigned int id, unsigned long hash ) {
			nodeID		= id;
			nameHash	= hash;
		}

		unsigned long nameHash;
	};
} // namespace vgs

#endif //__RENDERMESSAGES_H__