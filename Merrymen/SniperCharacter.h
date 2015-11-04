/*
----------------------------------------------------------------------------------------
	


    Author: Mikhail Kutuzov
	Date:	7/16/2014 3:14:02 PM
----------------------------------------------------------------------------------------
*/

#ifndef __SNIPER_CHARACTER_H__
#define __SNIPER_CHARACTER_H__

#include "Character.h"
#include "MerrymenMacro.h"

namespace merrymen
{
/*
----------------------------------------------------------------------------------------
	SniperCharacter

	Class that represents Sniper characters.
----------------------------------------------------------------------------------------
*/
class SniperCharacter : public Character {
	DECLARE_CHARACTER_CLASS(SniperCharacter);

	void				SetAiming(const bool aiming) override;
	virtual void		SetActiveWeapon(WeaponID wid);

};

}

#endif __SNIPER_CHARACTER_H__