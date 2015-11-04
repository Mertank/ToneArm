/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	CompassNode

	Created by: Karl Merten
	Created on: 8/13/2014 5:30:30 PM

========================
*/
#include "CompassNode.h"

#include "../ToneArmEngine/ModelNode.h"
#include "../ToneArmEngine/NetworkModule.h"
#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/NetworkNode.h"
#include "TeamDeathMatchMH.h"

#define HIDE_THRESHOLD	1000.0f

namespace merrymen {

CompassNode* CompassNode::Create( void ) {
	CompassNode* ptr = new CompassNode();
	if ( ptr && ptr->Init() ) {
		return ptr;
	}

	delete ptr;
	return NULL;
}

bool CompassNode::Init( void ) {
	m_arrow = vgs::ModelNode::Create( "Models/Arrow.vgsModel" );
	m_arrow->SetVisible( false );
	AddChild( m_arrow );
	m_players = vgs::Engine::GetInstance()->GetModuleByType<vgs::NetworkModule>( vgs::EngineModuleType::NETWORKER )->GetMsgHandler()->GetNetworkNodeMap();

	m_showTimer = 0.0f;

	return true;
}

void CompassNode::Update( float dt ) {
	std::vector<Character*> enemies;
	distanceSort.m_local = vgs::NetworkNode::GetLocalPlayer()->GetParentCharacter();

	if ( distanceSort.m_local->IsAiming() ) {
		m_showTimer = 0.0f;
		m_arrow->SetVisible( false );
	}

	glm::vec3 curPosition = distanceSort.m_local->GetAbsoluteWorldPosition();
	if ( glm::distance( curPosition, m_prevPosition ) > 0.1f ) {
		m_arrow->SetVisible( false );
		m_showTimer = 0.0f;
		m_prevPosition = curPosition;
		return;
	}

	m_prevPosition = curPosition;

	if ( m_showTimer > 1.0f ) {
		for ( vgs::MessageHandler::NetworkNodeMap::const_iterator iter = m_players->begin(); iter != m_players->end(); ++iter ) {
			if ( distanceSort.m_local->GetTeamTag() != iter->second->GetParentCharacter()->GetTeamTag() ) {
				enemies.push_back( iter->second->GetParentCharacter() );
			}
		}

		if ( enemies.size() == 0 ) {
			m_arrow->SetVisible( false );
			return;
		}

		std::sort( enemies.begin(), enemies.end(), distanceSort );

		SetPosition( distanceSort.m_local->GetAbsoluteWorldPosition() );

		float closest = glm::distance( ( *enemies.begin() )->GetAbsoluteWorldPosition(), distanceSort.m_local->GetAbsoluteWorldPosition() );
		if ( closest > HIDE_THRESHOLD ) {
			m_arrow->SetVisible();

			glm::vec3 toPlayer = glm::normalize( ( *enemies.begin() )->GetAbsoluteWorldPosition() - distanceSort.m_local->GetAbsoluteWorldPosition() );
			glm::vec3 up = glm::vec3( 0.0f, 1.0f, 0.0f );
			m_transform.SetLocalAxes( toPlayer, glm::cross( up, toPlayer ), up );
		} else {
			m_arrow->SetVisible( false );
		}
	} else {
		m_showTimer += dt;
	}

	Node::Update( dt );
}

}