/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RoundInfoGUI.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 8/5/2014 4:21:38 PM

========================
*/
#include "RoundInfoGUI.h"

#include <sstream>

using namespace vgs;
using namespace merrymen;

RoundInfoGUI::RoundInfoGUI()
	: m_roundNumber(0)
{
}

vgs::SFGWinPtr RoundInfoGUI::CreateElements(vgs::WidgetMap& widgetMap)
{
	m_window = sfg::Window::Create(sfg::Window::Style::NO_STYLE);
	m_window->SetId("round_info_window");

	m_window->SetRequisition(sf::Vector2f(100.f, 0.0f));
	SetRelativePosition(sf::Vector2f(0.5f, 0.1f));

	m_timerLbl = sfg::Label::Create("Numbers here");

	sfg::Box::Ptr winContainer = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);

	winContainer->Pack(m_timerLbl);

	m_window->Add(winContainer);

	//m_deskPtr->SetProperty("Window#pause_menu_window", "BackgroundColor", sf::Color(255, 255, 255, 255));
	m_deskPtr->SetProperty("Window#round_info_window > Box > Label", "FontSize", 20.f);
	//m_deskPtr->SetProperty("Window#pause_menu_window > Box > Button.pause_menu_btn", "Color", sf::Color(150, 150, 150, 255));

	SetVisible(false);

	return m_window;
}

void RoundInfoGUI::RefreshLayer(float dt)
{
	m_updateMutex.lock();
	double timer = m_valuesToUpdate.m_started + m_valuesToUpdate.m_duration - (RakNet::GetTime() * 0.001);
	m_updateMutex.unlock();

	if (timer < 0)
	{
		timer = 0;
	}

	std::stringstream ss;
	ss << (unsigned)timer;

	m_timerLbl->SetText(ss.str().c_str());

	RelativeGUILayer::RefreshLayer(dt);
}

void RoundInfoGUI::SetUpdateTimer(const UpdateTimer& msg)
{
	m_updateMutex.lock();
	m_valuesToUpdate = msg;
	m_updateMutex.unlock();
}

void RoundInfoGUI::SetRoundNumber(unsigned int n)
{
	m_updateMutex.lock();
	m_roundNumber = n;
	m_updateMutex.unlock();
}