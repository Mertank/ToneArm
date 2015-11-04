/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MultiplayerGameScene.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 7/31/2014 5:03:15 PM

========================
*/
#include "MultiplayerGameScene.h"
#include "PauseGUI.h"
#include "TankCharacter.h"
#include "SniperCharacter.h"
#include "FogOfWarNode.h"
#include "TeamDeathMatchMH.h"

#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/LevelResource.h"
#include "../ToneArmEngine/CachedResourceLoader.h"
#include "../ToneArmEngine/AnimationNode.h"
#include "../ToneArmEngine/ModelNode.h"
#include "../ToneArmEngine/NetworkModule.h"
#include "../ToneArmEngine/LevelNode.h"
#include "../ToneArmEngine/NetworkNode.h"
#include "../ToneArmEngine/GUIManager.h"
#include "../ToneArmEngine/OpenGLRenderModule.h"
#include "../ToneArmEngine/ScoreTableGUI.h"
#include "../ToneArmEngine/RoundInfoGUI.h"
#include "../ToneArmEngine/ClassSelectionGUI.h"

using namespace vgs;
using namespace merrymen;

bool MultiplayerGameScene::Init() 
{
	if (Scene::Init())
	{
		// schedule level res loading
		Engine::GetInstance()->GetModuleByType<CachedResourceLoader>(EngineModuleType::RESOURCELOADER)
			->LoadResourceAsync("Casino/Casino.vgsLevel", PASS_CALLBACKFN(&MultiplayerGameScene::OnLevelLoaded, this), (char)0xA6);

		//Engine::GetInstance()->GetModuleByType<CachedResourceLoader>(EngineModuleType::RESOURCELOADER)
		//	->LoadResourceAsync("TestLevel/testLevel.vgsLevel", PASS_STATIC_CALLBACKFN(OnLevelLoaded_St), (char)0xA6);

		//create fog of war
		FogOfWarNode* fog = FogOfWarNode::Create();
		AddChild( fog );

		// will be called when ingame starts
		GameScene::InitGUI();

		GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();
		if (guiMgr)
		{
			guiMgr->CreateLayer(new ScoreTableGUI(), "score_win");
			guiMgr->CreateLayer(new PauseGUI(PauseGUI::Type::MULTI), "pause_menu_gui");
			//guiMgr->CreateLayer(new RoundInfoGUI(), "round_win");
		}

		return true;
	}

	return false;
}

void MultiplayerGameScene::Update(float dt)
{
	GameScene::Update(dt);
}

void MultiplayerGameScene::EnterScene()
{
	GameScene::EnterScene();
}

void MultiplayerGameScene::ExitScene()	
{
	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();
	if (guiMgr)
	{
		guiMgr->RemoveLayer("pause_menu_gui");
		guiMgr->RemoveLayer("score_win");
		//guiMgr->RemoveLayer("round_win");

		if (guiMgr->GetLayer<ClassSelectionGUI>("class_select_win"))
		{
			guiMgr->RemoveLayer("class_select_win");
		}
	}
	GameScene::ExitScene();
}

Character* const MultiplayerGameScene::GetLocalCharacter() const
{
	Character* result = nullptr;
	if (NetworkNode::GetLocalPlayer()) 
	{
		result = static_cast<Character*>(NetworkNode::GetLocalPlayer()->GetParent());
	}
	return result;
}

void MultiplayerGameScene::OnLevelLoaded(vgs::CachedResource* res)
{
	LevelNode* level = LevelNode::CreateFromResource(std::static_pointer_cast<LevelResource>(res->ptr));
	SetLevel(level);
#ifdef TONEARM_DEBUG
	level->ShowCollisions(false);
#endif
}

void MultiplayerGameScene::OnLevelLoaded_St(vgs::CachedResource* res)
{
	LevelNode* level = LevelNode::CreateFromResource(std::static_pointer_cast<LevelResource>(res->ptr));

	GameScene* gs = static_cast<GameScene*>(Engine::GetInstance()->GetRunningScene());

	gs->SetLevel(level);
#ifdef TONEARM_DEBUG
	level->ShowCollisions(true);
#endif
}