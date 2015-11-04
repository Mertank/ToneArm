/*
========================

Copyright (c) 2014 Vinyl Games Studio

	PlayerMapHelper.h

		Created by: Vladyslav Dolhopolov
		Created on: 9/11/2014 3:14:05 PM

========================
*/
#ifndef __PLAYERMAPHELPER_H__
#define __PLAYERMAPHELPER_H__

#include "Player.h"

#include <unordered_set>
#include <unordered_map>
#include <exception>
#include <sstream>

namespace vgs
{
	/*enum class SetName
	{
		ALIVE,
		DEAD,
		INGAME,
		PREGAME,
		POSTGAME,
		ALL,
	};*/

	typedef std::unordered_map<unsigned char, Player*>	PlayerMap;
	typedef std::unordered_set<Player*>					PlayerSet;
	typedef std::unordered_set<CharacterEntity*>		CharacterSet;
	//typedef std::unordered_map<SetName, PlayerSet>	PlayerSetMap;

	class PlayerMapHelperException : public std::runtime_error
	{
	protected:
		PlayerMapHelperException(unsigned char uid)
			: std::runtime_error("PlayerMapHelperException")
			, m_elemUid(uid)
		{
		}

		unsigned char m_elemUid;
	};

	class PlayerAlreadyExists : protected PlayerMapHelperException
	{
	public:
		PlayerAlreadyExists(unsigned char uid)
			: PlayerMapHelperException(uid)
		{
		}
		
		virtual const char* what() const override
		{
			std::stringstream ss;
			ss << "Player with UID " << m_elemUid << " already exist";
			return ss.str().c_str();
		}
	};

	class PlayerNonexists : protected PlayerMapHelperException
	{
	public:
		PlayerNonexists(unsigned char uid)
			: PlayerMapHelperException(uid)
		{
		}
		
		virtual const char* what() const override
		{
			std::stringstream ss;
			ss << "Player with UID " << m_elemUid << " doesn't exist";
			return ss.str().c_str();
		}
	};

	/*
	========================

		PlayerMapHelper

			Helps manage players map, throwing appropriate exceptions.

			Created by: Vladyslav Dolhopolov
			Created on: 9/11/2014 3:14:05 PM

	========================
	*/
	class PlayerMapHelper
	{
	public:
						PlayerMapHelper();
						~PlayerMapHelper();

		void			Insert(unsigned char uid, Player* player);
		void			Remove(unsigned char uid);
		Player*			operator[](unsigned char uid);

		PlayerSet		GetByPhaseSet(PlayerState::GamePhase phase) const;
		PlayerSet		GetByStatusSet(PlayerState::IngameStatus status) const;
		PlayerSet		GetByTeamSet(char team) const;

		PlayerSet		GetAllSet() const;

		CharacterSet	GetAllCharacterSet() const;

	private:
		PlayerMap		m_playerMap;
		//PlayerSetMap	m_sets;

	};

} // namespace vgs

#endif __PLAYERMAPHELPER_H__