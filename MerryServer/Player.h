/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Player.h

		Created by: Vladyslav Dolhopolov
		Created on: 9/8/2014 12:37:43 PM

========================
*/
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "CharacterController.h"

#include <NetConsts.h>

#include <RakString.h>
#include <RakNetTypes.h>

namespace RakNet
{
	struct SystemAddress;
}

namespace vgs
{
	class CharacterEntity;
	//class PickableItem;
	class Level;

	struct PlayerState
	{
		enum class GamePhase : char
		{
			PREGAME,
			INGAME,
			POSTGAME,
		};

		enum class IngameStatus : char
		{
			ALIVE,
			DEAD,
		};

		PlayerState()
			: m_phase(GamePhase::PREGAME)
			, m_status(IngameStatus::ALIVE)
		{
		}

		GamePhase		GetPhase() const			{ return m_phase; }
		void			SetPhase(GamePhase ph);

		IngameStatus	GetStatus() const			{ return m_status; }
		void			SetStatus(IngameStatus st);

	private:
		GamePhase		m_phase;
		IngameStatus	m_status;
	};

	struct SharedPlayerState
	{
		SharedPlayerState()
		{
			SetAll(true);
		}

		void SetAll(bool value)
		{
			amountChanged	= value;
			phaseChanged	= value;
			statusChanged	= value;
			teamChanged		= value;
		}

		bool amountChanged;
		bool phaseChanged;
		bool statusChanged;
		bool teamChanged;
	};

	/*
	========================

		Player

			Data related to a connected player. 

			Created by: Vladyslav Dolhopolov
			Created on: 9/8/2014 12:37:43 PM

	========================
	*/
	class Player : public CharacterController
	{
	public:
										Player(unsigned char uid, const RakNet::RakString& name, char characterClass, char team, const RakNet::SystemAddress& address);
										~Player();

		//PickableItem* const				CheckPickableItems(const Level* level);

		void							WritePregameSetup(RakNet::BitStream& bsOut) const;
		void							WritePregameUpdate(RakNet::BitStream& bsOut) const;
		void							WriteIngameUpdate(RakNet::BitStream& bsOut) const;
		void							WriteNewPlayerConnected(RakNet::BitStream& bsOut) const;
		void							WriteRespawn(RakNet::BitStream& bsOut) const;
		void							WriteStatsChanged(RakNet::BitStream& bsOut) const;

		void							SetUID(unsigned char val)								{ m_uid = val; }
		unsigned char					GetUID()												{ return m_uid; }

		const RakNet::RakString&		GetName() const											{ return m_name; }
		void							SetName(const RakNet::RakString& name)					{ m_name = name; }

		void							SetTeamTag(char val)									{ m_team = val; }
		char							GetTeamTag() const										{ return m_team; }

		void							SetCharacterClass(const char val)						{ m_class = val; }
		const char						GetCharacterClass() const								{ return m_class; }

		void							SetKills(unsigned char val)								{ m_kills = val; }
		unsigned char&					GetKills()												{ return m_kills; }

		void							SetDeaths(unsigned char val)							{ m_deaths = val; }
		unsigned char&					GetDeaths()												{ return m_deaths; }

		void							SetKillingSpreeCounter(unsigned char val)				{ m_killingSpreeCounter = val; }
		unsigned char&					GetKillingSpreeCounter()								{ return m_killingSpreeCounter; }

		void							SetMustDie(bool val = true)								{ m_mustDie = val; }
		bool							GetMustDie() const										{ return m_mustDie; }

		float&							GetRespawnCounter()										{ return m_respawnCounter; }
		tick_t&							GetLastUpdateSent()										{ return m_lastUpdateSent; }
		const RakNet::SystemAddress&	GetClientAddress() const								{ return m_clientAddress; }
		
		PlayerState&					GetState()												{ return m_state; }
		static SharedPlayerState&		GetSharedState()										{ return s_sharedState; }

	private:
		unsigned char					m_uid;
		RakNet::RakString				m_name;
		char							m_team;
		unsigned char					m_class;
		unsigned char					m_kills;
		unsigned char					m_deaths;
		unsigned char					m_killingSpreeCounter;
		bool							m_mustDie;
		float							m_respawnCounter;

		RakNet::SystemAddress			m_clientAddress;	
		tick_t							m_lastUpdateSent;

		PlayerState						m_state;
		static SharedPlayerState		s_sharedState;

	};

} // namespace merrymen

#endif __PLAYER_H__