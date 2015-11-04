/*
========================

Copyright (c) 2014 Vinyl Games Studio

	GUILayer.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/5/2014 3:39:00 PM

========================
*/
#ifndef __GUILAYER_H__
#define __GUILAYER_H__

#include <unordered_map>
#include <SFGUI\SFGUI.hpp>

namespace vgs
{

	typedef std::unordered_map<const char*, sfg::Widget::Ptr> WidgetMap;
	typedef sfg::Window::Ptr SFGWinPtr;

	/*
	========================

		GUILayer

			Represents separate window of gui.

			Created by: Vladyslav Dolhopolov
			Created on: 5/5/2014 3:00:59 PM

	========================
	*/
	class GUILayer
	{
		friend class GUIManager;

	public:
							GUILayer();
		virtual				~GUILayer ();
		// has to be called in the end of child's CreateElements()
		virtual SFGWinPtr	CreateElements(WidgetMap& widgetMap) = 0;

		SFGWinPtr			GetWindow()								{ return m_window; }

		void				SetVisible(bool visible);
		bool				IsVisible() const						{ return m_visible; }

		void				SetEnabled(bool enabled)				{ m_enabled = enabled; }
		bool				IsEnabled() const						{ return m_enabled; }

		void				SaveDeskPtr(sfg::Desktop* desk)			{ m_deskPtr = desk; }

	protected:
		virtual void		RefreshLayer(float dt)					{ m_window->Update(dt); }

		virtual void		ConnectSignals()	= 0;
		virtual void		InitPools()			= 0;

	protected:
		SFGWinPtr			m_window;
		bool				m_visible;
		bool				m_enabled;
		WidgetMap*			m_widgetMap;

		sfg::Desktop*		m_deskPtr;

	};

} // namespace vgs

#endif __GUILAYER_H__