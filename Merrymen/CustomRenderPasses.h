/*
========================

Copyright (c) 2014 Vinyl Games Studio

	CustomRenderPasses

		Header for values for the custom user passes.

		Created by: Karl Merten
		Created on: 01/07/2014

========================
*/

#ifndef __CUSTOMUSERPASSES_H__
#define __CUSTOMUSERPASSES_H__

#include "../ToneArmEngine/RenderPass.h"

namespace merrymen {

namespace CustomRenderPasses {
	enum Value {
		FOG_OF_WAR_PASS = ( vgs::PassType::USER_DEFINED_PASS * 2 )
	};
}

}
#endif //__CUSTOMUSERPASSES_H__
