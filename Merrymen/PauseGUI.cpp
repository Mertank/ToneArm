/*
========================

Copyright (c) 2014 Vinyl Games Studio

	PauseGUI.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 7/29/2014 11:48:29 AM

========================
*/
#include "PauseGUI.h"
#include "MainMenuScene.h"

#include "../ToneArmEngine/UICommand.h"
#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/RunningInfo.h"
#include "../ToneArmEngine/OpenGLRenderModule.h"
#include "../ToneArmEngine/GUIManager.h"
#include "../ToneArmEngine/LANMenu.h"
#include "../ToneArmEngine/NetworkModule.h"

using namespace vgs;
using namespace merrymen;

PauseGUI::PauseGUI(Type type)
	: m_type(type)
{

}

PauseGUI::~PauseGUI( void ) {
	m_widgetMap->erase( "pause_menu_btn" );
}

vgs::SFGWinPtr PauseGUI::CreateElements(vgs::WidgetMap& widgetMap)
{
	m_widgetMap = &widgetMap;

	m_window = sfg::Window::Create(sfg::Window::Style::BACKGROUND & sfg::Window::Style::TOPLEVEL);
	m_window->SetRequisition(sf::Vector2f(300.f, 0.f));
	m_window->SetId("pause_menu_window");

	m_exitBtn = sfg::Button::Create(m_type == Type::SINGLE ? "Go to Menu" : "Disconnect");
	m_exitBtn->SetRequisition(sf::Vector2f(150.f, 0.f));
	m_exitBtn->SetClass("pause_menu_btn");

	widgetMap["pause_menu_btn"] = std::shared_ptr<sfg::Button>( m_exitBtn );

	sfg::Box::Ptr btnContainer = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.0f);

	auto pauseLbl = sfg::Label::Create("Pause");
	pauseLbl->SetClass("menu_title");

	btnContainer->Pack(pauseLbl);
	btnContainer->Pack(m_exitBtn);

	m_window->Add(btnContainer);

	m_deskPtr->SetProperty("Window#pause_menu_window > Box > Label.menu_title", "FontSize", 32.f);
	m_deskPtr->SetProperty("Window#pause_menu_window", "BackgroundColor", sf::Color(0, 0, 0, 127));
	m_deskPtr->SetProperty("Window#pause_menu_window > Box > Button.pause_menu_btn", "FontSize", 28.f);
	m_deskPtr->SetProperty("Window#pause_menu_window > Box > Button.pause_menu_btn", "Color", sf::Color(150, 150, 150, 255));

	ConnectSignals();

	SetVisible(false);

	return m_window;
}

void PauseGUI::ConnectSignals()
{
	m_exitBtn->GetSignal(sfg::Window::OnLeftClick).Connect(
		std::bind(m_type == Type::SINGLE ? &PauseGUI::OnExit : &PauseGUI::OnDisconnect, this));
}

void PauseGUI::RefreshLayer(float dt)
{
	RelativeGUILayer::RefreshLayer(dt);
}

void PauseGUI::OnDisconnect()
{
	NetworkModule* netmod = Engine::GetInstance()->GetModuleByType<NetworkModule>(EngineModuleType::NETWORKER);
	if (netmod)
	{
		netmod->Disconnect();
	}
	
	OnExit();
}

void PauseGUI::OnExit()
{
	UICommand::AddEngCommand(UICommand(UICommand::UICommandID::CMD_ENG_SET_SCENE, 0, (unsigned char*)(new MainMenuScene())));
}