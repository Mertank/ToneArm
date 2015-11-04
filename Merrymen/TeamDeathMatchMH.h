/*
========================

Copyright (c) 2014 Vinyl Games Studio

	TeamDeathMatchMH.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/23/2014 3:33:18 PM

========================
*/
#ifndef __TeamDeathMatchMH_H__
#define __TeamDeathMatchMH_H__

#include "..\ToneArmEngine\MessageHandler.h"
#include "..\ToneArmEngine\ScoreTableGUI.h"

#include <WeaponStats.h>

using namespace vgs;

namespace vgs
{
	class GUIManager;
}

namespace merrymen
{
	class Character;

	/*
	========================

		TeamDeathMatchMH

			Represents somewhat team death match game mode on client side.

			Created by: Vladyslav Dolhopolov
			Created on: 5/23/2014 3:33:18 PM

	========================
	*/
	class TeamDeathMatchMH : public MessageHandler
	{
	public:
								TeamDeathMatchMH();
		virtual					~TeamDeathMatchMH();

		virtual void			ProcessMessage(unsigned int pid, RakNet::Packet* packet)	override;
		virtual void			SendSetupRequest()											override;

		virtual void			ProcessPlayerDisconnection(unsigned int uid)				override;
		virtual void			OpenPregameSetupWin()										override;

		void					SendChangeWeaponRequest(WeaponID wid);
		void					SendReloadRequest();

	private:
		Character*				CreatePlayer(unsigned char uid, char chClass, char team, const RakNet::RakString& name, bool local = false);

		GUIManager*				m_guiMgr;

		// temporary
		std::queue<AddRowMsg>	m_rowsToAdd;

	};

} // namespace merrymen

#endif __TeamDeathMatchMH_H__