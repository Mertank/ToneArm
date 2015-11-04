/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Level.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 9/9/2014 3:07:29 PM

========================
*/
#include "Level.h"
#include "ServerEngine.h"
#include "Player.h"
#include "CharacterEntity.h"
#include "PickableItemSS.h"

#include "../ToneArmEngine/PhysicsModule.h"
#include "../ToneArmEngine/BoxCollider.h"
#include "../ToneArmEngine/CylinderCollider.h"
#include "../ToneArmEngine/Trigger.h"

#include <GameObject.h>

#include <iostream>

using namespace vgs;

Level::Level(const char* path)
{
	m_collidersOwner = GameObject::Create();

	std::cout << "Loading level collision map..." << std::endl;
	m_levelRes = ServerEngine::GetInstance()
		->GetModuleByType<CachedResourceLoader>(EngineModuleType::RESOURCELOADER)->LoadResource<LevelResource>(path, (char)0xA6);

	// create and store level colliders
	const std::vector<Shape*>& collisions = m_levelRes->GetColliders();
	
	for (std::vector<Shape*>::const_iterator collidersItr = collisions.begin(); collidersItr != collisions.end(); ++collidersItr) {

		if ((*collidersItr)->GetRTTI() == BoxShape::rtti) {

			BoxCollider* box = new BoxCollider(m_collidersOwner, (*collidersItr)->GetPosition(), ((BoxShape*)(*collidersItr))->GetDimensions(), false);
			m_collidersOwner->AddComponent(box);
			box->SetRotation((*collidersItr)->GetRotation());			
		} else {

			CylinderShape* shape = static_cast<CylinderShape*>(*collidersItr);
			CylinderCollider* cylinder = new CylinderCollider(m_collidersOwner, shape->GetPosition(), shape->GetRadius(),
															  shape->GetHeight(), false);
			m_collidersOwner->AddComponent(cylinder);
			cylinder->SetRotation((*collidersItr)->GetRotation());
		}
	}

	// create partition tree based on level coliders
	PhysicsModule* physics = ServerEngine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
	physics->CreateSpacePartitionTree();

	// create triggers
	const std::vector<TriggerInfo*> triggers = m_levelRes->GetTriggers();

	for (auto trigger : triggers) {
		
		Trigger* triggerObject = nullptr;
		Shape* triggerShape = trigger->TriggerShape;

		if (triggerShape->GetRTTI() == CylinderShape::rtti) {
			triggerObject = new Trigger(m_collidersOwner, trigger->TriggerTag, new CylinderShape(*static_cast<CylinderShape*>(triggerShape)), nullptr, nullptr);
		}
		else if (triggerShape->GetRTTI() == BoxShape::rtti) {
			triggerObject = new Trigger(m_collidersOwner, trigger->TriggerTag, new BoxShape(*static_cast<BoxShape*>(triggerShape)), nullptr, nullptr);
		}
		
		m_collidersOwner->AddComponent(triggerObject);
	}

	std::cout << path << " loaded" << std::endl;
}

Level::~Level()
{
	DELETE_STD_VECTOR_POINTER(PickableItemSS, m_pickableObjects)

	// make sure that PhysicsModule knows the colliders that belong to the level do not exist any more
	PhysicsModule* physics = ServerEngine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);

	for (auto collider : m_collidersOwner->GetAllComponentsOfType<ColliderComponent>()) {
		physics->RemoveCollider(collider);
	}

	physics->DeleteSpacePartiotionTree();

	delete m_collidersOwner;
}

void Level::SpawnPlayer(Player* player, SpawnMode mode, const std::set<Player*>* enemyTeam)
{
	switch (mode)
	{
	case SpawnMode::DEBUG:
		player->GetCharacter()->SetPosition(GetDebugSpawnPoint());
		break;

	case SpawnMode::RANDOM:
		player->GetCharacter()->SetPosition(GetRandomSpawnPoint());
		break;

	case SpawnMode::ENEMY_CLEAR:
		player->GetCharacter()->SetPosition(GetSpawnPointAwayFromEnemies(*enemyTeam));
		break;

	default:
		break;
	}
}

glm::vec3 Level::GetDebugSpawnPoint()
{
	static float offsetX = -2850.0f;
	static float offsetZ = 1500.0f;
	offsetX += 75.0f;
	offsetZ += 75.0f;

	return glm::vec3(offsetX, m_levelRes->GetSpawnPoints()[0].y, offsetZ);
}

glm::vec3& Level::GetRandomSpawnPoint()
{
	int r = rand() % m_levelRes->GetSpawnPoints().size();
	return m_levelRes->GetSpawnPoints()[r];
}

glm::vec3& Level::GetSpawnPointAwayFromEnemies(const std::set<Player*>& enemyTeam)
{
	// pick a spawnpoint, which doesn't have enemies next to it
	
	if (enemyTeam.empty())
	{
		return GetRandomSpawnPoint();
	}
	else
	{
		std::vector<glm::vec3>&	points			= m_levelRes->GetSpawnPoints();
		std::vector<glm::vec3*>	clearPoints;
		static const float		thresholdDist	= 1000.f;

		for (auto& point : points)
		{
			glm::vec2 _2dSpawn(point.x, point.z);

			bool clear = true;
			for (const auto enemy : enemyTeam)
			{
				glm::vec2 _2dEnemy(enemy->GetCharacter()->GetPosition().x, enemy->GetCharacter()->GetPosition().z);

				if (glm::distance(_2dEnemy, _2dSpawn) < thresholdDist)
				{
					clear = false;
					break;
				}
			}

			if (clear)
			{
				clearPoints.push_back(&point);
			}
		}

		if (clearPoints.empty())
		{
			return GetRandomSpawnPoint();
		}
		else
		{
			int r = rand() % clearPoints.size();
			return *clearPoints[r];
		}
	}
}

void Level::AddPickableObject(PickableItemSS* const item) {

	// add the item to the level if it has not already been added
	if (std::count(m_pickableObjects.begin(), m_pickableObjects.end(), item) == 0) {

		m_pickableObjects.push_back(item);
		item->SetLevel(this);
	}

	// turn on collision detection for that object
	if (item->IsActive()) {

		PhysicsModule* physics = ServerEngine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
		physics->AddPickableItem(item);
	}
}

void Level::RemovePickableObject(PickableItemSS* const item)
{
	for (unsigned int i = 0; i < m_pickableObjects.size(); i++)
	{
		if (item == m_pickableObjects[i])
		{
			m_pickableObjects.erase(m_pickableObjects.begin() + i);

			return;
		}
	}

	// turn off collision detection for that object
	if (item->IsActive()) {

		PhysicsModule* physics = ServerEngine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
		physics->RemovePickableItem(item);
	}
}
