#include "BoxCollider.h"
#include "CylinderCollider.h"

#ifndef MERRY_SERVER
#ifdef TONEARM_DEBUG
#include "MeshHelper.h"
#include "ModelNode.h"
#endif
#endif

namespace vgs {

IMPLEMENT_RTTI(BoxCollider, ColliderComponent);

//
// default constructor
//
BoxCollider::BoxCollider() :
	m_shape(new BoxShape())
{}

//
// constructor that sets collider's attributes to the passed values
//
BoxCollider::BoxCollider(GameObject* const owner, const glm::vec3& position, const glm::vec3& dimensions, bool dynamic) :
	ColliderComponent(owner, dynamic)
{
	m_shape = BoxShape::CreateBoxShape(position, dimensions);

	SetActive(true);

#ifdef TONEARM_DEBUG
#ifndef MERRY_SERVER
	if (GetOwner() && GetOwner()->GetRTTI().DerivesFrom(Node::rtti)) {

		float*			vertexData = NULL;
		unsigned short*	indexData = NULL;

		unsigned int	vertexCount = 0;
		unsigned int	indexCount = 0;
	
		MeshHelper::MakeRectMesh( dimensions.x, dimensions.y, dimensions.z, VertexComponents::POSITION, &vertexData, &indexData, vertexCount, indexCount );

		m_colliderMesh = ModelNode::CreateWithMesh( vertexData, indexData, GL_UNSIGNED_SHORT, vertexCount, indexCount, VertexComponents::POSITION, glm::vec3( 0.0f, 1.0f, 0.0f ), glm::vec3( 0.0f ), dimensions );	
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
BoxCollider::~BoxCollider() {

	delete m_shape;
}

} //namespace vgs