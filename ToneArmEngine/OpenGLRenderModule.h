/*
========================

Copyright (c) 2014 Vinyl Games Studio

	OpenGLRenderModule

		An OpenGL specific implementation of RenderModule.

		Created by: Karl Merten
		Created on: 25/04/2014

========================
*/

#ifndef __OPENGLRENDERMODULE_H__
#define __OPENGLRENDERMODULE_H__

#include "RenderModule.h"

namespace sf {
	class Thread;
	class Window;
	class Mutex;
}

namespace vgs {

	struct OpenGLRendererSettings;
	struct TransformMessage;
	
	class  OpenGLBufferData;

	class OpenGLRenderModule : public RenderModule {
	public:
		virtual							~OpenGLRenderModule( void );

		static OpenGLRenderModule*		CreateOpenGLRenderer( void );
	
		virtual void					Startup( void );
		virtual void					Update( float dt );
		virtual void					Shutdown( void );

		virtual void					SetClearColor( const glm::vec4& newColor );
		virtual void					SetFrameRateLimit( unsigned int newLimit );

		glm::vec4						GetClearColor( void );
		unsigned int					GetFrameRateLimit( void );

		inline sf::Window*				GetWindow( void ) { return m_window; }

		virtual RenderableMesh*			GenerateMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex );
		virtual RenderableMesh*			GenerateDynamicMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex );
		virtual RenderableModel*		GenerateModelFromMessage( CreateModelMessage* msg );
		virtual Texture*				GenerateTextureFromMessage( CreateTextureMessage* msg );
		virtual RenderableModel*		GenerateDuplicateModelFromMessage( DuplicateModelMessage* msg );
		virtual ParticleEmitter*		GenerateEmitterFromMessage( CreateParticleEmitterMessage* msg );
		virtual void					UpdateMeshData( UpdateMeshMessage* msg );
		virtual Animator*				CreateAnimator( unsigned int jointCount ) const;

		OpenGLBufferData*				GetBufferForNode( unsigned int nodeID, unsigned int meshIndex );

		void							SendGlobalUniforms( void ) const;	
		GUIManager*						GetGUIManager() const	{ return m_guiMgr; }
	private:
										OpenGLRenderModule( void );		

		bool							Initialize( void );
		void							InitGUI( void );

		void							LoadRendererSettings( void );
		void							LoadShaders( void );
		void							RenderThreadFunc( void );

		sf::Window*						m_window;
		sf::Thread*						m_renderThread;
		sf::Mutex*						m_renderSettingsMutex;

		glm::vec3						m_lightDirection; //REmove this eventually.
		
		GUIManager*						m_guiMgr;

		std::vector<OpenGLBufferData*>	m_buffers;

		OpenGLRendererSettings*			m_renderSettings;
		bool							m_renderThreadActive;
	};

} //namespace vgs

#endif //__OPENGLRENDERMODULE_H__