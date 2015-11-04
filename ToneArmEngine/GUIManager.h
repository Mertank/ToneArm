/*
========================

Copyright (c) 2014 Vinyl Games Studio

	GUIManager.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/5/2014 1:37:56 PM

========================
*/
#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "MessagePoolManager.h"
#include "GUIMessages.h"

#include <unordered_map>
#include <list>

#include <SFGUI\SFGUI.hpp>

#include "VGSMacro.h"

namespace vgs
{
	class GUILayer;
	class MessagePoolManager;

	typedef std::unordered_map<const char*, sfg::Widget::Ptr>				WidgetMap;
	typedef	std::unordered_map<const char*, GUILayer*>						LayerMap;
	typedef sfg::Window::Ptr												SFGWinPtr;

	/*
	========================

		GUIManager

			Stores and manages gui widgets and layers.

			Created by: Vladyslav Dolhopolov
			Created on: 5/5/2014 1:37:56 PM

	========================
	*/
	class GUIManager
	{
	public:
								GUIManager();
								~GUIManager();
		
		void					Init();
		void					Update(float dt);
		void					Draw();
		void					HandleEvent(sf::Event& ev);

		void					SetApplicationWin(sf::Window* win)				{ m_applicationWindow = win; }
		void					LoadThemeFile(const char* file);

		void					SetProperties(const std::string& properties)	{ m_desktop.SetProperties(properties); }

		MessagePoolManager*		GetMsgPoolMgr()									{ return m_msgPoolMgr; }

		// next ones can be called from logic thread
		void					CreateLayer(GUILayer* layer, const char* name);
		void					RemoveLayer(const char* name);

		void					SendVisMsg(const SetVisibilityMsg& msg);
		void					SendEnableMsg(const SetEnableMsg& msg);

		// make sure you use this only to call const methods or to send gui messages
		template <class T>
		T*						GetLayer(const char* name);

	private:
		template <class T>
		void					AddLayer(const char* name);
		void					AddLayer(GUILayer* layer, const char* name);
		void					SendAddLayerMsg(const AddLayerMsg& msg);
		void					SendRemoveLayerMsg(const RemoveLayerMsg& msg);

		void					ProcessAddLayerMsg(AddLayerMsg* msgs, int amount);
		void					ProcessRemoveLayerMsg(RemoveLayerMsg* msgs, int amount);
		void					ProcessSetVisibilityMsg(SetVisibilityMsg* msgs, int amount);
		void					ProcessSetEnableMsg(SetEnableMsg* msgs, int amount);

	private:
		sf::Window*				m_applicationWindow;
		sfg::SFGUI				m_sfgui;
		sfg::Desktop			m_desktop;

		LayerMap				m_layers;
		WidgetMap				m_widgets; // depricated

		MessagePoolManager*		m_msgPoolMgr;

	};

	template <class T>
	void GUIManager::AddLayer(const char* name)
	{
		T* layer = new T();
		layer->SaveDeskPtr(&m_desktop);
		
		// adds the window layer creates to the desktop
		// saves all created widgets to the map
		SFGWinPtr layerWin = layer->CreateElements(m_widgets);
		m_desktop.Add(layerWin);
		
		m_layers[name] = static_cast<GUILayer*>(layer);
	}

	template <class T>
	T* GUIManager::GetLayer(const char* name)
	{
		if (m_layers.count(name))
		{
			return static_cast<T*>(m_layers[name]);
		}
		else
		{
			return nullptr;
		}
	}

} // namespace vgs

#endif __GUIMANAGER_H__