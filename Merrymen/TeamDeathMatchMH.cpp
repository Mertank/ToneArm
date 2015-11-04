/*
========================

Copyright (c) 2014 Vinyl Games Studio

	TeamDeathMatchMH.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 5/23/2014 3:33:18 PM

========================
*/
#include "TeamDeathMatchMH.h"
#include "GameScene.h"
#include "MultiplayerGameScene.h"
#include "TankCharacter.h"
#include "SniperCharacter.h"
#include "Effect.h"
#include "CompassNode.h"
#include "PickableBuffCS.h"

#include <NetworkMessageIDs.h>

#include "../ToneArmEngine/NetworkModule.h"
#include "../ToneArmEngine/MathHelper.h"
#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/Scene.h"
#include "../ToneArmEngine/NetworkNode.h"
#include "../ToneArmEngine/FixedCamera.h"
#include "../ToneArmEngine/MathHelper.h"
#include "../ToneArmEngine/VGSAssert.h"
#include "../ToneArmEngine/RTTI.h"
#include "../ToneArmEngine/GUIManager.h"
#include "../ToneArmEngine/LANMenu.h"
#include "../ToneArmEngine/ScoreTableGUI.h"
#include "../ToneArmEngine/ClassSelectionGUI.h"
#include "../ToneArmEngine/OpenGLRenderModule.h"
#include "../ToneArmEngine/OpenGLGUIPass.h"
#include "../ToneArmEngine/UICommand.h"
#include "../ToneArmEngine/LevelNode.h"
#include "../ToneArmEngine/AnimationNode.h"
#include "../ToneArmEngine/RoundInfoGUI.h"

#include <BitStream.h>

#include <iostream>

using namespace vgs;
using namespace merrymen;

TeamDeathMatchMH::TeamDeathMatchMH()
{
	m_guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetPass<OpenGLGUIPass>()->GetGUIManager();
}

TeamDeathMatchMH::~TeamDeathMatchMH() 
{
	while(!m_rowsToAdd.empty()) 
	{
		m_rowsToAdd.pop();
	}
}

void TeamDeathMatchMH::ProcessMessage(unsigned int pid, RakNet::Packet* packet)
{
	// temporary
	if (m_guiMgr->GetLayer<ScoreTableGUI>("score_win"))
	{
		while (!m_rowsToAdd.empty())
		{
			m_guiMgr->GetLayer<ScoreTableGUI>("score_win")->SendAddMsg(m_rowsToAdd.front());
			m_rowsToAdd.pop();
		}
	}

	switch (pid)
	{
	case MerryMessage::ID_MERRY_PREGAME_SETUP_ANSWER:
		{
			std::cout << "Pregame setup answer packet received" << std::endl;

			RakNet::BitStream bsIn(packet->data, packet->length, false);

			unsigned char		localUID;
			char				teamTag;
			char				chClass;
			RakNet::RakString	name;
			//double			roundStarted;
			//unsigned int		roundDuration;
			//unsigned int		roundNumber;

			bsIn.IgnoreBytes(1);
			bsIn.Read(localUID); 
			bsIn.Read(teamTag);
			bsIn.Read(chClass);
			bsIn.Read(name);
			//bsIn.Read(roundStarted);
			//bsIn.Read(roundDuration);
			//bsIn.Read(roundNumber);

			Character* player = CreatePlayer(localUID, chClass, teamTag, name, true);
			if (!player)
			{
				std::cout << "Network: couldn't create local player in MerryMessage::ID_MERRY_PREGAME_SETUP_ANSWER" << std::endl;
				break;
			}

			// TODO: take limit numbers out of a level map
			float x, y, z;//, angle;
			bsIn.ReadFloat16(x, -5000.f, +5000.f);
			bsIn.ReadFloat16(y, -5000.f, +5000.f);
			bsIn.ReadFloat16(z, -5000.f, +5000.f);
			//bsIn.ReadFloat16(angle, -360.f, +360.f);
			player->SetPosition(glm::vec3(x, y, z));

			if (m_guiMgr->GetLayer<ScoreTableGUI>("score_win"))
			{
				// add entry to the score table
				AddRowMsg msg = { std::string(name.C_String()), localUID, player->GetKills(), player->GetDeaths(), player->GetTeamTag(), true };
				m_guiMgr->GetLayer<ScoreTableGUI>("score_win")->SendAddMsg(msg);
			}

			//m_guiMgr->GetLayer<RoundInfoGUI>("round_win")->SetUpdateTimer(UpdateTimer(roundStarted, roundDuration));
			//m_guiMgr->SendVisMsg(SetVisibilityMsg("round_win", true));
			
			std::cout << "Network: This client's UID is " << (unsigned)localUID << std::endl;
		}
		break;

	case MerryMessage::ID_MERRY_PREGAME_UPDATE:
		{
			std::cout << "Pregame update packet received" << std::endl;

			// receive transforms of other players
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			bsIn.IgnoreBytes(1);
			unsigned char playerAmount;
			bsIn.Read(playerAmount);

			for (unsigned char i = 0; i < playerAmount; i++)
			{
				unsigned char		uid;
				char				teamTag;
				char				charClass;
				RakNet::RakString	name;
				unsigned char		kills;
				unsigned char		deaths;
				WeaponID			wId;
				glm::vec3			position;

				bsIn.Read(uid);
				bsIn.Read(name);
				bsIn.Read(teamTag);
				bsIn.Read(charClass);
				bsIn.Read(kills);
				bsIn.Read(deaths);
				bsIn.Read(wId);
				bsIn.ReadFloat16(position.x, -5000.f, +5000.f);
				bsIn.ReadFloat16(position.y, -5000.f, +5000.f);
				bsIn.ReadFloat16(position.z, -5000.f, +5000.f);

				NetworkNode* local = NetworkNode::GetLocalPlayer();
				if (local && local->GetUID() == uid)
				{
					std::cout << "It was gonna create second copy of local player!" << std::endl;
					continue;
				}

				Character* player = CreatePlayer(uid, charClass, teamTag, name);
				if (!player)
				{
					std::cout << "Network: couldn't create player with uid " << (unsigned)uid << " in MerryMessage::ID_MERRY_PREGAME_UPDATE" << std::endl;
					break;
				}
				player->SetKills(kills);
				player->SetDeaths(deaths);
				player->SetActiveWeapon(wId);
				player->SetPosition(position);

				if (m_guiMgr->GetLayer<ScoreTableGUI>("score_win"))
				{
					// add to the score table
					AddRowMsg msg = { std::string(name.C_String()), uid, player->GetKills(), player->GetDeaths(), player->GetTeamTag() };
					m_guiMgr->GetLayer<ScoreTableGUI>("score_win")->SendAddMsg(msg);
				}
			}

			// send back pregame update processed packet
			char data[2];
			data[0] = (char)MerryMessage::ID_MERRY_PREGAME_UPDATE_PROCESSED;
			data[1] = NetworkNode::GetLocalPlayer()->GetUID();
			m_module->Send(data, 2);

			NetworkNode::GetLocalPlayer()->TurnInput(); // on
			std::cout << "Input is turned on for local player" << std::endl;
		}
		break;

	case MerryMessage::ID_MERRY_INGAME_UPDATE:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			RakNet::Time serverTime;
			
			bsIn.IgnoreBytes(1);
			bsIn.Read(serverTime);
			bsIn.IgnoreBytes(1);

			unsigned char playerAmount;
			bsIn.Read(playerAmount);

			unsigned char	uid;
			glm::vec3		position(0.0f);
			float			angle;

			for (unsigned char i = 0; i < playerAmount; i++)
			{
				bsIn.Read(uid);

				// check if the package for the local character has arrived
				NetworkNode* localCharacter = NetworkNode::GetLocalPlayer();
				bool isLocalCharacter = (localCharacter != nullptr && uid == NetworkNode::GetLocalPlayer()->GetUID());
				
				if (m_netNodes.count(uid))
				{
					Character* character = m_netNodes[uid]->GetParentCharacter();

					// TODO: take these numbers out of map
					bsIn.ReadFloat16(position.x, -5000.f, +5000.f);
					bsIn.ReadFloat16(position.z, -5000.f, +5000.f);
					bsIn.ReadFloat16(angle, -360.f, +360.f);

					// set isSprinting flag used by the animation system
					if (bsIn.ReadBit()) {
						character->SetSprinting(true);
					}
					else {
						character->SetSprinting(false);
					}

					// set isAiming flag used by the animation system
					if (bsIn.ReadBit()) {
						character->SetAiming(true);
					}
					else {
						character->SetAiming(false);
					}

					// shooting bit
					bool isShooting = false;

					if(bsIn.ReadBit()) {
						isShooting = true;
					}

					position.y = m_netNodes[uid]->GetParent()->GetPosition().y;
					
					if (NetworkModule::GetEnableInterpolation())
					{
						TransformSnapshot snap(serverTime * 0.001, position, angle);

						if (m_netNodes[uid]->GetSnapshots().size() > 0) {

							TransformSnapshot lastSnapshot = *m_netNodes[uid]->GetSnapshots().begin();

							// set isMoving flag used by the animation system 
							if (std::abs(snap.GetPosition().x - lastSnapshot.GetPosition().x) > 1.f ||
								std::abs(snap.GetPosition().z - lastSnapshot.GetPosition().z) > 1.f) {
								character->SetMoving(true);
							}
							else {
								character->SetMoving(false);
							}

							// this value will be used by the animation system
							character->SetMovementDirection(MathHelper::Normalize(snap.GetPosition() - lastSnapshot.GetPosition()));
						}

						m_netNodes[uid]->AddSnapshot(snap);

						// set rotation for not local players
						if (NetworkNode::GetLocalPlayer())
						{
							if (NetworkNode::GetLocalPlayer()->GetUID() != uid)
							{
								m_netNodes[uid]->GetParent()->SetRotation(glm::vec3(0.f, angle, 0.f));
							}
						}
					}
					else
					{
						m_netNodes[uid]->GetParent()->SetPosition(position);
						// set rotation for not local players
						if (NetworkNode::GetLocalPlayer())
						{
							if (NetworkNode::GetLocalPlayer()->GetUID() != uid)
							{
								m_netNodes[uid]->GetParent()->SetRotation(glm::vec3(0.f, angle, 0.f));
							}
						}
					}

					WeaponCS* activeWeapon = character->GetActiveWeapon();

					if (bsIn.ReadBit())
					{
						unsigned short ammo;
						bsIn.Read(ammo);

						// a shot was performed
						if (isShooting) {
							
							if (activeWeapon->IsReloading()) {
								activeWeapon->StopReloading();
							}

							activeWeapon->PlayShootingEffects();
							activeWeapon->PlayShotImpactEffects();

							character->PlayShootAnimation();
						}
						else if (ammo == activeWeapon->GetWeaponStats().Ammo) {

							// reloading finished
							activeWeapon->SetReloading(false);

							if (ammo == activeWeapon->GetWeaponStats().Ammo) {

								activeWeapon->PlayWeaponSound(WeaponSoundTypes::CockSound);
								character->PlayCockAnimation();
							}
						}

						activeWeapon->GetWeaponStats().Ammo = ammo;
						character->SetStatsChanged(true);
					}
					else if (isLocalCharacter && isShooting && activeWeapon->GetWeaponStats().Ammo == 0 && !activeWeapon->IsReloading()) {
						activeWeapon->PlayWeaponSound(WeaponSoundTypes::TriggerClickSound);
					}
				}
				else
				{
					//std::cout << "Received info about unknown player with uid " << (unsigned)uid << " in MerryMessage::ID_MERRY_INGAME_UPDATE packet" << std::endl;
				}
			}
		}
		break;

	case MerryMessage::ID_MERRY_INGAME_HIT:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			unsigned char murdererUID;
			unsigned char victimUID;
			float victimHealth, victimArmour;

			bsIn.IgnoreBytes(1);
			bsIn.Read(murdererUID);
			bsIn.Read(victimUID);
			bsIn.ReadFloat16(victimHealth, 0.0f, 255.0f);
			bsIn.ReadFloat16(victimArmour, 0.0f, 255.0f);

			if (m_netNodes.count(victimUID)) {
				if (NetworkNode::GetLocalPlayer())
				{
					if (murdererUID == NetworkNode::GetLocalPlayer()->GetUID())
					{
						std::cout << "You hit " << m_netNodes[victimUID]->GetName().C_String() << " (" << (unsigned)victimUID << ")" << std::endl;
					}
				}

				Character* hitCharacter = m_netNodes[victimUID]->GetParentCharacter();

				hitCharacter->GetCharacterStats().HP = victimHealth;
				hitCharacter->GetCharacterStats().Armour = victimArmour;
				hitCharacter->PlayTakingDamageEffects();
			}
		}
		break;

	case MerryMessage::ID_MERRY_INGAME_KILLED:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			unsigned char murdererUID;
			unsigned char murdererKillScore;
			unsigned char victimUID;
			unsigned char victimDeathScore;
			unsigned char murdererKillingSpreeVal;

			bsIn.IgnoreBytes(1);
			bsIn.Read(murdererUID);
			bsIn.Read(victimUID);
			bsIn.Read(murdererKillScore);
			bsIn.Read(murdererKillingSpreeVal);
			bsIn.Read(victimDeathScore);

			bool bothExist = true;

			// save new scores for both players
			if (m_netNodes.count(murdererUID))
			{
				m_netNodes[murdererUID]->GetParentCharacter()->SetKills(murdererKillScore);

				UpdateRowMsg mMsg = { murdererUID, m_netNodes[murdererUID]->GetParentCharacter()->GetKills(), m_netNodes[murdererUID]->GetParentCharacter()->GetDeaths() };
				m_guiMgr->GetLayer<ScoreTableGUI>("score_win")->SendUpdateMsg(mMsg);
				
				// display killing spree message
				if (m_guiMgr->GetLayer<GameSceneGUI>("game_scene_gui"))
				{
					m_guiMgr->GetLayer<GameSceneGUI>("game_scene_gui")->SetKillingSpreeLabel(m_netNodes[murdererUID]->GetName(), 
						(KillingSpreeValue)murdererKillingSpreeVal);
				}
			}
			else
			{
				bothExist = false;
			}

			if (m_netNodes.count(victimUID))
			{
				m_netNodes[victimUID]->GetParentCharacter()->SetDeaths(victimDeathScore);

				UpdateRowMsg vMsg = { victimUID, m_netNodes[victimUID]->GetParentCharacter()->GetKills(), m_netNodes[victimUID]->GetParentCharacter()->GetDeaths() };
				m_guiMgr->GetLayer<ScoreTableGUI>("score_win")->SendUpdateMsg(vMsg);

				// call die
				m_netNodes[victimUID]->GetParentCharacter()->Die();
			}
			else
			{
				bothExist = false;
			}

			// make output to console
			if (NetworkNode::GetLocalPlayer() && murdererUID == NetworkNode::GetLocalPlayer()->GetUID())
			{
				std::cout << "You killed " << m_netNodes[victimUID]->GetName().C_String() << " (" << (unsigned)victimUID << ")" << std::endl;
			}
			if (NetworkNode::GetLocalPlayer() && victimUID == NetworkNode::GetLocalPlayer()->GetUID())
			{
				// show score table if local player is dead
				m_guiMgr->GetLayer<ScoreTableGUI>("score_win")->SetPostgame(true);
				m_guiMgr->SendVisMsg(SetVisibilityMsg("score_win", true));

				NetworkNode::GetLocalPlayer()->GetParentCharacter()->GetCharacterStats().HP = 0.0f;
				NetworkNode::GetLocalPlayer()->GetParentCharacter()->GetCharacterStats().Armour = 0.0f;
				
				std::cout << "You were killed by " << m_netNodes[victimUID]->GetName().C_String() << " (" << (unsigned)murdererUID << ")" << std::endl;
			}

			// if we know names of both players diplay a message on the screen
			if (bothExist)
			{
				std::stringstream ss;

				if (m_netNodes[victimUID]->GetParentCharacter()->GetTeamTag() == m_netNodes[murdererUID]->GetParentCharacter()->GetTeamTag())
				{
					ss << m_netNodes[murdererUID]->GetName().C_String() << " just killed his teammate";
				}
				else
				{
					ss << m_netNodes[victimUID]->GetName().C_String() << " was killed by " << m_netNodes[murdererUID]->GetName().C_String();
				}

				if (m_guiMgr->GetLayer<GameSceneGUI>("game_scene_gui"))
				{
					m_guiMgr->GetLayer<GameSceneGUI>("game_scene_gui")->SetNewsLabel(ss.str().c_str());
				}
			}
		}
		break;

	case MerryMessage::ID_MERRY_INGAME_RESPAWNED:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			
			unsigned char uid;
			float hp, stamina, armour;

			bsIn.IgnoreBytes(1);
			bsIn.Read(uid);
			bsIn.ReadFloat16(hp, 0.0f, 255.0f);
			bsIn.ReadFloat16(stamina, 0.0f, 255.0f);
			bsIn.ReadFloat16(armour, 0.0f, 255.0f);

			if (m_netNodes.count(uid))
			{
				// hide score table
				m_guiMgr->GetLayer<ScoreTableGUI>("score_win")->SetPostgame(false);
				m_guiMgr->SendVisMsg(SetVisibilityMsg("score_win", false));

				std::cout << "Player " << m_netNodes[uid]->GetName().C_String() << " (" << (unsigned)uid << ") respawned" << std::endl;

				m_netNodes[uid]->GetParentCharacter()->GetCharacterStats().HP = hp;
				m_netNodes[uid]->GetParentCharacter()->GetCharacterStats().Stamina = stamina;
				m_netNodes[uid]->GetParentCharacter()->GetCharacterStats().Armour = armour;
				m_netNodes[uid]->GetParentCharacter()->Respawn();
				m_netNodes[uid]->GetParentCharacter()->SetStatsChanged(true);
			}
		}
		break;

	case MerryMessage::ID_MERRY_NEW_PLAYER_CONNECTED:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			unsigned char		uid;
			char				teamTag, charClass;
			RakNet::RakString	name;
			glm::vec3			position;

			bsIn.IgnoreBytes(1);
			bsIn.Read(uid);
			bsIn.Read(name);
			bsIn.Read(teamTag);
			bsIn.Read(charClass);
			bsIn.ReadFloat16(position.x, -5000.f, +5000.f);
			bsIn.ReadFloat16(position.y, -5000.f, +5000.f);
			bsIn.ReadFloat16(position.z, -5000.f, +5000.f);

			NetworkNode* local = NetworkNode::GetLocalPlayer();
			if (local && local->GetUID() == uid)
			{
				std::cout << "It was gonna create second copy of local player!" << std::endl;
				break;
			}

			Character* player = CreatePlayer(uid, charClass, teamTag, name);
			if (!player)
			{
				std::cout << "Network: couldn't create player with uid " << (unsigned)uid << " in MerryMessage::ID_MERRY_NEW_PLAYER_CONNECTED" << std::endl;
				break;
			}
			player->SetPosition(position);

			// moved to the top of ProcessMessage
			//if (m_guiMgr->GetLayer<ScoreTableGUI>("score_win"))
			//{
				// add entry to the score table
				AddRowMsg msg = { std::string(name.C_String()), uid, player->GetKills(), player->GetDeaths(), player->GetTeamTag() };
				m_rowsToAdd.push(msg);
				//m_guiMgr->GetLayer<ScoreTableGUI>("score_win")->SendAddMsg(msg);
			//}

			std::cout << "New player has connected with name " << name.C_String() << "(" << (unsigned)uid << ")" << std::endl;

			if (m_guiMgr->GetLayer<GameSceneGUI>("game_scene_gui"))
			{
				// display a message on screen that a new player has connected
				std::stringstream ss;
				ss << "new player (" << name.C_String() << ") has connected";
				m_guiMgr->GetLayer<GameSceneGUI>("game_scene_gui")->SetNewsLabel(ss.str().c_str());
			}
		}	
		break;

	case MerryMessage::ID_MERRY_INGAME_CHANGE_WEAPON:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			unsigned char uid;
			WeaponID wid;

			bsIn.IgnoreBytes(1);
			bsIn.Read(uid);
			bsIn.Read(wid);
			
			if (m_netNodes.count(uid))
			{
				m_netNodes[uid]->GetParentCharacter()->SetActiveWeapon(wid);
			}
		}	
		break;

	case MerryMessage::ID_MERRY_INGAME_RELOAD:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			unsigned char uid;

			bsIn.IgnoreBytes(1);
			bsIn.Read(uid);

			if (m_netNodes.count(uid)) 
			{
				m_netNodes[uid]->GetParentCharacter()->Reload();

			}
		}
		break;

	case MerryMessage::ID_MERRY_INGAME_BASIC_STATS_CHANGED:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			unsigned char uid;
			float newHP, newStamina, newArmour;

			bsIn.IgnoreBytes(1);
			bsIn.Read(uid);
			// read new calues for player's stats
			bsIn.ReadFloat16(newHP, 0.0f, 255.0f);
			bsIn.ReadFloat16(newStamina, 0.0f, 255.0f);
			bsIn.ReadFloat16(newArmour, 0.0f, 255.0f);

			if (m_netNodes.count(uid)) {

				// feed new values to the character
				m_netNodes[uid]->GetParentCharacter()->GetCharacterStats().HP = newHP;
				m_netNodes[uid]->GetParentCharacter()->GetCharacterStats().Stamina = newStamina;
				m_netNodes[uid]->GetParentCharacter()->GetCharacterStats().Armour = newArmour;
				m_netNodes[uid]->GetParentCharacter()->SetStatsChanged(true);
			}
		}
		break;

	case MerryMessage::ID_MERRY_INGAME_BUFF_APPLIED:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			unsigned char uid, buffType;
			float duration;

			bsIn.IgnoreBytes(1);
			bsIn.Read(uid);
			bsIn.Read(buffType);
			bsIn.ReadFloat16(duration, 0.0f, 100000.0f);

			if (m_netNodes.count(uid)) {
		
				m_netNodes[uid]->GetParentCharacter()->ShowEffectApplied((merrymen::EffectType)buffType, duration);
				break;
			}
		}
		break;

	case MerryMessage::ID_MERRY_INGAME_OBJECT_PICKED:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);

			unsigned char uid, objectType;

			bsIn.IgnoreBytes(1);
			bsIn.Read(uid);
			bsIn.Read(objectType);

			Character* picker = m_netNodes[uid]->GetParentCharacter();
			LevelNode* level = static_cast<GameScene*>(Engine::GetRunningScene())->GetLevel();

			// find a proper pickable object on the level
			switch (objectType) {

			case PickableItemType::PickableBuff:
				{
					PickableBuffCS* pickedObject = level->GetNearestPickableObject<PickableBuffCS>(picker->GetPosition());

					// remove the object
					if (pickedObject) {
						level->RemovePickableObject(pickedObject);
					}

					break;
				}

			default:
				break;
			}
		}
		break;

	// not in use right now
	case MerryMessage::ID_MERRY_POSTGAME_ROUND_FINISHED:
		{
			std::cout << "Round finished" << std::endl;

			NetworkNode* local = NetworkNode::GetLocalPlayer();
			if (local)
			{
				local->TurnInput(false);
			}

			if (m_guiMgr->GetLayer<ScoreTableGUI>("score_win"))
			{
				m_guiMgr->GetLayer<ScoreTableGUI>("score_win")->SetPostgame(true);
				m_guiMgr->SendVisMsg(SetVisibilityMsg("score_win", true));
			}
		}
		break;

	// not in use right now
	case MerryMessage::ID_MERRY_POSTGAME_ROUND_RESTARTED:
		{
			std::cout << "New round has started" << std::endl;

			NetworkNode* local = NetworkNode::GetLocalPlayer();
			if (local)
			{
				local->TurnInput(true);
			}

			if (m_guiMgr->GetLayer<ScoreTableGUI>("score_win"))
			{
				m_guiMgr->GetLayer<ScoreTableGUI>("score_win")->SetPostgame(false);
				m_guiMgr->SendVisMsg(SetVisibilityMsg("score_win", false));
			}

			double started;
			unsigned int duration;
			unsigned int roundNumber;

			RakNet::BitStream bsIn(packet->data, packet->length, false);

			bsIn.IgnoreBytes(1);
			bsIn.Read(started);
			bsIn.Read(duration);
			bsIn.Read(roundNumber);

			if (m_guiMgr->GetLayer<RoundInfoGUI>("round_win"))
			{
				m_guiMgr->GetLayer<RoundInfoGUI>("round_win")->SetUpdateTimer(UpdateTimer(started, duration));
			}
		}
		break;

	default:
		break;
	}
}

void TeamDeathMatchMH::OpenPregameSetupWin()
{
	// set game scene
	// NOTE: can call Create meth cause calling it on the logic thread is fine
	Engine::SetScene(MultiplayerGameScene::Create());

	m_guiMgr->SendEnableMsg(SetEnableMsg("lan_menu", false));
	m_guiMgr->SendVisMsg(SetVisibilityMsg("lan_menu", false));

	// create pregame set up window
	m_guiMgr->CreateLayer(new ClassSelectionGUI(std::bind(&TeamDeathMatchMH::SendSetupRequest, this)), "class_select_win");
}

void TeamDeathMatchMH::SendSetupRequest()
{
	RakNet::BitStream bsOut;
	
	const ClassSelectionGUI* selectWin = m_guiMgr->GetLayer<ClassSelectionGUI>("class_select_win");
	//selectWin->StartSpinner();
	m_guiMgr->SendVisMsg(SetVisibilityMsg("class_select_win", false));
	m_guiMgr->SendEnableMsg(SetEnableMsg("class_select_win", false));

	std::string name	= selectWin->GetName();
	char teamTag		= selectWin->GetTeam();
	char charClass		= selectWin->GetCharacterClass();

	// send what team and class a player chose
	bsOut.Write(MerryMessage::ID_MERRY_PREGAME_SETUP_REQUEST);
	bsOut.Write(RakNet::RakString(name.c_str()));
	bsOut.Write(teamTag);
	bsOut.Write(charClass);

	//m_module->Send(bsOut);
	UICommand::AddNetCommand(UICommand(UICommand::UICommandID::CMD_NET_PREGAME_SETUP_REQUEST, bsOut));
}

void TeamDeathMatchMH::SendChangeWeaponRequest(WeaponID wid)
{
	RakNet::BitStream bsOut;

	bsOut.Write(MerryMessage::ID_MERRY_INGAME_CHANGE_WEAPON);
	bsOut.Write(NetworkNode::GetLocalPlayer()->GetUID());
	bsOut.Write(wid);

	m_module->Send(bsOut);
}

void TeamDeathMatchMH::ProcessPlayerDisconnection(unsigned int uid)
{
	if (m_netNodes.count(uid))
	{
		std::cout << "Player " << m_netNodes[uid]->GetName().C_String() << "(" << (unsigned)uid << ") has disconnected" << std::endl;

		Character* character = m_netNodes[uid]->GetParentCharacter();

		RemoveRowMsg rMsg = { uid, m_netNodes[uid]->GetParentCharacter()->GetTeamTag() };
		m_guiMgr->GetLayer<ScoreTableGUI>("score_win")->SendRemoveMsg(rMsg);

		static_cast<GameScene*>(Engine::GetInstance()->GetRunningScene())->RemovePlayer(character);
		delete m_netNodes[uid]->GetParent();
		m_netNodes.erase(uid);
	}
	else
	{
		std::cout << "Trying to remove player with unknown uid " << (unsigned)uid << " in MerryMessage::ID_ENGINE_CLIENT_DISCONNECTED packet" << std::endl;
	}
}

void TeamDeathMatchMH::SendReloadRequest() {

	RakNet::BitStream bsOut;

	bsOut.Write(MerryMessage::ID_MERRY_INGAME_RELOAD);
	bsOut.Write(NetworkNode::GetLocalPlayer()->GetUID());

	m_module->Send(bsOut);
}

Character* TeamDeathMatchMH::CreatePlayer(unsigned char uid, char chClass, char team, const RakNet::RakString& name, bool local)
{
	if (m_netNodes.count(uid))
	{
		std::cout << "watafaq situation" << std::endl;
		return nullptr;
	}

	Character* player = nullptr;

	switch (chClass)
	{
	case CharacterClass::TANK:

		if (team == 1) {
			player = TankCharacter::Create("Models/DonTank.vgsmodel", "Data/Characters.ini", "Tank");
		}
		else if (team == 0) {
			player = TankCharacter::Create("Models/RobynTank.vgsmodel", "Data/Characters.ini", "Tank");
		}

		break;

	case CharacterClass::SNIPER:

		if (team == 1)
		{
			player = SniperCharacter::Create("Models/DonTank.vgsmodel", "Data/Characters.ini", "Sniper");
		}
		else if (team == 0)
		{
			player = SniperCharacter::Create("Models/RobynTank.vgsmodel", "Data/Characters.ini", "Sniper");
		}

		break;

	default:
		break;
	}

	if (!player)
	{
		return nullptr;
	}

	NetworkNode* node = local ? NetworkNode::CreateLocal() : NetworkNode::CreateRemote();
	
	if (node)
	{
		player->AddChild(node);
		player->SetTeamTag(team);

		node->SetParentCharacter(player);
		node->SetUID(uid);
		node->SetName(name);

		// FIXME: should not be related to any certain scene
		GameScene* gs = static_cast<GameScene*>(Engine::GetInstance()->GetRunningScene());
		gs->AddPlayer(player);
		m_netNodes[uid] = node;

		// fixate camera on local player
		if (local)
		{
			FixedCamera* fixedCam = FixedCamera::Create();
			fixedCam->InitWithTarget(player, 1000.0f, 1900.0f);
			Engine::GetInstance()->GetRunningScene()->SetCurrentCamera(fixedCam);

			gs->CreateReticle();

			gs->AddChild( CompassNode::Create() );
		}
		
		return player;
	}
	else
	{
		return nullptr;
	}
}