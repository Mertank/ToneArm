/*
========================

Copyright (c) 2014 Vinyl Games Studio

	SinglePlayerGameScene.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 7/31/2014 12:01:43 PM

========================
*/
#include "SinglePlayerGameScene.h"

#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/LevelResource.h"
#include "../ToneArmEngine/CachedResourceLoader.h"
#include "../ToneArmEngine/ModelNode.h"
#include "../ToneArmEngine/LevelNode.h"
#include "../ToneArmEngine/OpenGLRenderModule.h"

#include "TankCharacter.h"
#include "SniperCharacter.h"
#include "FogOfWarNode.h"
#include "PauseGUI.h"

using namespace vgs;
using namespace merrymen;

IMPLEMENT_RTTI(SinglePlayerGameScene, GameScene);

SinglePlayerGameScene::SinglePlayerGameScene()
{

}

bool SinglePlayerGameScene::Init() 
{
	if (Scene::Init())
	{
		/*FreeLookCamera* freelookCam = FreeLookCamera::Create();
		AddChild(freelookCam);
		SetCurrentCamera( freelookCam );*/

		// create a level from the level resource
		std::shared_ptr<LevelResource> levelRes = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>( EngineModuleType::RESOURCELOADER )->LoadResource<LevelResource>( "Casino/Casino.vgsLevel", ( char )0xA6 );
		LevelNode* level = LevelNode::CreateFromResource( levelRes );
		
#ifdef TONEARM_DEBUG
		level->ShowCollisions( false );
#endif
		SetLevel(level);
		
		//create local character(s)
		TankCharacter* player = TankCharacter::Create("Models/RobynTank.vgsmodel", "Data/Characters.ini", "Tank", false);
		player->SetPosition(glm::vec3(1000.0f, 8.5f, 1000.0f));
		AddPlayer(player);

		/*SniperCharacter* player = SniperCharacter::Create("Models/DonTank.vgsmodel", "Data/Characters.ini", "Sniper", false);
		player->SetPosition(glm::vec3(1000.0f, 8.5f, 1000.0f));
		AddPlayer(player);*/

		/*SniperCharacter* player2 = SniperCharacter::Create("Models/DonTank.vgsmodel", "Data/Characters.ini", "Sniper", false);
		player2->SetPosition(glm::vec3(1000.0f, 8.5f, 1200.0f));
		AddPlayer(player2);*/

		// create camera
		FixedCamera* fixedCam = FixedCamera::Create();
		fixedCam->InitWithTarget(player, 1000.0f, 1900.0f);
		fixedCam->Init();
		SetCurrentCamera(fixedCam);

		//create fog of war
		FogOfWarNode* fog = FogOfWarNode::Create();
		AddChild( fog );

		// will be called when ingame starts
		GameScene::InitGUI();

		GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();
		guiMgr->CreateLayer(new PauseGUI(PauseGUI::Type::SINGLE), "pause_menu_gui");

		CreateReticle();

		return true;
	}

	return false;
}

void SinglePlayerGameScene::EnterScene()
{
	GameScene::EnterScene();
}

void SinglePlayerGameScene::ExitScene()	
{
	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();
	if (guiMgr)
	{
		guiMgr->RemoveLayer("pause_menu_gui");
	}
	GameScene::ExitScene();
}

void SinglePlayerGameScene::Update(float dt)
{
	GameScene::Update(dt);
}

Character* const SinglePlayerGameScene::GetLocalCharacter() const
{
	return *m_players.begin();
}