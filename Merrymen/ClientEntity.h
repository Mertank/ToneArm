/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/10/2014 10:06:05 AM
------------------------------------------------------------------------------------------
*/

#ifndef __CLIENT_ENTITY_H__
#define __CLIENT_ENTITY_H__

#include "../ToneArmEngine/ModelNode.h"
#include "../ToneArmEngine/ColliderComponent.h"

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	GameEntity

	Class that represents interactive objects, such as characters, drones, etc.
------------------------------------------------------------------------------------------
*/

using namespace vgs;

struct Damage;
enum class ProjectileType : unsigned char;

class ClientEntity : public Node {

public:
	DECLARE_RTTI;

	virtual						~ClientEntity();

	virtual bool				Init(const char* modelFile, bool online = true);
	virtual void				Update(float dt);

	// actions
	virtual void				Move(float dt) = 0;
	virtual void				TakeDamage(const Damage& damage, ProjectileType projectileType, GameObject* shooter = nullptr) = 0;
	virtual void				Die(GameObject* killer = nullptr) = 0;

	// accessors
	ModelNode* const			GetModel()										{ return m_model; }
	void						SetModel(ModelNode* const model)				{ m_model = model;
																				  AddChild(model); }

	bool						IsOnline() const								{ return m_online; }

protected:
								ClientEntity() {}

	ModelNode*					m_model;
	bool						m_online;
};

} // namespace merrymen

#endif __CLIENT_ENTITY_H__