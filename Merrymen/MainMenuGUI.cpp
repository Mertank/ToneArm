/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MainMenuGUI.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 7/29/2014 11:48:29 AM

========================
*/
#include "MainMenuGUI.h"

#include "../ToneArmEngine/UICommand.h"
#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/RunningInfo.h"
#include "../ToneArmEngine/OpenGLRenderModule.h"
#include "../ToneArmEngine/AnimationTestScene.h"
#include "../ToneArmEngine/GUIManager.h"
#include "../ToneArmEngine/LANMenu.h"

using namespace vgs;
using namespace merrymen;

vgs::SFGWinPtr MainMenuGUI::CreateElements(vgs::WidgetMap& widgetMap)
{
	m_window = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
	m_window->SetRequisition(sf::Vector2f(300.f, 0.f));
	m_window->SetId("main_menu_window");

	//RunningInfo* args = Engine::GetInstance()->GetRunningInfo();

	//m_window->SetPosition(sf::Vector2f(0.f, 0.f));
	//m_window->SetRequisition(sf::Vector2f(args->contextWidth, args->contextHeight));

	auto titleLbl = sfg::Label::Create("Merrymen");
	titleLbl->SetClass("menu_title");

	m_mpStart = sfg::Button::Create("Multiplayer");
	m_mpStart->SetRequisition(sf::Vector2f(300.f, 0.f));
	m_mpStart->SetClass("menu_btn");

	m_exit = sfg::Button::Create("Exit");
	m_exit->SetRequisition(sf::Vector2f(300.f, 0.f));
	m_exit->SetClass("menu_btn");

#ifdef TONEARM_DEBUG
	m_animTestStart = sfg::Button::Create("Animation Testbed");
	m_animTestStart->SetClass("menu_btn");

	m_spStart = sfg::Button::Create("Single player");
	m_spStart->SetRequisition(sf::Vector2f(300.f, 0.f));
	m_spStart->SetClass("menu_btn");
#endif
	//m_aligner = sfg::Fixed::Create();

	sfg::Box::Ptr btnContainer = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.0f);

	btnContainer->Pack(titleLbl);
#ifdef TONEARM_DEBUG
	btnContainer->Pack(m_spStart);
#endif
	btnContainer->Pack(m_mpStart);
#ifdef TONEARM_DEBUG
	btnContainer->Pack(m_animTestStart);
#endif
	btnContainer->Pack(m_exit);

	//m_aligner->Put(btnContainer, sf::Vector2f((float)args->contextWidth * 0.5f, (float)args->contextHeight * 0.7f));

	//sf::Image img;
	//m_image = sfg::Image::Create();

	//if(img.loadFromFile("menu_background.png"))
	//{
	//	m_image->SetImage(img);
	//	m_aligner->Put(m_image, sf::Vector2f(0, 0));
	//}

	m_window->Add(btnContainer);

	m_deskPtr->SetProperty("Window#main_menu_window > Box > Label.menu_title", "FontSize", 32.f);
	m_deskPtr->SetProperty("Window#main_menu_window", "BackgroundColor", sf::Color(0, 0, 0, 127));
	m_deskPtr->SetProperty("Window#main_menu_window > Box > Button.menu_btn", "FontSize", 28.f);
	m_deskPtr->SetProperty("Window#main_menu_window > Box > Button.menu_btn", "Color", sf::Color(150, 150, 150, 255));

	ConnectSignals();

	return m_window;
}

void MainMenuGUI::ConnectSignals()
{
	m_mpStart->GetSignal(sfg::Window::OnLeftClick).Connect(std::bind(&MainMenuGUI::OnMpStart, this));
#ifdef TONEARM_DEBUG
	m_spStart->GetSignal(sfg::Window::OnLeftClick).Connect(std::bind(&MainMenuGUI::OnSpStart, this));
	m_animTestStart->GetSignal(sfg::Window::OnLeftClick).Connect(std::bind(&MainMenuGUI::OnAnimStart, this));
#endif
	m_exit->GetSignal(sfg::Window::OnLeftClick).Connect(std::bind(&MainMenuGUI::OnExit, this));
}

void MainMenuGUI::RefreshLayer(float dt)
{
	RelativeGUILayer::RefreshLayer(dt);
}

void MainMenuGUI::OnMpStart()
{
	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>( EngineModuleType::RENDERER )->GetGUIManager();
	if (guiMgr)
	{
		guiMgr->SendVisMsg(SetVisibilityMsg("main_menu", false));
		guiMgr->SendEnableMsg(SetEnableMsg("main_menu", false));

		guiMgr->SendVisMsg(SetVisibilityMsg("lan_menu", true));
		guiMgr->SendEnableMsg(SetEnableMsg("lan_menu", true));
	}
}
#ifdef TONEARM_DEBUG
void MainMenuGUI::OnSpStart()
{
	m_spStart->SetState( sfg::Widget::State::INSENSITIVE );
	UICommand::AddEngCommand(UICommand(UICommand::UICommandID::CMD_ENG_SET_SCENE, 0, (unsigned char*)(new SinglePlayerGameScene())));	
}

void MainMenuGUI::OnAnimStart()
{
	m_animTestStart->SetState( sfg::Widget::State::INSENSITIVE );
	UICommand::AddEngCommand(UICommand(UICommand::UICommandID::CMD_ENG_SET_SCENE, 0, (unsigned char*)(new AnimationTestScene())));
}
#endif

void MainMenuGUI::OnExit()
{
	UICommand::AddEngCommand(UICommand(UICommand::UICommandID::CMD_ENG_SHUTDOWN, 0, 0));
}