/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Level.h

		Created by: Vladyslav Dolhopolov
		Created on: 9/9/2014 3:07:29 PM

========================
*/
#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "../ToneArmEngine/LevelResource.h"

#include <set>

namespace merrymen {
class PickableItemSS;
}

namespace vgs
{
	class Player;
	class CharacterEntity;
	class GameObject;
	class Trigger;

	enum class SpawnMode
	{
		DEBUG,
		RANDOM,
		ENEMY_CLEAR,
	};

	/*
	========================

		Level

			Level stores colliders, pickable objects.
			Has functions to spawn players.

			Created by: Vladyslav Dolhopolov
			Created on: 9/9/2014 3:07:29 PM

	========================
	*/
	class Level
	{
		typedef std::shared_ptr<LevelResource> LevelRes;

	public:
															Level(const char* path);
															~Level();

		void												SpawnPlayer(Player* player, SpawnMode mode, const std::set<Player*>* enemyTeam = nullptr);

		const std::vector<merrymen::PickableItemSS*>&		GetPickableObjects() const											{ return m_pickableObjects; }
		void												AddPickableObject(merrymen::PickableItemSS* const item);
		void												RemovePickableObject(merrymen::PickableItemSS* const item);

	private:
		glm::vec3&											GetRandomSpawnPoint();
		glm::vec3&											GetSpawnPointAwayFromEnemies(const std::set<Player*>& enemyTeam);
		glm::vec3											GetDebugSpawnPoint();

		GameObject* const									GetCollidersOwner() const											{ return m_collidersOwner; }

	private:
		LevelRes											m_levelRes;

		GameObject*											m_collidersOwner;
		std::vector<merrymen::PickableItemSS*>				m_pickableObjects;
	};

} // namespace vgs

#endif __LEVEL_H__