/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	OpenGLRenderModule

	Created by: Karl Merten
	Created on: 25/04/2014

========================
*/
#include "OpenGLRenderModule.h"

#include <sstream>
#include <SFML\Window.hpp>
#include <GL\glew.h>
#include <vector>
#include <ctime>

#include "Log.h"
#include "Engine.h"
#include "Scene.h"
#include "RunningInfo.h"
#include "OpenGLRendererSettings.h"
#include "InitializationFileResource.h"
#include "OpenGLProgramManager.h"
#include "OpenGLProgram.h"
#include "CachedResourceLoader.h"
#include "GUIManager.h"
#include "MessagePoolManager.h"
#include "GUIMessages.h"
#include "LANMenu.h"
#include "RSync.h"
#include "NetConsts.h"

#include "RenderableModel.h"
#include "CameraProjectionData.h"
#include "OpenGLMesh.h"
#include "OpenGLModel.h"
#include "OpenGLTexture.h"
#include "ProgramsFileResource.h"
#include "OpenGLBufferData.h"
#include "OpenGLDynamicMesh.h"

#include "OpenGLBasicPass.h"
#include "OpenGLTransparencyPass.h"
#include "OpenGLGUIPass.h"
#include "OpenGLAnimator.h"
#include "OpenGLParticleEmitter.h"

#include "../Merrymen/MainMenuGUI.h"

namespace vgs {
/*
============
OpenGLRenderModule::OpenGLRenderModule

	OpenGLRenderModule default constructor
============
*/
OpenGLRenderModule::OpenGLRenderModule( void ) :
	m_renderSettings( NULL ),
	m_renderSettingsMutex( NULL ),
	m_window( NULL ),
	m_renderThreadActive( false ),
	m_renderThread( NULL )
{}
/*
============
OpenGLRenderModule::~OpenGLRenderModule

	OpenGLRenderModule destructor
============
*/
OpenGLRenderModule::~OpenGLRenderModule( void ) {

	delete m_window;
	delete m_renderThread;
	delete m_renderSettingsMutex;
	delete m_renderSettings;

	DELETE_STD_VECTOR_POINTER( OpenGLBufferData, m_buffers )
}
/*
============
OpenGLRenderModule::CreateOpenGLRenderer

	Creates and OpenGL Renderer.
	Makes an SFML window with the set dimensions
============
*/
OpenGLRenderModule*	OpenGLRenderModule::CreateOpenGLRenderer( void ) {
	OpenGLRenderModule* renderer = new OpenGLRenderModule();
	if ( renderer && renderer->Initialize() ) {
		return renderer;
	} else {
		delete renderer;
		Log::GetInstance()->WriteToLog( "OpenGLRenderer", "Error initializing the OpenGL Renderer" );
		return NULL;
	}
}
/*
============
OpenGLRenderModule::Update

	Polls window events. Called on Logic thread.
============
*/
void OpenGLRenderModule::Update( float dt ) {
	//TODO: So bad!!!!!!! So pure.
	//m_camera->Update( dt );
}
/*
=============
RenderModule::InitGUI

	Initializes the GUIManager.
=============
*/
void OpenGLRenderModule::InitGUI()
{
	for ( std::set<RenderPass*>::iterator iter = m_passes.begin();
		  iter != m_passes.end(); ++iter ) {
		if ( ( *iter )->GetRTTI() == OpenGLGUIPass::rtti ) {
			m_guiMgr = ( ( OpenGLGUIPass* )( *iter ) )->GetGUIManager();
			break;
		}
	}
}
/*
============
OpenGLRenderModule::Initialize

	Initializes an OpenGL renderer
============
*/
bool OpenGLRenderModule::Initialize( void ) {
	RenderModule::Init();

	LoadRendererSettings();

	Log::GetInstance()->WriteToLog( NULL, "\nOpenGL Renderer Init\nSuccessfully created context", true );
	std::stringstream ss;
	ss << "Using OpenGL Version: ";
	ss << m_renderSettings->majorVersion;
	ss << ".";
	ss << m_renderSettings->minorVersion;
	ss << "\n";

	Log::GetInstance()->WriteToLog( NULL, ss.str().c_str() );

	m_renderSettingsMutex = new sf::Mutex();	

	m_passes.insert( OpenGLBasicPass::CreateWithPriority( 0 ) );
	m_passes.insert( OpenGLTransparencyPass::CreateWithPriority( 5 ) );
	m_passes.insert( OpenGLGUIPass::CreateGUIPassWithPriority( -1 ) ); //GUI is always last, so let's overflow it to get the max. pure magic

	InitGUI();

	return true;
}
/*
============
OpenGLRenderModule::LoadRendererSettings

	Loads the renderer settings from a config file.
============
*/
void OpenGLRenderModule::LoadRendererSettings( void ) {
	m_renderSettings = new OpenGLRendererSettings();	
	std::shared_ptr<InitializationFileResource> iniFile = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>( EngineModuleType::RESOURCELOADER )->LoadResource<InitializationFileResource>( "ToneArmEngine.ini", ( char )0xAB );
	const InitializationCategory* renderSettings = iniFile->GetCategory( "OpenGL" );

	m_renderSettings->majorVersion = renderSettings->GetIntValueForKey( "MajorVersion" );
	m_renderSettings->minorVersion = renderSettings->GetIntValueForKey( "MinorVersion" );
	m_renderSettings->frameRateCap = renderSettings->GetIntValueForKey( "FrameRateCap" );

	m_renderSettings->clearColor = glm::vec4( renderSettings->GetVec3ValueForKey( "ClearColor" ), 1.0f );
}
/*
============
OpenGLRenderModule::StartUp

	Creates the render thread.
============
*/
void OpenGLRenderModule::Startup( void ) {
	m_renderThread = new sf::Thread( &vgs::OpenGLRenderModule::RenderThreadFunc, this );
	
	m_renderThread->launch();
}
/*
============
OpenGLRenderModule::LoadShaders

	Loads the shaders for the program.
============
*/
void OpenGLRenderModule::LoadShaders( void ) {
	std::shared_ptr<ProgramsFileResource> programs = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>( EngineModuleType::RESOURCELOADER )->LoadResource<ProgramsFileResource>( "Data/Shaders.sh", ( char )0xA6 );

	std::vector<Shader*> shaders;
	for ( unsigned int i = 0; i < programs->GetProgramCount(); ++i ) {
		const ProgramInfo* progInfo		= programs->GetProgram( i );
		unsigned int initialPosition	= progInfo->shaders[0]->shaderStart;
		unsigned int dataOffset			= 0;
		char* programSource				= programs->GetProgramData( i );

		for ( unsigned char j = 0; j < progInfo->shaderCount; ++j ) {
			Shader* s = new Shader();
			s->shaderType = progInfo->shaders[j]->shaderType;
			s->shaderLength = progInfo->shaders[j]->shaderLength;
			s->shaderLocation = &programSource[dataOffset];
			
			dataOffset += progInfo->shaders[j]->shaderLength;
			shaders.push_back( s );
		}

		OpenGLProgramManager::GetInstance()->AddProgram( shaders, "outColor", progInfo->programName, progInfo->features );
		
		DELETE_STD_VECTOR_POINTER( Shader, shaders )
		shaders.clear();

		delete[] programSource;
	}
}
/*
============
OpenGLRenderModule::Shutdown

	Notifies the render thread to exit, and 
	s for it.
============
*/
void OpenGLRenderModule::Shutdown( void ) {
	m_renderThreadActive = false;
	
	// let rederer's function end
	RSync_SendNotification();
	m_renderThread->wait();

	RenderModule::Shutdown();

	m_window->close();

	OpenGLProgramManager::DestroyInstance();
}
/*
============
OpenGLRenderModule::SetClearColor

	Sets the clear color.
============
*/
void OpenGLRenderModule::SetClearColor( const glm::vec4& newColor ) {
	m_renderSettingsMutex->lock();
	m_renderSettings->clearColor = newColor;
	m_renderSettingsMutex->unlock();
}
/*
============
OpenGLRenderModule::SetFrameRateLimit

	Sets the clear color.
============
*/
void OpenGLRenderModule::SetFrameRateLimit( unsigned int newLimit ) {
	m_renderSettingsMutex->lock();
	m_renderSettings->frameRateCap = newLimit;
	m_renderSettingsMutex->unlock();
}
/*
============
OpenGLRenderModule::GetClearColor

	Gets the clear color.
============
*/
glm::vec4 OpenGLRenderModule::GetClearColor( void ) {
	m_renderSettingsMutex->lock();
	glm::vec4 clearColor = m_renderSettings->clearColor;
	m_renderSettingsMutex->unlock();

	return clearColor;
}
/*
============
OpenGLRenderModule::GetBufferForNode

	Gets the buffer for the node.
	If it does not exist, it creates a new one.
============
*/
OpenGLBufferData* OpenGLRenderModule::GetBufferForNode( unsigned int nodeID, unsigned int meshIndex ) {
	OpenGLBufferData* retData = NULL;

	BEGIN_STD_VECTOR_ITERATOR( OpenGLBufferData*, m_buffers )
		if ( currentItem->GetOriginalNodeID() == nodeID && currentItem->GetMeshIndex() == meshIndex ) {
			retData = currentItem;
			break;
		}
	END_STD_VECTOR_ITERATOR

	if ( !retData ) {
		retData = OpenGLBufferData::CreateForNode( nodeID, meshIndex );
		m_buffers.push_back( retData );
	} else {
		retData = retData->Duplicate();
	}

	return retData;
}
/*
============
OpenGLRenderModule::GetFrameRateLimit

	Gets the frameRate limit.
============
*/
unsigned int OpenGLRenderModule::GetFrameRateLimit( void ) {
	m_renderSettingsMutex->lock();
	unsigned int limit = m_renderSettings->frameRateCap;
	m_renderSettingsMutex->unlock();

	return limit;
}
/*
============
OpenGLRenderModule::RenderThreadFunc

	The function the render thread will run.
	This is where the magic happens.
============
*/
void OpenGLRenderModule::RenderThreadFunc( void ) {
	m_renderThreadActive = true;

	RunningInfo* engineArgs = Engine::GetInstance()->GetRunningInfo();
	sf::VideoMode videoMode = sf::VideoMode( engineArgs->contextWidth, engineArgs->contextHeight, engineArgs->bitsPerPixel );
	sf::ContextSettings contextSettings = sf::ContextSettings( 32, 8, 4, m_renderSettings->majorVersion, m_renderSettings->minorVersion );

	if ( engineArgs->contextStyle == ContextStyle::FULLSCREEN ) {
		m_window = new sf::Window( videoMode, engineArgs->contextName, sf::Style::Fullscreen, contextSettings );
		m_window->setVerticalSyncEnabled( true );
	} else if ( engineArgs->contextStyle == ContextStyle::WINDOWED ) {
		m_window = new sf::Window( videoMode, engineArgs->contextName, sf::Style::Default, contextSettings );
	} else if ( engineArgs->contextStyle == ContextStyle::BORDERLESS_WINDOW ) {
		m_window = new sf::Window( videoMode, engineArgs->contextName, sf::Style::None, contextSettings );
	}

	if ( m_window->isOpen() != true ) {
		Log::GetInstance()->WriteToLog( "OpenGL Renderer", "SFML Window was not created", true );
		return;
	}

	glewExperimental = GL_TRUE;
	glewInit();

	LoadShaders();

	sf::Vector2u windowSize = m_window->getSize();

	unsigned int frameRateLimit		= GetFrameRateLimit();
	glm::vec4 clearColor			= GetClearColor();

	double thisTime = 0.001 * RakNet::GetTime();
	double prevTime = thisTime;

	glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
	glEnable( GL_CULL_FACE );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// save sf window in the gui mgr
	m_guiMgr->SetApplicationWin(m_window);

	while ( m_renderThreadActive ) {
		// wait for notification from the logic thread to continue
		RSync_WaitForNotification();

		// handle render messages
		// right after receiving notification to make sure that
		// we read messages from the previous frame, not current
		ProcessMessages();

		// calc frame timings
		//std::cout << "---R" << m_tick.frameno << "---" << std::endl;
		m_tick.t = m_tick.frameno * m_tick.dt;

		thisTime = 0.001 * RakNet::GetTime();
		m_tick.dt = float(thisTime - prevTime);
		prevTime = thisTime;
		
		sf::Event ev;
		while( m_window->pollEvent( ev ) ) {
			if (m_guiMgr/* && m_captureInput*/)
			{
				if (ev.type != sf::Event::LostFocus)
				{
					m_guiMgr->HandleEvent(ev);
				}
			}

			if ( ev.type == sf::Event::Closed ) {
				Engine::GetInstance()->ApplicationWasClosed();

			} else if ( ev.type == sf::Event::Resized ) {
				RunningInfo* info = Engine::GetInstance()->GetRunningInfo();						
				Scene* scene = Engine::GetInstance()->GetRunningScene();

				if ( scene ) {
					Camera* cam = scene->GetCurrentCamera();
					if ( cam ) {
						const Projection& proj = cam->GetProjection();
						cam->SetProjection( Projection( ( float )ev.size.width, ( float )ev.size.height, proj.nearClipping, proj.farClipping, proj.FOV, proj.projectionType ) );
					}

					info->contextWidth = ev.size.width;
					info->contextHeight = ev.size.height;
				} else {
					if ( m_window->getSize().x != info->contextWidth ||
							m_window->getSize().y != info->contextHeight ) {
						m_window->setSize( sf::Vector2u( info->contextWidth, info->contextHeight ) );
					}
				}
			} else if ( ev.type == sf::Event::GainedFocus ) {
				m_captureInput = true;
			} else if ( ev.type == sf::Event::LostFocus ) {
				m_captureInput = false;
			}
		}
		
		// gui update
		m_guiMgr->Update(m_tick.dt);

		m_lightDirection = glm::vec3( m_camera->GetViewMatrix() * glm::vec4( 0.0f, -1.0f, 0.0f , 0.0f ) );

		for ( RenderableMap::iterator object = m_renderObjects.begin();
				object != m_renderObjects.end(); ++object ) {
			object->second->Update( m_tick.dt );
		}

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		for ( std::multiset<RenderPass*>::iterator pass = m_passes.begin();
				pass != m_passes.end(); ++pass ) {
			( *pass )->DoPass( &m_renderObjects, this );
		}

		m_window->display();		
#ifdef TONEARM_DEBUG
		m_instantaniousFrameRate = ( unsigned int )( 1.0f / m_tick.dt + 0.5f );
		m_averageTimeElapsed += m_tick.dt;
		++m_frameCount;
		m_averageFrameRateSum += m_instantaniousFrameRate;
			
		if ( m_averageTimeElapsed > 1.0f ) {
			m_averageTimeElapsed = 0.0f;
			m_averageFrameRate = ( float )m_averageFrameRateSum / m_frameCount;

			m_averageFrameRateSum = 0;
			m_frameCount = 0;
		}
#endif

		//Update settings, if necessary
		//unsigned int currentLimit = GetFrameRateLimit();
		//if ( frameRateLimit != currentLimit ) {
		//	frameRateLimit = currentLimit;
		//	//targetTime = 1.0 / frameRateLimit;
		//}

		glm::vec4 currentColor = GetClearColor();
		if ( currentColor.r != clearColor.r ||
				currentColor.g != clearColor.g ||
				currentColor.b != clearColor.b ) {
			clearColor = currentColor;
			glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
		}

		m_tick.frameno++;
	}
}
/*
============
OpenGLRenderModule::SendUniforms

	Sends the global unforms to the program
============
*/
void OpenGLRenderModule::SendGlobalUniforms( void ) const {
	OpenGLProgram* const prog = OpenGLProgramManager::GetInstance()->GetActiveProgram();
	
	if ( !prog ) { //Invalid program
		return;
	}

	prog->SetUniform( "u_ViewMatrix", m_camera->GetViewMatrix() );
	prog->SetUniform( "u_ProjectionMatrix", m_camera->GetProjectionMatrix() );

	//TODO: Make light class for this stuff
	if ( prog->GetFeatures() & ProgramFeatures::FRAGMENT_LIGHTING ||
		 prog->GetFeatures() & ProgramFeatures::VERTEX_LIGHTING ) {
		prog->SetUniform( "u_LightDir", m_lightDirection );
		prog->SetUniform( "u_LightColor", glm::vec3( 1.0f, 1.0f, 1.0f ) );
	}
}
/*
============
OpenGLRenderModule::GenerateDynamicMeshFromMessage

	Generates an OpenGL specific dynamic mesh
============
*/
RenderableMesh*	OpenGLRenderModule::GenerateDynamicMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex ) {
	return OpenGLDynamicMesh::CreateDynamicMeshFromMessage( msg, meshIndex );
}
/*
============
OpenGLRenderModule::GenerateDynamicMeshFromMessage

	Generates an OpenGL specific dynamic mesh
============
*/
void OpenGLRenderModule::UpdateMeshData( UpdateMeshMessage* msg ) {
	OpenGLModel* model = ( OpenGLModel* )FindRenderable( msg->nodeID );
	OpenGLDynamicMesh* mesh = ( OpenGLDynamicMesh* )model->GetMesh( 0 );
	if ( mesh ) {
		mesh->UpdateData( msg );
	} 

	delete[] msg->verticies;
	delete[] msg->indicies;
}
/*
============
OpenGLRenderModule::GenerateMeshFromMessage

	Generates an OpenGL specific mesh
============
*/
RenderableMesh*	OpenGLRenderModule::GenerateMeshFromMessage( CreateMeshMessage* msg, unsigned int meshIndex ) {
	return OpenGLMesh::CreateMeshFromMessage( msg, meshIndex );
}
/*
============
OpenGLRenderModule::GenerateModelFromMessage

	Generates an OpenGL specific model
============
*/
RenderableModel* OpenGLRenderModule::GenerateModelFromMessage( CreateModelMessage* msg ) {
	return OpenGLModel::CreateModelFromMessage( msg );
}
/*
============
OpenGLRenderModule::GenerateTextureFromMessage

	Generates an OpenGL specific texture
============
*/
Texture* OpenGLRenderModule::GenerateTextureFromMessage( CreateTextureMessage* msg ) {
	return OpenGLTexture::CreateTextureFromMessage( msg );
}
/*
============
OpenGLRenderModule::GenerateDuplicateModelFromMessage

	Duplicates a mesh OpenGL style
============
*/
RenderableModel* OpenGLRenderModule::GenerateDuplicateModelFromMessage( DuplicateModelMessage* msg ) {
	OpenGLModel* orig = ( OpenGLModel* )FindRenderable( msg->originalModelID );
	if ( orig ) {
		return OpenGLModel::DuplicateModel( orig, msg );
	} else {
		return NULL;
	}
}
/*
============
OpenGLRenderModule::CreateAnimator

	Creates an animator specific to OpenGL
============
*/
Animator* OpenGLRenderModule::CreateAnimator( unsigned int jointCount ) const {
	return OpenGLAnimator::CreateAnimatorWithJointCount( jointCount );
}
/*
============
OpenGLRenderModule::GenerateEmitterFromMessage

	Creates an emitter specific to OpenGL
============
*/
ParticleEmitter* OpenGLRenderModule::GenerateEmitterFromMessage( CreateParticleEmitterMessage* msg ) {
	return OpenGLParticleEmitter::CreateOpenGLParticleEmitter( msg->settings );
}
}// namespace vgs