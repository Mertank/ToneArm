/*
========================

Copyright (c) 2014 Vinyl Games Studio

	GUILayer.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 5/5/2014 3:39:00 PM

========================
*/
#include "GUILayer.h"

using namespace vgs;

GUILayer::GUILayer()
	: m_visible(true)
	, m_widgetMap(nullptr)
	, m_deskPtr(nullptr)
	, m_enabled(true)
{
}

GUILayer::~GUILayer()
{
	m_deskPtr->Remove(m_window);
}

//SFGWinPtr GUILayer::CreateElements(WidgetMap& widgetMap)
//{
//	//InitPools();
//	//return nullptr;
//}

void GUILayer::SetVisible(bool visible)
{
	if (visible != m_visible)
	{
		m_visible = visible;
		m_window->Show(m_visible);
	}
}