/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MultiplayerGameScene.h

		Created by: Vladyslav Dolhopolov
		Created on: 7/31/2014 5:03:15 PM

========================
*/
#ifndef __MULTIPLAYERGAMESCENE_H__
#define __MULTIPLAYERGAMESCENE_H__

#include "../Merrymen/GameScene.h"

namespace vgs
{
	struct CachedResource;
}

namespace merrymen
{
	/*
	========================

		MultiplayerGameScene

			Description.

			Created by: Vladyslav Dolhopolov
			Created on: 7/31/2014 12:01:43 PM

	========================
	*/
	class MultiplayerGameScene : public GameScene
	{
	public:
									CREATE_METH(MultiplayerGameScene)
		virtual bool				Init()								override;
		virtual void				Update(float dt)					override;
		virtual void				EnterScene()						override;
		virtual void				ExitScene()							override;
		virtual Character* const	GetLocalCharacter() const			override;

		void						OnLevelLoaded(vgs::CachedResource* res);
		// test
		static void					OnLevelLoaded_St(vgs::CachedResource* res);

	};

} // namespace vgs

#endif __MULTIPLAYERGAMESCENE_H__