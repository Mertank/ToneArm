/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	10/6/2014 4:38:12 PM
------------------------------------------------------------------------------------------
*/

#ifndef __PICKABLE_ITEM_CS_H__
#define __PICKABLE_ITEM_CS_H__

#include <PickableItem.h>

namespace vgs {
class ModelNode;
class LevelNode;
}

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	PickableItemCS

	Client-side represantation of pickable in-game object.
------------------------------------------------------------------------------------------
*/
class PickableItemCS : public vgs::PickableItem {

	DECLARE_RTTI;

public:
	virtual							~PickableItemCS();

	virtual void					HandlePickup(void* const picker = nullptr);
	virtual void					Update(float dt) {};

	// accessors
	vgs::ModelNode* const			GetModel() const								{ return m_model; }
	void							SetModel(vgs::ModelNode* const model)			{ m_model = model; }

	vgs::LevelNode* const			GetLevel() const								{ return m_level; }
	void							SetLevel(vgs::LevelNode* const level)			{ m_level = level; }

	void							SetActive(bool active) override;

protected:
									PickableItemCS();
									PickableItemCS(const char* modelFile, const glm::vec3& position, float pickUpRadius, bool active);

	vgs::ModelNode*					m_model;
	vgs::LevelNode*					m_level;
};

}

#endif __PICKABLE_ITEM_CS_H__