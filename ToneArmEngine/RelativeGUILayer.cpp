/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RelativeGUILayer.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 6/25/2014 2:51:00 PM

========================
*/
#include "RelativeGUILayer.h"
#include "Engine.h"
#include "RunningInfo.h"

using namespace vgs;

RelativeGUILayer::RelativeGUILayer()
	: m_relativePosition(.5f, .4f)
	, m_contextWidth(0)
	, m_contextHeight(0)
{
}

void RelativeGUILayer::RefreshLayer(float dt)
{
	RunningInfo* args = Engine::GetInstance()->GetRunningInfo();

	if (args->contextWidth != m_contextWidth || args->contextHeight != m_contextHeight)
	{
		m_contextWidth = args->contextWidth;
		m_contextHeight = args->contextHeight;

		m_window->SetPosition(sf::Vector2f((float)m_contextWidth * m_relativePosition.x - m_window->GetRequisition().x / 2,
										   (float)m_contextHeight * m_relativePosition.y - m_window->GetRequisition().y / 2));

		//GUILayer::RefreshLayer(dt);
	}
	GUILayer::RefreshLayer(dt);
}

void RelativeGUILayer::SetRelativePosition(const sf::Vector2f& vec)
{
	m_relativePosition.x = vec.x < 0 ? 0 : vec.x > 1 ? 1 : vec.x;
	m_relativePosition.y = vec.y < 0 ? 0 : vec.y > 1 ? 1 : vec.y;
}