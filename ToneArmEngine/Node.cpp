/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Node.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 4/28/2014 2:00:30 PM

========================
*/
#include "Node.h"
#include "ModelNode.h"
#include "Component.h"

using namespace vgs;

unsigned int Node::sm_idCounter = 1;

IMPLEMENT_RTTI(Node, GameObject);

Node::Node()
	: m_parent(nullptr)
	, m_children(0)
	, m_id(sm_idCounter++)
{
}

Node::~Node()
{
	for (auto child : m_children)
	{
		delete child;
	}

	m_children.clear();
}

bool Node::Init( void ) {
	return true;
}

void Node::Update(float dt)
{
	for (auto child : m_children)
	{
		child->Update(dt);
	}

	GameObject::Update(dt);
}

void Node::AddChild(Node* child)
{
	if (child && !child->GetParent())
	{
		child->SetParent(this);
		// fuck me
		m_children.push_front(child);
	}
}

Node* Node::GetChild(const unsigned int id) const
{
	if (id < 0)
	{
		return nullptr;
	}

	for (auto child : m_children)
	{
		if (child->GetID() == id)
		{
			return child;
		}
	}
	return nullptr;
}

void Node::RemoveChild(Node* child)
{
	if (child)
	{
		m_children.remove(child);
	}
}

void Node::RemoveChild(const unsigned int id)
{
	if (id < 0)
	{
		return;
	}

	for (auto child : m_children)
	{
		if (child->GetID() == id)
		{
			m_children.remove(child);
			break;
		}
	}
}

void Node::RemoveFromParent()
{
	m_parent->RemoveChild(this);
	m_parent = nullptr;
}

const glm::mat4& Node::GetGlobalModelMatrix()
{
	/*if ( m_transformChanged ) {
		m_combinedModelMatrix = GetLocalModelMatrix();
	}

	Node* parent = GetParent();

	if ( parent ) {
		m_combinedModelMatrix = parent->GetGlobalModelMatrix() * m_combinedModelMatrix;
	}

	return m_combinedModelMatrix;*/
	m_combinedModelMatrix = GetLocalModelMatrix();
 
	Node* parent = GetParent();
	while ( parent && !parent->GetRTTI().DerivesFrom( RenderableNode::rtti ) ) {
		m_combinedModelMatrix = parent->GetLocalModelMatrix() * m_combinedModelMatrix;
		parent = parent->GetParent();
	}
 
	return m_combinedModelMatrix;
}

const glm::vec3& Node::GetAbsoluteWorldPosition() {

	Node* parent = GetParent();

	if ( m_transformChanged ) {
		glm::mat4 global = GetGlobalModelMatrix();
		m_worldPosition = glm::vec3(global[3][0], global[3][1], global[3][2]);
	}

	if ( parent ) {
		m_worldPosition = GetPosition() + parent->GetAbsoluteWorldPosition();
	} 

	return m_worldPosition;
}

const bool Node::IsTransformChanged( void ) const {
	if ( m_parent ) {
		if ( !m_parent->GetRTTI().DerivesFrom( RenderableNode::rtti ) ) {
			return m_transformChanged || m_parent->IsTransformChanged();
		} else {
			return m_transformChanged;
		}
	} else {
		return m_transformChanged;
	}
}