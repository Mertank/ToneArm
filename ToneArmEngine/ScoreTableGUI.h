/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ScoreTableGUI.h

		Created by: Vladyslav Dolhopolov
		Created on: 6/17/2014 1:07:58 PM

========================
*/
#ifndef __SCORETABLEGUI_H__
#define __SCORETABLEGUI_H__

#include <unordered_map>

#include "../Merrymen/Character.h"
#include "../ToneArmEngine/RelativeGUILayer.h"

#include <SFML\System\Mutex.hpp>

namespace vgs
{
	class NetworkNode;
}

namespace merrymen
{
	struct AddRowMsg
	{
		std::string		name;
		unsigned char	uid;
		unsigned char	kills;
		unsigned char	deaths;
		char			team;
		bool			local;
	};

	struct RemoveRowMsg
	{
		unsigned char	uid;
		char			team;
	};

	struct UpdateRowMsg
	{
		unsigned char	uid;
		unsigned char	kills;
		unsigned char	deaths;
	};

	/*
	========================

		ScoreTableGUI

			Window for showing players' name, score and ping.

			Created by: Vladyslav Dolhopolov
			Created on: 6/17/2014 1:07:58 PM

	========================
	*/
	class ScoreTableGUI : public vgs::RelativeGUILayer 
	{
		struct Row
		{
			sfg::Box::Ptr	box;
			sfg::Label::Ptr name;
			sfg::Label::Ptr kills;
			sfg::Label::Ptr deaths;
			sfg::Label::Ptr ping;
		};

		typedef std::unordered_map<unsigned char, Row> UIDRowMap;

	public:
								ScoreTableGUI();
		virtual					~ScoreTableGUI();

		virtual vgs::SFGWinPtr	CreateElements(vgs::WidgetMap& widgetMap)					override;

		void					SendAddMsg(const AddRowMsg& msg);
		void					SendRemoveMsg(const RemoveRowMsg& msg);
		void					SendUpdateMsg(const UpdateRowMsg& msg);

		void					ProcessAddRowMsg(AddRowMsg* msgs, int amount);
		void					ProcessRemoveRowMsg(RemoveRowMsg* msgs, int amount);
		void					ProcessUpdateRowMsg(UpdateRowMsg* msgs, int amount);

		void					SetEnabled(bool enabled = true)								{ m_enabled = enabled; }
		bool					IsEnabled()													{ return m_enabled; }

		void					SetPostgame(bool post = true)								{ m_postMutex.lock();
																							  m_postgame = post;
																							  m_postMutex.unlock(); }

	protected:
		virtual void			ConnectSignals() {}
		virtual void			InitPools();
		virtual void			RefreshLayer(float dt)										override;

	private:
		void					AddRow(unsigned char kills, unsigned char deaths, char team, std::string name, unsigned char uid, bool local);
		void					RemoveRow(unsigned char uid, char team);
		void					UpdateRow(unsigned char uid, unsigned char kills, unsigned char deaths);

		sfg::Box::Ptr			m_team1Box;
		sfg::Box::Ptr			m_team2Box;

		UIDRowMap				m_table;

		bool					m_enabled;
		bool					m_postgame;
		sf::Mutex				m_postMutex;
	};

} // namespace merrymen

#endif __SCORETABLEGUI_H__