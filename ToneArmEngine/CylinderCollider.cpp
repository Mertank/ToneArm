/*
----------------------------------------------------------------------------------------
	


    Author: Mikhail Kutuzov
	Date:	5/26/2014 5:08:41 PM
----------------------------------------------------------------------------------------
*/

#include "CylinderCollider.h"
#include "MeshHelper.h"
#include "ModelNode.h"

#ifndef MERRY_SERVER
#ifdef TONEARM_DEBUG
#include "MeshHelper.h"
#include "ModelNode.h"
#endif
#endif

using namespace vgs;

IMPLEMENT_RTTI(CylinderCollider, ColliderComponent);

//
// default constructor
//
CylinderCollider::CylinderCollider() :
	m_shape(new CylinderShape())
{}

//
// constructor that sets collider's attributes to the passed values
//
CylinderCollider::CylinderCollider(GameObject* const owner, const glm::vec3& position, float radius, float height, bool dynamic) :
	ColliderComponent(owner, dynamic)
{
	m_shape = CylinderShape::CreateCylinderShape(position, radius, height);

	SetActive(true);

#ifndef MERRY_SERVER
#ifdef TONEARM_DEBUG
	if (GetOwner() && GetOwner()->GetRTTI().DerivesFrom(Node::rtti)) {

		float*			vertexData = NULL;
		unsigned short*	indexData = NULL;

		unsigned int	vertexCount = 0;
		unsigned int	indexCount = 0;
	
		MeshHelper::MakeCylinderMesh( m_shape->GetRadius(), m_shape->GetHeight(), 32, VertexComponents::POSITION, &vertexData, &indexData, vertexCount, indexCount );

		m_colliderMesh = ModelNode::CreateWithMesh( vertexData, indexData, GL_UNSIGNED_SHORT, vertexCount, indexCount, VertexComponents::POSITION, glm::vec3( 0.0f, 1.0f, 0.0f ), glm::vec3( 0.0f ), m_shape->GetBoundingDimensions() );	
		m_colliderMesh->SetOutlineOnly( true );
		m_colliderMesh->SetLighting( false );
	
		static_cast<Node*>(GetOwner())->AddChild( m_colliderMesh );

		m_colliderMesh->SetPosition(position);
		m_colliderMesh->SetRotation(this->GetRotation());
	}
#endif
#endif
}

//
// destructor
//
CylinderCollider::~CylinderCollider() {

	delete m_shape;
}