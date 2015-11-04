/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Node.h

		Created by: Vladyslav Dolhopolov
		Created on: 4/28/2014 2:00:30 PM

========================
*/
#ifndef __NODE_H__
#define __NODE_H__

#include <list>

#include "Transform.h"
#include "VGSMacro.h"
#include "RTTI.h"

#include <GameObject.h>
#include <vector>

namespace vgs
{
	class ModelNode;
	/*
	========================

		Node

			Base node class

			Created by: Vladyslav Dolhopolov
			Created on: 4/28/2014 2:00:30 PM

	========================
	*/
	class Node : public GameObject
	{
										DECLARE_RTTI
	public:
										Node();
		virtual							~Node();

		CREATE_METH(Node);

		virtual bool					Init();

		// each child in update should call base node's update in order to update its children
		virtual void					Update(float dt);

		virtual void					AddChild(Node* child);
		virtual void					RemoveChild(Node* child);
		virtual void					RemoveChild(const unsigned int id);
		virtual void					RemoveFromParent();

		Node*							GetParent() const								{ return m_parent; }
		Node*							GetChild(const unsigned int id) const;
		template <typename T>
		T*								GetChildOfType( void );

		template<class T>
		std::vector<T*>					GetAllChildrenOfType();

		unsigned int					GetID() const									{ return m_id; }

		const glm::mat4&				GetGlobalModelMatrix();
		const glm::vec3&				GetAbsoluteWorldPosition();
		
		const bool						IsTransformChanged() const override;

	private:
		void							SetParent(Node* node)							{ if (node) { m_parent = node; } }

	protected:
		Node*							m_parent;
		std::list<Node*>				m_children;

		static unsigned int				sm_idCounter;
		const unsigned int				m_id;
		
		glm::vec3						m_worldPosition;
		glm::mat4						m_combinedModelMatrix;
	};

	template <typename T>
	T* Node::GetChildOfType( void ) {
		for ( std::list<Node*>::iterator iter = m_children.begin();
			  iter != m_children.end(); ++iter ) {
			if ( ( *iter )->GetRTTI() == T::rtti ) {
				return ( T* )( *iter );
			}
		}

		return NULL;
	}

} // namespace vgs

#endif __NODE_H__