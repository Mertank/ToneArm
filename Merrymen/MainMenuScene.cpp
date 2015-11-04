/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MainMenuScene.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 7/30/2014 4:58:02 PM

========================
*/
#include "MainMenuScene.h"
#include "MainMenuGUI.h"
#include "TeamDeathMatchMH.h"

#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/OpenGLRenderModule.h"
#include "../ToneArmEngine/NetworkModule.h"
#include "../ToneArmEngine/LANMenu.h"

using namespace vgs;
using namespace merrymen;

MainMenuScene::MainMenuScene()
{
}

bool MainMenuScene::Init()
{
	return Scene::Init();
}

void MainMenuScene::Update(float dt)
{
	Scene::Update(dt);
}

void MainMenuScene::EnterScene()
{
	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();
	if (guiMgr)
	{
		guiMgr->CreateLayer(new MainMenuGUI(), "main_menu");
		guiMgr->CreateLayer(new LANMenu(), "lan_menu");
	}

	// setup network 
	NetworkModule* netmod = Engine::GetInstance()->GetModuleByType<NetworkModule>(EngineModuleType::NETWORKER);
	if (netmod)
	{
		// FIXME: message handler should be picked after server tells which one
		netmod->SetMsgHandler(new TeamDeathMatchMH());
	}
}

void MainMenuScene::ExitScene()
{
	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();
	if (guiMgr)
	{
		guiMgr->RemoveLayer("main_menu");
		guiMgr->RemoveLayer("lan_menu");
	}
}