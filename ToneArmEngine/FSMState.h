/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	9/3/2014 1:53:42 PM
------------------------------------------------------------------------------------------
*/

#ifndef __FSM_STATE_H__
#define __FSM_STATE_H__

#include "RTTI.h"

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	State

	Base class for states of finite state machines.
------------------------------------------------------------------------------------------
*/
class FSMState {

	DECLARE_RTTI;

public:
					FSMState() {};
					~FSMState() {};

	virtual void	OnEnter() = 0;
	virtual void	Execute() = 0;
	virtual void	OnExit() = 0;
};

}

#endif __FSM_STATE_H__