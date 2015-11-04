/*
========================

Copyright (c) 2014 Vinyl Games Studio

	PauseGUI.h

		Created by: Vladyslav Dolhopolov
		Created on: 7/29/2014 11:48:29 AM

========================
*/
#ifndef __PAUSEGUI_H__
#define __PAUSEGUI_H__

#include "SinglePlayerGameScene.h"

#include "../ToneArmEngine/RelativeGUILayer.h"

namespace merrymen
{
	/*
	========================

		PauseGUI

			Pause menu for the game.
			Contains buttons to disconnect or exit to main menu.

			Created by: Vladyslav Dolhopolov
			Created on: 7/29/2014 11:48:29 AM

	========================
	*/
	class PauseGUI : public vgs::RelativeGUILayer
	{
	public:
		enum Type
		{
			SINGLE,
			MULTI
		};
		
								PauseGUI(Type type);
								~PauseGUI( void );

		virtual vgs::SFGWinPtr	CreateElements(vgs::WidgetMap& widgetMap)	override;

	protected:
		virtual void			ConnectSignals()							override;
		virtual void			RefreshLayer(float dt)						override;
		virtual void			InitPools()									{}

	private:
		void					OnDisconnect();
		void					OnExit();

		Type					m_type;

		sfg::Button::Ptr		m_exitBtn;

		sfg::Box::Ptr			m_buttonBox;
	};

} // namespace merrymen

#endif __PAUSEGUI_H__