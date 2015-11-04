/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RoundInfoGUI.h

		Created by: Vladyslav Dolhopolov
		Created on: 8/5/2014 4:21:38 PM

========================
*/
#ifndef __ROUNDINFOGUI_H__
#define __ROUNDINFOGUI_H__

#include "RelativeGUILayer.h"

#include <GetTime.h>

#include <SFML\System\Mutex.hpp>

namespace merrymen
{
	struct UpdateTimer
	{
		UpdateTimer()
			: m_started(0)
			, m_duration(0)
		{
		}

		UpdateTimer(double s, unsigned int d)
			: m_started(s)
			, m_duration(d)
		{
		}

		double			m_started;
		unsigned int	m_duration;
	};

	/*
	========================

		RoundInfoGUI

			Description.

			Created by: Vladyslav Dolhopolov
			Created on: 8/5/2014 4:21:38 PM

	========================
	*/
	class RoundInfoGUI : public vgs::RelativeGUILayer
	{
	public:
								RoundInfoGUI();
		virtual vgs::SFGWinPtr	CreateElements(vgs::WidgetMap& widgetMap)	override;
		
		void					SetUpdateTimer(const UpdateTimer& msg);
		void					SetRoundNumber(unsigned int n);

	protected:
		virtual void			ConnectSignals()							{};
		virtual void			RefreshLayer(float dt)						override;
		virtual void			InitPools()									{}

	private:
		sfg::Label::Ptr			m_timerLbl;

		UpdateTimer				m_valuesToUpdate;
		sf::Mutex				m_updateMutex;

		unsigned int			m_roundNumber;
	};

} // namespace vgs

#endif __ROUNDINFOGUI_H__