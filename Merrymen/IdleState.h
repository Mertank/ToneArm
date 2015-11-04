/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/3/2014 4:08:26 PM
------------------------------------------------------------------------------------------
*/

#ifndef __IDLE_STATE_H__
#define __IDLE_STATE_H__

#include "../ToneArmEngine/FSMState.h"

using namespace vgs;

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	IdleState

	Class that represents idle state of animation fsm.
------------------------------------------------------------------------------------------
*/
class IdleState : public FSMState {

public:
			IdleState() {}
			~IdleState() {}

	void	OnEnter() override {}
	void	Execute() override;
	void	OnExit() override {}
};

}

#endif __IDLE_STATE_H__