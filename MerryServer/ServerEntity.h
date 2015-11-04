/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ServerEntity.h

		Created by: Vladyslav Dolhopolov
		Created on: 5/26/2014 12:51:51 PM

========================
*/
#ifndef __SERVERENTITY_H__
#define __SERVERENTITY_H__

#include <GameObject.h>

namespace merrymen {
struct Damage;
enum class ProjectileType : unsigned char;
}

namespace vgs
{
	/*
	========================

		ServerEntity

			Base entity class. Has User ID.

			Created by: Vladyslav Dolhopolov
			Created on: 5/26/2014 12:51:51 PM

	========================
	*/
	
	class ServerEntity : public GameObject
	{
		DECLARE_RTTI;
	public:
		virtual			~ServerEntity()					{}

		virtual void	Update(float dt);

		// actions
		virtual void	Move(float dt) = 0;
		virtual void	TakeDamage(const merrymen::Damage& damage, merrymen::ProjectileType projectileType, GameObject* shooter = nullptr) = 0;
		virtual void	Die(GameObject* killer = nullptr) = 0;

		void			SetOwnerUID(unsigned char val)	{ m_ownerUid = val; }
		unsigned char	GetOwnerUID() const				{ return m_ownerUid; }

	protected:
		unsigned char	m_ownerUid;
	};

} // namespace vgs

#endif __SERVERENTITY_H__