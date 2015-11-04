/*
========================

Copyright (c) 2014 Vinyl Games Studio

	NetworkNode.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/23/2014 11:51:58 AM

========================
*/
#ifndef __NETWORKNODE_H__
#define __NETWORKNODE_H__

#include "Node.h"

#include <RakString.h>

#include <list>

namespace merrymen
{
	class Character;
}

namespace vgs
{
	class InputModule;

	/*
	========================

		TransformSnapshot

			Timestamped transform of a network node.

			Created by: Vladyslav Dolhopolov
			Created on: 7/8/2014 11:51:58 AM

	========================
	*/
	struct TransformSnapshot
	{
		TransformSnapshot()
			: t(0)
			, p(0)
			, r(0)
		{
		}

		TransformSnapshot(double time, const glm::vec3& pos, float rot)
			: t(time)
			, p(pos)
			, r(rot)
		{
		}

		const glm::vec3&	GetPosition() const					{ return p; }
		void				SetPosition(const glm::vec3& value) { p = value; }

		double		t;
		glm::vec3	p;
		float		r;
	};

	/*
	========================

		NetworkNode

			Stores info needed in multiplayer game
			as well as takes care of correcting node's position.

			Created by: Vladyslav Dolhopolov
			Created on: 5/23/2014 11:51:58 AM

	========================
	*/
	class NetworkNode : public Node
	{
		enum class NetworkRole : unsigned char
		{
			NOT_ASSIGNED,
			LOCAL,
			REMOTE,
		};

		typedef std::list<TransformSnapshot> SnapshotList;
		typedef SnapshotList::const_iterator SnapshotIterator;

	public:								
		static NetworkNode*				CreateLocal();
		static NetworkNode*				CreateRemote();
										
		virtual							~NetworkNode()									override;


		virtual bool					Init()											override;
		virtual void					Update(float dt)								override;

		void							SetUID(unsigned char val)						{ m_uid = val; }
		unsigned char					GetUID() const									{ return m_uid; }

		// TODO: let server	know each time player changes their name
		void							SetName(const RakNet::RakString& val)			{ m_playerName = val; }
		const RakNet::RakString&		GetName() const									{ return m_playerName; }

		void							SetRole(NetworkRole val)						{ m_role = val; }
		NetworkRole						GetRole() const									{ return m_role; }

		void							SetParentCharacter(merrymen::Character* val)	{ m_character = val; }
		merrymen::Character*			GetParentCharacter() const						{ return m_character; }

		void							TurnInput(bool on = true)						{ m_allowInput = on; }

		static NetworkNode*	const		GetLocalPlayer()								{ return m_local; }

		//virtual void					SetPosition		(const glm::vec3& pos) {};

		void							AddSnapshot(const TransformSnapshot& snap)		{ m_snapshots.push_front(snap); }
		const SnapshotList				GetSnapshots()									{ return m_snapshots; }

	private:
										NetworkNode();
		void							InterpolateTransform();

	private:
		static NetworkNode*				m_local;	

		merrymen::Character*			m_character;

		unsigned char					m_uid;
		RakNet::RakString				m_playerName;
		NetworkRole						m_role;

		// for local player
		bool							m_allowInput;

		SnapshotList					m_snapshots;
	};

} // namespace vgs

#endif __NETWORKNODE_H__