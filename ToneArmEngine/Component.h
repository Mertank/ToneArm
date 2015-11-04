/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/3/2014 2:39:05 PM
------------------------------------------------------------------------------------------
*/

#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "VGSMacro.h"
#include "RTTI.h"

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	Component

	Base class for components
------------------------------------------------------------------------------------------
*/

class GameObject;

class Component {
						DECLARE_RTTI;
public:
	
	virtual				~Component();

	virtual void		Update(float dt) = 0;

	GameObject*			GetOwner() const					{ return m_owner; }
	void				SetOwner(GameObject* const owner)	{ m_owner = owner; }

	bool				IsActive() const					{ return m_active; }
	virtual void		SetActive(bool active)				{ m_active = active; }

protected:
						Component();
						Component(GameObject* const owner);

	bool				Init(GameObject* const owner);

private:
	GameObject*			m_owner;

	bool				m_active;
};

}

#endif __COMPONENT_H__