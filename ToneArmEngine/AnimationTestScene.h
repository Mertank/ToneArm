/*
========================

Copyright (c) 2014 Vinyl Games Studio

	AnimationTestScene

		Just testing animations.

		Created by: Karl Merten
		Created on: 06/08/2014

========================
*/

#include "../Merrymen/GameScene.h"
#include "DebugConsoleModule.h"

namespace merrymen {
	class Character;
}

namespace vgs {

class AnimationTestScene : public Scene {
										CREATE_METH(AnimationTestScene)
										DECLARE_DEBUG_COMMAND_CALLBACK( vgs::AnimationTestScene, playAnim );
										DECLARE_DEBUG_COMMAND_CALLBACK( vgs::AnimationTestScene, reloadModel );
public:
										AnimationTestScene();

	virtual bool						Init()								override;
	virtual void						EnterScene()						override;
	virtual void						ExitScene()							override;
	virtual void						Update(float dt)					override;
private:
	static ModelNode*					m_characterToAnimate;
	static unsigned int					m_ticks;
	static bool							m_reloadModel;
};

}