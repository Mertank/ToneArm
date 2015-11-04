/*
========================

Copyright (c) 2014 Vinyl Games Studio

	SinglePlayerGameScene.h

		Created by: Vladyslav Dolhopolov
		Created on: 7/31/2014 12:01:43 PM

========================
*/
#ifndef __SINGLEPLAYERGAMESCENE_H__
#define __SINGLEPLAYERGAMESCENE_H__

#include "../Merrymen/GameScene.h"

namespace merrymen
{
	/*
	========================

		SinglePlayerGameScene

			Description.

			Created by: Vladyslav Dolhopolov
			Created on: 7/31/2014 12:01:43 PM

	========================
	*/
	class SinglePlayerGameScene : public GameScene
	{
	public:
									DECLARE_RTTI;							

									CREATE_METH(SinglePlayerGameScene)

									SinglePlayerGameScene();

		virtual bool				Init()								override;
		virtual void				EnterScene()						override;
		virtual void				ExitScene()							override;
		virtual void				Update(float dt)					override;
		virtual Character* const	GetLocalCharacter() const			override;
	};

} // namespace vgs

#endif __SINGLEPLAYERGAMESCENE_H__