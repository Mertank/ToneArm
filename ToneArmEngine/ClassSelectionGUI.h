/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ClassSelectionGUI.h

		Created by: Vladyslav Dolhopolov
		Created on: 6/19/2014 3:21:22 PM

========================
*/
#ifndef __CLASSSELECTIONGUI_H__
#define __CLASSSELECTIONGUI_H__

#include "../Merrymen/Character.h"
#include "../ToneArmEngine/RelativeGUILayer.h"

namespace merrymen
{
	/*
	========================

		ClassSelectionGUI

			Dialog which shows up after connection to a server.
			Allows to select team and class of character.

			Created by: Vladyslav Dolhopolov
			Created on: 6/19/2014 3:21:22 PM

	========================
	*/
	class ClassSelectionGUI : public vgs::RelativeGUILayer
	{
	public:
								ClassSelectionGUI(const std::function<void()>& func);
		virtual					~ClassSelectionGUI() {}

		virtual vgs::SFGWinPtr	CreateElements(vgs::WidgetMap& widgetMap)	override;
		virtual void			RefreshLayer(float dt)						override;

		std::string				GetName()									const;
		const char				GetCharacterClass()							const;
		const char				GetTeam()									const;

	protected:
		virtual void			ConnectSignals()							{}
		virtual void			InitPools()									{}

	private:
		void					StartSpinner();
		void					StopSpinner();
		void					OnDonePressed();

	private:
		sfg::Entry::Ptr			m_nameEntry;
		sfg::Button::Ptr		m_doneBtn;
		bool					m_donePressed;

		sfg::RadioButton::Ptr	m_team1Radio;
		sfg::RadioButton::Ptr	m_team2Radio;

		sfg::RadioButton::Ptr	m_class1Radio;
		sfg::RadioButton::Ptr	m_class2Radio;

		sfg::Box::Ptr			m_spinnerBox;
		sfg::Spinner::Ptr		m_spinner;
		bool					m_spinStarted;

		std::function<void()>	m_callback;

	};

} // namespace merrymen

#endif __CLASSSELECTIONGUI_H__