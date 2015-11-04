/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MainMenuScene.h

		Created by: Vladyslav Dolhopolov
		Created on: 7/30/2014 4:58:02 PM

========================
*/
#ifndef __MAINMENUSCENE_H__
#define __MAINMENUSCENE_H__

#include "../ToneArmEngine/GUIManager.h"
#include "../ToneArmEngine/Scene.h"

using namespace vgs;

namespace merrymen
{
	/*
	========================

		MainMenuScene

			Basicaly storage for main menu gui windows.

			Created by: Vladyslav Dolhopolov
			Created on: 7/30/2014 4:58:02 PM

	========================
	*/
	class MainMenuScene : public vgs::Scene
	{
	public:
						CREATE_METH(MainMenuScene);

						MainMenuScene();

		virtual bool	Init()						override;
		virtual void	Update(float dt)			override;

		virtual void	EnterScene()				override;
		virtual void	ExitScene()					override;
	};

} // namespace vgs

#endif __MAINMENUSCENE_H__