/*
========================

Copyright (c) 2014 Vinyl Games Studio

	NetworkNode.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 5/23/2014 11:51:58 AM

========================
*/
#include "NetworkNode.h"
#include "InputModule.h"
#include "Engine.h"
#include "NetworkModule.h"

#include "OpenGLRenderModule.h"
#include "MathHelper.h"
#include "Scene.h"
#include "Camera.h"
#include "RunningInfo.h"
#include "VGSAssert.h"

#include <WeaponStats.h>

#include "../Merrymen/Character.h"
#include "../Merrymen/TeamDeathMatchMH.h"

#include <BitStream.h>
#include <RakSleep.h>
#include <GetTime.h>

#include <iostream>
#include <iomanip>

#include <SFML\Window.hpp>
#include <SFML\Window\Keyboard.hpp>
#include <SFML\Window\Mouse.hpp>

using namespace vgs;
using namespace merrymen;

NetworkNode* NetworkNode::m_local = nullptr;

NetworkNode::NetworkNode()
	: m_playerName("dude")
	, m_role(NetworkRole::NOT_ASSIGNED)
	, m_uid(0)
	, m_allowInput(false)
	, m_character(nullptr)
{
}

NetworkNode::~NetworkNode()
{
	if (m_role == NetworkRole::LOCAL)
	{
		Engine::GetInstance()->GetModuleByType<NetworkModule>(EngineModuleType::NETWORKER)->GetMsgHandler()->SendDiconnectMessage(m_uid, 0);
		// the power of earth
		RakSleep(150);
		m_local = nullptr;
	}
}

NetworkNode* NetworkNode::CreateLocal()
{
	VGSAssert(m_local == nullptr, "Only one local player can be created");

	NetworkNode* node = new NetworkNode();									
	if (node && node->Init())											
	{	
		node->SetRole(NetworkRole::LOCAL);

		m_local = node;

		return node;													
	}																	
	delete node;														
	return nullptr;		
}

NetworkNode* NetworkNode::CreateRemote()
{
	NetworkNode* node = new NetworkNode();									
	if (node && node->Init())											
	{	
		node->SetRole(NetworkRole::REMOTE);
		return node;													
	}																	
	delete node;														
	return nullptr;		
}

bool NetworkNode::Init()
{
	return true;
}

void NetworkNode::Update(float dt)
{
	if (NetworkModule::GetEnableInterpolation())
	{
		InterpolateTransform();
	}

	// save and send input only for local player
	if (m_role == NetworkRole::LOCAL && m_allowInput)
	{
		using sf::Keyboard;

		TeamDeathMatchMH* mh = static_cast<TeamDeathMatchMH*>(Engine::GetInstance()->
			GetModuleByType<NetworkModule>(EngineModuleType::NETWORKER)->GetMsgHandler());

		// weapon change request
		static bool num1Pressed = false;
		static bool num2Pressed = false;

		if (Engine::GetInstance()->GetModuleByType<InputModule>(EngineModuleType::INPUTER)->IsKeyPressed(InputModule::ControlElement::PrimaryWeapon) && !num1Pressed)
		{
			num1Pressed = true;
			mh->SendChangeWeaponRequest(GetParentCharacter()->GetWeapons().begin()->first);
		}
		else if (!Engine::GetInstance()->GetModuleByType<InputModule>(EngineModuleType::INPUTER)->IsKeyPressed(InputModule::ControlElement::PrimaryWeapon))
		{
			num1Pressed = false;
		}

		if (Engine::GetInstance()->GetModuleByType<InputModule>(EngineModuleType::INPUTER)->IsKeyPressed(InputModule::ControlElement::SecondaryWeapon) && !num2Pressed)
		{
			num2Pressed = true;
			std::unordered_map<WeaponID, WeaponCS*>::iterator weaponItr = GetParentCharacter()->GetWeapons().begin();
			weaponItr++;
			mh->SendChangeWeaponRequest(weaponItr->first);
		}
		else if (!Engine::GetInstance()->GetModuleByType<InputModule>(EngineModuleType::INPUTER)->IsKeyPressed(InputModule::ControlElement::SecondaryWeapon))
		{
			num2Pressed = false;
		}

		// reload request
		if (Engine::GetInstance()->GetModuleByType<InputModule>(EngineModuleType::INPUTER)->IsKeyPressed(InputModule::ControlElement::Reload)) {
			mh->SendReloadRequest();
		}

		if (GetParentCharacter() && !GetParentCharacter()->IsDead()) {
			// rotation
			glm::mat4x4 camProjMatrix		= Engine::GetRunningScene()->GetCurrentCamera()->GetProjectionMatrix();
			glm::mat4x4 camViewMatrix		= Engine::GetRunningScene()->GetCurrentCamera()->GetViewMatrix();
			glm::vec4 position				= glm::vec4( GetAbsoluteWorldPosition(), 1.0f );
			glm::vec2 screenSize			= glm::vec2(Engine::GetInstance()->GetRunningInfo()->contextWidth, Engine::GetInstance()->GetRunningInfo()->contextHeight);

			// project the character onto the screen
			glm::vec2 positionOnScreen		= MathHelper::ProjectPointOntoScreen( position );

			// get mouse coordinates
			glm::vec2 mousePosition			= glm::vec2((float)sf::Mouse::getPosition().x, (float)sf::Mouse::getPosition().y);
	
			OpenGLRenderModule* rm			= Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER);
			if (rm)
			{
				// find the position of the mouse inside of the window
				glm::vec2 windowPosition	= glm::vec2((float)rm->GetWindow()->getPosition().x, (float)rm->GetWindow()->getPosition().y);
				mousePosition				-= windowPosition;

				// calculate a vector from the character to the mouse position
				glm::vec2 toMouse			= mousePosition - positionOnScreen;
				glm::vec2 intialFwd			= glm::vec2(0.0f, 1.0f);
				float angle					= MathHelper::AngleBetweenVectors(toMouse, intialFwd);
			
				//float d = glm::length(toMouse);

				if (toMouse.x < 0.0f)
				{
					angle = -angle;
				}

				Engine::GetInstance()->GetModuleByType<InputModule>(EngineModuleType::INPUTER)->GetState().angle = angle;
				GetParentCharacter()->SetRotation(glm::vec3(0.f, angle, 0.f));
			}
		}

		if (Engine::GetInstance()->GetModuleByType<InputModule>(EngineModuleType::INPUTER)->IsDirty())
		{
			RakNet::BitStream bs;

			Engine::GetInstance()->GetModuleByType<InputModule>(EngineModuleType::INPUTER)->PackIntoBS(bs);
			mh->SendInputPacket(this, bs);
		}
	}

	//GetParentCharacter()->Update(dt);
	Node::Update(dt);
}

void NetworkNode::InterpolateTransform()
{
	double clientTime = RakNet::GetTime() * 0.001 - NetworkModule::GetClientDelay();

	SnapshotIterator snap = m_snapshots.begin();
    SnapshotIterator nearestFuture = snap;
	glm::vec3 position;

    if (snap->t <= clientTime)
	{
        // there are no snapshots in the future, so use the most recent one from the past
		position = snap->p;
    } 
	else 
	{
        // skip over the future snapshots until we find the one nearest to the current client time
        for (++snap; snap != m_snapshots.end(); ++snap) 
		{
            if (snap->t > clientTime) 
			{
                // still in the future
                nearestFuture = snap;
            }
			else
			{
                // no longer in the future; the current snapshot becomes the most recent one from the past
                SnapshotIterator nearestPast = snap;

                // compute the length of the interpolation window
                double dt = nearestFuture->t - nearestPast->t;

                // compute the interpolation coefficient (alpha, between 0 and 1)
				float alpha = (float)((clientTime - nearestPast->t) / dt);

                // compute the interpolated position by blending the past and future positions
                position = (alpha) * nearestFuture->p + (1 - alpha) * nearestPast->p;
				
				GetParentCharacter()->SetPosition(position);
                // break out of the loop
                break;
            }
        }

        // the loop may have terminated for one of two reasons:
        //   1 - we've reached the end of the snapshot list before finding any snapshot from the past
        //   2 - we successfully computed an interpolated position
		if (snap == m_snapshots.end()) 
		{
            // there are no snapshots in the past, so use the least recent one from the future
            position = nearestFuture->p;
        }
		else
		{
            // blow away all the old snapshots from the past
            m_snapshots.erase(++snap, m_snapshots.end());
        }
    }
}