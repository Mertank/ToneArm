/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	10/7/2014 10:51:55 AM
------------------------------------------------------------------------------------------
*/

#ifndef __PICKABLE_ITEM_SS_H__
#define __PICKABLE_ITEM_SS_H__

#include <PickableItem.h>

namespace vgs {
class Level;
}

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	PickableItemCS

	Server-side represantation of pickable in-game object.
------------------------------------------------------------------------------------------
*/
class PickableItemSS : public vgs::PickableItem {

	DECLARE_RTTI

public:
	virtual					~PickableItemSS() {};

	virtual void			HandlePickup(void* const picker);
	virtual void			Update(float dt) {};

	// accessors
	vgs::Level* const		GetLevel() const						{ return m_level; }
	void					SetLevel(vgs::Level* const level)		{ m_level = level; }

protected:
							PickableItemSS() {};
							PickableItemSS(const glm::vec3& position, float pickUpRadius, bool active = true);

	vgs::Level*				m_level;
};

}

#endif __PICKABLE_ITEM_SS_H__