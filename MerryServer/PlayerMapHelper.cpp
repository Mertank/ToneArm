/*
========================

Copyright (c) 2014 Vinyl Games Studio

	PlayerMapHelper.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 9/11/2014 3:14:05 PM

========================
*/
#include "PlayerMapHelper.h"
#include "Player.h"

using namespace vgs;

PlayerMapHelper::PlayerMapHelper()
{
}

PlayerMapHelper::~PlayerMapHelper()
{
	for (auto player : m_playerMap)
	{
		delete player.second;
	}
	m_playerMap.clear();
}

void PlayerMapHelper::Insert(unsigned char uid, Player* player)
{
	if (m_playerMap.count(uid))
	{
		throw PlayerAlreadyExists(uid);
	}
	else
	{
		m_playerMap[uid] = player;
	}
}

void PlayerMapHelper::Remove(unsigned char uid)
{
	if (m_playerMap.count(uid))
	{
		m_playerMap.erase(uid);
	}
	else
	{
		throw PlayerNonexists(uid);
	}
}

Player* PlayerMapHelper::operator[](unsigned char uid)
{
	if (m_playerMap.count(uid))
	{
		return m_playerMap[uid];
	}
	else
	{
		throw PlayerNonexists(uid);
	}
}

PlayerSet PlayerMapHelper::GetByPhaseSet(PlayerState::GamePhase phase) const
{
	PlayerSet ret;
	//if (Player::GetSharedState().phaseChanged)
	{
		for (const auto& pair : m_playerMap)
		{
			Player* pl = pair.second;
			if (pl->GetState().GetPhase() == phase)
			{
				ret.insert(pl);
			}
		}
		//Player::GetSharedState().phaseChanged = false;
	}
	return ret;
}

PlayerSet PlayerMapHelper::GetByStatusSet(PlayerState::IngameStatus status) const
{
	PlayerSet ret;
	//if (Player::GetSharedState().statusChanged)
	{
		for (const auto& pair : m_playerMap)
		{
			Player* pl = pair.second;
			if (pl->GetState().GetStatus() == status)
			{
				ret.insert(pl);
			}
		}
		//Player::GetSharedState().statusChanged = false;
	}
	return ret;
}

PlayerSet PlayerMapHelper::GetByTeamSet(char team) const
{
	PlayerSet ret;
	for (const auto& pair : m_playerMap)
	{
		Player* pl = pair.second;
		if (pl->GetTeamTag() == team)
		{
			ret.insert(pl);
		}
	}
	return ret;
}

PlayerSet PlayerMapHelper::GetAllSet() const
{
	PlayerSet ret;
	//if (Player::GetSharedState().amountChanged)
	{
		for (const auto& pair : m_playerMap)
		{
			ret.insert(pair.second);
		}
		//Player::GetSharedState().amountChanged = false;
	}
	return ret;
}

CharacterSet PlayerMapHelper::GetAllCharacterSet() const
{
	CharacterSet ret;
	for (const auto player : GetAllSet())
	{
		ret.insert(player->GetCharacter());
	}
	return ret;
}