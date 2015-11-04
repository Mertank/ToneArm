/*
------------------------------------------------------------------------------------------



    Author: Mikhail Kutuzov
	Date:	7/18/2014 4:06:44 PM
------------------------------------------------------------------------------------------
*/

#ifndef __PICKABLE_ITEM_H__
#define __PICKABLE_ITEM_H__

#include "GameObject.h"

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	PickableItem

	Base class for items that can be picked up by characters during the gameplay.
------------------------------------------------------------------------------------------
*/

enum class PickableItemType : unsigned char {

	PickableBuff,
	MoneyBag
};

class PickableItem : public GameObject {

	DECLARE_RTTI

public:
	virtual				~PickableItem() {};

	virtual void		HandlePickup(void* const picker = nullptr) = 0;
	virtual void		Update(float dt) = 0;

	// accessors
	float				GetPickUpRadius() const					{ return m_pickUpRadius; }
	void				SetPickUpRadius(const float value)		{ m_pickUpRadius = value; }

	bool				IsActive() const						{ return m_active; }
	virtual void		SetActive(bool active)					{ m_active = active; }

protected:
						PickableItem();
						PickableItem(const glm::vec3& position, float pickUpRadius, bool active);

	float				m_pickUpRadius;
	bool				m_active;
};

}

#endif __PICKABLE_ITEM_H__