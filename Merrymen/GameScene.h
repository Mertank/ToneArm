/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/16/2014 12:31:56 PM
------------------------------------------------------------------------------------------
*/

#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "../ToneArmEngine/Scene.h"
#include "../ToneArmEngine/FreeLookCamera.h"
#include "../ToneArmEngine/FixedCamera.h"

#include "GameSceneGUI.h"
#include "Character.h"

using namespace vgs;

namespace vgs
{
class LevelNode;
}

namespace merrymen
{

class TeamDeathMatchMH;
class ScoreTableGUI;


/*
------------------------------------------------------------------------------------------
	GameScene

	A class for the main in-game scene.
------------------------------------------------------------------------------------------
*/
class GameScene : public Scene {

public:
										DECLARE_RTTI;

										~GameScene();

	//bool								Init() override;
	void								Update(float dt) override;

	virtual void						EnterScene( void );
	virtual void						ExitScene( void );

	void								InitGUI();
	void								UpdateGUI();

	// accessors
	const std::vector<Character*>&		GetPlayers() const								{ return m_players; }
	void								AddPlayer(Character* player)					{ m_players.push_back(player);
																						  AddChild(player); }

	void								RemovePlayer(Character* player);
																						
	LevelNode* const					GetLevel() const								{ return m_level; }
	void								SetLevel(LevelNode* level);

	virtual Character* const			GetLocalCharacter() const						= 0;

	virtual void						CreateReticle();
	virtual void						ShowReticle(const bool show);
	
	//DECLARE_DEBUG_COMMAND_CALLBACK( merrymen::GameScene, playAnim )

protected:
										GameScene();

	std::vector<Character*>				m_players;
	LevelNode*							m_level;

	GameSceneGUI*						m_gamesceneGUI;

	bool								m_pausePressed;
	bool								m_pauseMenuOpened;
};

} // namespace merrymen

#endif __GAME_SCENE_H__