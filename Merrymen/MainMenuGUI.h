/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MainMenuGUI.h

		Created by: Vladyslav Dolhopolov
		Created on: 7/29/2014 11:48:29 AM

========================
*/
#ifndef __MAINMENUGUI_H__
#define __MAINMENUGUI_H__

#include "../Merrymen/SinglePlayerGameScene.h"
#include "../ToneArmEngine/RelativeGUILayer.h"

namespace merrymen
{
	/*
	========================

		MainMenuGUI

			Main menu for the game.
			Contains buttons to start sp or mp game.

			Created by: Vladyslav Dolhopolov
			Created on: 7/29/2014 11:48:29 AM

	========================
	*/
	class MainMenuGUI : public vgs::RelativeGUILayer
	{
	public:
		virtual vgs::SFGWinPtr	CreateElements(vgs::WidgetMap& widgetMap)	override;

	protected:
		virtual void			ConnectSignals()							override;
		virtual void			RefreshLayer(float dt)						override;
		virtual void			InitPools()									{}

	private:
		void					OnMpStart();
		void					OnExit();
#ifdef TONEARM_DEBUG		
		void					OnSpStart();
		void					OnAnimStart();
#endif

		sfg::Button::Ptr		m_mpStart;
		sfg::Button::Ptr		m_exit;
#ifdef TONEARM_DEBUG
		sfg::Button::Ptr		m_spStart;
		sfg::Button::Ptr		m_animTestStart;
#endif

		sfg::Fixed::Ptr			m_aligner;
		sfg::Box::Ptr			m_buttonBox;

		sfg::Image::Ptr			m_image;
	};

} // namespace merrymen

#endif __MAINMENUGUI_H__