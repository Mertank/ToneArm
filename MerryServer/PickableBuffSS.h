/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/22/2014 1:43:46 PM
------------------------------------------------------------------------------------------
*/

#include "PickableItemSS.h"

#include <Effect.h>

#ifndef __PICKABLE_BUFF_SS_H__
#define __PICKABLE_BUFF_SS_H__

using namespace vgs;

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	PickableBuffSS

	Class that represents pickable buffs on the level (server side version).
------------------------------------------------------------------------------------------
*/
class PickableBuffSS : public PickableItemSS {

	DECLARE_RTTI

public:
					PickableBuffSS(Effect* const buff, const glm::vec3& position, float pickUpRadius, bool active = true);
	virtual			~PickableBuffSS();

	void			HandlePickup(void* const picker) override;

	void			Update(float dt) {};

	const Effect&	GetBuff() const						{ return *m_buff; }
	void			SetBuff(Effect* const buff)			{ m_buff = buff; }

protected:
					PickableBuffSS();
private:
	Effect*			m_buff;
};

}

#endif __PICKABLE_BUFF_SS_H__