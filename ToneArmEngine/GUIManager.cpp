/*
========================

Copyright (c) 2014 Vinyl Games Studio

	GUIManager.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 5/5/2014 1:37:56 PM

========================
*/
#include "GUIManager.h"
#include "GUILayer.h"
#include "LANMenu.h"

#include <SFML\System.hpp>
#include <SFML\Window.hpp>

#include <SFGUI/Engines/BREW.hpp>
#include <SFGUI/Renderers/VertexArrayRenderer.hpp>
#include <SFGUI/Renderers/VertexBufferRenderer.hpp>

#include <iostream>

using namespace vgs;

GUIManager::GUIManager()
	: m_applicationWindow(nullptr)
	, m_msgPoolMgr(new MessagePoolManager())
{
}

GUIManager::~GUIManager()
{
	for (auto layer : m_layers)
	{
		delete layer.second;
	}

	delete m_msgPoolMgr;
}

void GUIManager::Init()
{
	m_msgPoolMgr->AddResizeablePool<AddLayerMsg, GUIManager>		("add_layer",		10, 10, this, &GUIManager::ProcessAddLayerMsg);
	m_msgPoolMgr->AddResizeablePool<RemoveLayerMsg, GUIManager>		("remove_layer",	10, 10, this, &GUIManager::ProcessRemoveLayerMsg);
	m_msgPoolMgr->AddResizeablePool<SetVisibilityMsg, GUIManager>	("set_visible",		100, 10, this, &GUIManager::ProcessSetVisibilityMsg);
	m_msgPoolMgr->AddResizeablePool<SetEnableMsg, GUIManager>		("set_enable",		100, 10, this, &GUIManager::ProcessSetEnableMsg);
}

void GUIManager::Update(float dt)
{
	m_msgPoolMgr->Process();

	for (auto layer : m_layers)
	{
		//if (layer.second->IsEnabled())
		//{
			layer.second->RefreshLayer(dt);
		//}
	}

	m_desktop.Refresh();
	m_desktop.Update(dt);
}

void GUIManager::Draw()
{
	if ( m_applicationWindow ) {		
		m_sfgui.Display(*m_applicationWindow);
	}
}

void GUIManager::HandleEvent(sf::Event& ev)
{
	m_desktop.HandleEvent(ev);
}

void GUIManager::LoadThemeFile(const char* file)
{
	//m_desktop.LoadThemeFromFile(file);
}

void GUIManager::AddLayer(GUILayer* layer, const char* name)
{
	layer->SaveDeskPtr(&m_desktop);
		
	// adds the window layer creates to the desktop
	// saves all created widgets to the map
	SFGWinPtr layerWin = layer->CreateElements(m_widgets);
	m_desktop.Add(layerWin);
		
	m_layers[name] = layer;
}

void GUIManager::CreateLayer(GUILayer* layer, const char* name)
{
	if (!layer || !name)
	{
		std::cout << "Could not create layer with name " << name << std::endl;
	}

	SendAddLayerMsg(AddLayerMsg(layer, name));
}

void GUIManager::RemoveLayer(const char* name)
{
	if (name)
	{
		LayerMap::const_iterator iter = m_layers.find(name);
		if (iter != m_layers.end())
		{
			SendRemoveLayerMsg(name);
		}
	}
}

void GUIManager::SendAddLayerMsg(const AddLayerMsg& msg)
{
	m_msgPoolMgr->LockPool<AddLayerMsg, GUIManager>("add_layer");
	AddLayerMsg* m = m_msgPoolMgr->GetMessageByName<AddLayerMsg, GUIManager>("add_layer");

	if (m)
	{
		m->layer	= msg.layer;
		m->name		= msg.name;
	}

	m_msgPoolMgr->UnlockPool<AddLayerMsg, GUIManager>("add_layer");
}

void GUIManager::SendRemoveLayerMsg(const RemoveLayerMsg& msg)
{
	m_msgPoolMgr->LockPool<RemoveLayerMsg, GUIManager>("remove_layer");
	RemoveLayerMsg* m = m_msgPoolMgr->GetMessageByName<RemoveLayerMsg, GUIManager>("remove_layer");

	if (m)
	{
		m->name	= msg.name;
	}

	m_msgPoolMgr->UnlockPool<RemoveLayerMsg, GUIManager>("remove_layer");
}

void GUIManager::SendVisMsg(const SetVisibilityMsg& msg)
{
	m_msgPoolMgr->LockPool<SetVisibilityMsg, GUIManager>("set_visible");
	SetVisibilityMsg* m = m_msgPoolMgr->GetMessageByName<SetVisibilityMsg, GUIManager>("set_visible");

	if (m)
	{
		m->name		= msg.name;
		m->value	= msg.value;
	}

	m_msgPoolMgr->UnlockPool<SetVisibilityMsg, GUIManager>("set_visible");
}

void GUIManager::SendEnableMsg(const SetEnableMsg& msg)
{
	m_msgPoolMgr->LockPool<SetEnableMsg, GUIManager>("set_enable");
	SetEnableMsg* m = m_msgPoolMgr->GetMessageByName<SetEnableMsg, GUIManager>("set_enable");

	if (m)
	{
		m->name		= msg.name;
		m->value	= msg.value;
	}

	m_msgPoolMgr->UnlockPool<SetEnableMsg, GUIManager>("set_enable");
}

void GUIManager::ProcessAddLayerMsg(AddLayerMsg* msgs, int amount)
{
	for (AddLayerMsg* ptr = msgs; ptr != msgs + amount; ptr++)
	{
		AddLayer(ptr->layer, ptr->name);
	}
}

void GUIManager::ProcessRemoveLayerMsg(RemoveLayerMsg* msgs, int amount)
{
	for (RemoveLayerMsg* ptr = msgs; ptr != msgs + amount; ptr++)
	{
		delete m_layers[ptr->name];
		m_layers.erase(ptr->name);
	}
}

void GUIManager::ProcessSetVisibilityMsg(SetVisibilityMsg* msgs, int amount)
{
	for (SetVisibilityMsg* ptr = msgs; ptr != msgs + amount; ptr++)
	{
		LayerMap::iterator iter = m_layers.find(ptr->name);
		if (iter != m_layers.end())
		{
			iter->second->SetVisible(ptr->value);
		}
	}
}

void GUIManager::ProcessSetEnableMsg(SetEnableMsg* msgs, int amount)
{
	for (SetEnableMsg* ptr = msgs; ptr != msgs + amount; ptr++)
	{
		LayerMap::iterator iter = m_layers.find(ptr->name);
		if (iter != m_layers.end())
		{
			iter->second->SetEnabled(ptr->value);
		}
	}
}

