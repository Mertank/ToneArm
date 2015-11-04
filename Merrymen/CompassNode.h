/*
========================

Copyright (c) 2014 Vinyl Games Studio

	CompassNode

		A node that points to the direction of the closest player on the other team

		Created by: Karl Merten
		Created on: 8/13/2014 5:30:30 PM

========================
*/

#include "../ToneArmEngine/Node.h"
#include "Character.h"

#include <unordered_map>

namespace vgs {
	class NetworkNode;
	class ModelNode;
}

namespace merrymen {

class CompassNode : public vgs::Node {
public:
	static CompassNode*											Create( void );
	virtual void												Update( float dt );
private:
	bool														Init( void );

	const std::unordered_map<unsigned char, vgs::NetworkNode*>*	m_players;	
	ModelNode*													m_arrow;
	float														m_showTimer;
	glm::vec3													m_prevPosition;

	struct EnemyDistanceSort {

		EnemyDistanceSort( void ) :
			m_local( NULL )
		{}

		bool operator()( Character* lhs, Character* rhs ) {
			return glm::distance( m_local->GetAbsoluteWorldPosition(), lhs->GetAbsoluteWorldPosition() ) < glm::distance( m_local->GetAbsoluteWorldPosition(), rhs->GetAbsoluteWorldPosition() );
		}

		Character*	m_local;		
	} distanceSort;
};

}