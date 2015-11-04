/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/21/2014 2:15:53 PM
------------------------------------------------------------------------------------------
*/

#ifndef __PICKABLE_BUFF_CS_H__
#define __PICKABLE_BUFF_CS_H__

#include "PickableItemCS.h"

#include <Effect.h>

using namespace vgs;

namespace merrymen
{

class Character;

/*
------------------------------------------------------------------------------------------
	PickableBuff

	Class that represents pickable buffs on the level (client side version).
------------------------------------------------------------------------------------------
*/

class PickableBuffCS : public PickableItemCS {

	DECLARE_RTTI

public:
									PickableBuffCS(const char* modelFile, Effect* const buff, const glm::vec3& position, float pickUpRadius, bool active);
	virtual							~PickableBuffCS();

	void							HandlePickup(void* const picker) override;
	void							Update(float dt);
	// accessors

	const Effect&					GetBuff() const						{ return *m_buff; }
	void							SetBuff(Effect* const buff)			{ m_buff = buff; }

protected:
									PickableBuffCS();
	
private:
	Effect*							m_buff;
};

}

#endif __PICKABLE_BUFF_CS_H__