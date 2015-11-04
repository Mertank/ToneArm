/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/19/2014 4:30:15 PM
------------------------------------------------------------------------------------------
*/

#ifndef __TANK_CHARACTER_H__
#define __TANK_CHARACTER_H__

#include "Character.h"
#include "MerrymenMacro.h"

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	TankCharacter

	Class that represents Tank characters.
------------------------------------------------------------------------------------------
*/

class TankCharacter : public Character {
	DECLARE_CHARACTER_CLASS(TankCharacter);
};

} // namespace merrymen

#endif __TANK_CHARACTER_H__