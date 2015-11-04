/*
========================

Copyright (c) 2014 Vinyl Games Studio

	HeistTestGameMode.h

		Created by: Vladyslav Dolhopolov
		Created on: 10/3/2014 4:14:15 PM

========================
*/
#ifndef __HEISTTESTGAMEMODE_H__
#define __HEISTTESTGAMEMODE_H__

#include "WrapperGameMode.h"

namespace vgs
{
	class GameObject;
	class Trigger;

	/*
	========================

		HeistTestGameMode

			Description.

			Created by: Vladyslav Dolhopolov
			Created on: 10/3/2014 4:14:15 PM

	========================
	*/
	class HeistTestGameMode : public WrapperGameMode
	{
	public:
						HeistTestGameMode(GameMode* dec);

		virtual bool	Init()																override;
		virtual void	Update(float dt)													override;
		virtual void	ProcessMessage(unsigned char pid, RakNet::Packet* packet)			override;

	private:
		void			OnTriggerEnter(GameObject* const character, Trigger* const trigger);
		void			OnTriggerExit(GameObject* const character, Trigger* const trigger);

	};

} // namespace vgs

#endif __HEISTTESTGAMEMODE_H__