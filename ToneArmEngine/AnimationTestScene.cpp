/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	AnimationTestScene

	Created by: Karl Merten
	Created on: 06/08/2014

========================
*/
#include "AnimationTestScene.h"

#include "Engine.h"
#include "LevelResource.h"
#include "CachedResourceLoader.h"
#include "AnimationNode.h"
#include "ModelNode.h"
#include "NetworkModule.h"
#include "LevelNode.h"
#include "OpenGLRenderModule.h"
#include "InputModule.h"
#include "MeshHelper.h"

#include "../Merrymen/TankCharacter.h"
#include "../Merrymen/SniperCharacter.h"
#include "../Merrymen/FogOfWarNode.h"
#include "../Merrymen/TeamDeathMatchMH.h"
#include "../Merrymen/SinglePlayerGameScene.h"
#include "../Merrymen/PauseGUI.h"

using namespace vgs;
using namespace merrymen;

ModelNode* AnimationTestScene::m_characterToAnimate = NULL;
unsigned int AnimationTestScene::m_ticks = 0;
bool AnimationTestScene::m_reloadModel = false;

INITIALIZE_DEBUG_COMMAND_CALLBACK( vgs::AnimationTestScene, playAnim )
INITIALIZE_DEBUG_COMMAND_CALLBACK( vgs::AnimationTestScene, reloadModel )

AnimationTestScene::AnimationTestScene()
{}

bool AnimationTestScene::Init() 
{
	if (Scene::Init()) {
	
		//create local character(s)
		// create camera
		FreeLookCamera* cam = FreeLookCamera::Create();
		cam->SetPosition( 0.0f, 75.0f, 400.0f );
		cam->Yaw( glm::radians( 180.0f ) );
		SetCurrentCamera(cam);

		return true;
	}

	return false;
}

void AnimationTestScene::EnterScene()
{
	Scene::EnterScene();

	m_characterToAnimate = ModelNode::Create( "Models/ActualRobynTank.vgsModel" );
	AddChild( m_characterToAnimate );

	ModelNode* pistol = ModelNode::Create( "Models/Sniper.vgsModel" );
	m_characterToAnimate->AddChild( pistol );
	pistol->SetParentBone( StringUtils::Hash( "TankRebel_RightHand" ) );
}

void AnimationTestScene::ExitScene()	
{
	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();
	if (guiMgr)
	{
		guiMgr->RemoveLayer("pause_menu_gui");
	}
	Scene::ExitScene();
}

void AnimationTestScene::Update(float dt)
{
	Scene::Update(dt);

	if ( m_reloadModel ) {
		++m_ticks;
		if ( m_ticks > 5 ) {
			m_characterToAnimate = ModelNode::Create( "Models/DonTank.vgsModel" );
			AddChild( m_characterToAnimate );

			ModelNode* pistol = ModelNode::Create( "Models/Sniper.vgsModel" );
			m_characterToAnimate->AddChild( pistol );
			pistol->SetParentBone( StringUtils::Hash( "TankPMC_RightHand" ) );

			m_ticks = 0;
			m_reloadModel = false;			
		}
	}
}

BEGIN_CALLBACK_FUNCTION( AnimationTestScene, playAnim )
	AnimationNode* animNode = m_characterToAnimate->GetChildOfType<AnimationNode>();
	if ( animNode ) {
		if ( args.size() == 1 ) {
			animNode->PlayAnimation( args[0], -1 );
		} else if ( args.size() == 2 )  {
			animNode->PlayAnimation( args[0], -1, 0.2f, ( float )atof( args[1] ) );
		} else {
			animNode->PlayAnimation( args[0], atoi( args[2] ), 0.2f, ( float )atof( args[1] ) );
		}
	}
END_CALLBACK_FUNCTION

BEGIN_CALLBACK_FUNCTION( AnimationTestScene, reloadModel )
	m_reloadModel = true;
	
	m_characterToAnimate->RemoveFromParent();
	delete m_characterToAnimate;

	FreeLookCamera* cam = ( FreeLookCamera* )Engine::GetInstance()->GetRunningScene()->GetCurrentCamera();
	cam->SetPosition( 0.0f, 75.0f, 400.0f );
	cam->SetYaw( glm::radians( 180.0f ) );
	cam->SetPitch( 0.0f );
END_CALLBACK_FUNCTION