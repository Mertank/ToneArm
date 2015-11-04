/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/16/2014 12:31:56 PM
------------------------------------------------------------------------------------------
*/

#include "GameScene.h"
#include "TankCharacter.h"
#include "SniperCharacter.h"
#include "FogOfWarNode.h"
#include "FogOfWarPass.h"
#include "TeamDeathMatchMH.h"
#include "PauseGUI.h"

#include <PickableItem.h>

#include "../ToneArmEngine/RunningInfo.h"
#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/OpenGLRenderModule.h"
#include "../ToneArmEngine/GUIManager.h"
#include "../ToneArmEngine/NetworkModule.h"
#include "../ToneArmEngine/LevelNode.h"
#include "../ToneArmEngine/LevelResource.h"
#include "../ToneArmEngine/CachedResourceLoader.h"
#include "../ToneArmEngine/SpacePartitionTree.h"
#include "../ToneArmEngine/NetworkNode.h"
#include "../ToneArmEngine/InputModule.h"
#include "../ToneArmEngine/ScoreTableGUI.h"
#include "../ToneArmEngine/ParticleEmitterNode.h"
#include "../ToneArmEngine/MeshHelper.h"
#include "../ToneArmEngine/SoundModule.h"
#include "../ToneArmEngine/AnimationNode.h"
#include "../ToneArmEngine/OpenGLRenderModule.h"
#include "../ToneArmEngine/AnimationNode.h"

#include <SFML\Window\Keyboard.hpp>

using namespace merrymen;
using namespace vgs;

IMPLEMENT_RTTI(GameScene, Scene);

//INITIALIZE_DEBUG_COMMAND_CALLBACK( merrymen::GameScene, playAnim )

//
// default constructor
//
GameScene::GameScene()
	: m_players(0)
	, m_gamesceneGUI(nullptr)
	, m_pausePressed(false)
	, m_pauseMenuOpened(false)
{}

//
// destructor
//
GameScene::~GameScene()
{
	m_players.clear();
}


void GameScene::EnterScene( void ) {
	FogOfWarPass* pass = FogOfWarPass::CreateWithPriority( 10 );
	
	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );
	
	AddPassMessage* msg = renderer->GetRenderMessage<AddPassMessage>();
	msg->Init( pass );
	renderer->SendRenderMessage( msg );
}

void GameScene::ExitScene( void ) {
	// remove gamescene gui layer
	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();
	if (guiMgr)
	{
		guiMgr->RemoveLayer("game_scene_gui");
	}

	RenderModule* renderer = Engine::GetInstance()->GetModuleByType<RenderModule>( EngineModuleType::RENDERER );

	DeletePassMessage* msg = renderer->GetRenderMessage<DeletePassMessage>();
	msg->Init( 0, &FogOfWarPass::rtti );
	renderer->SendRenderMessage( msg );
}

//
// initializes relevant gui elements
//
void GameScene::InitGUI() {

	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();

	m_gamesceneGUI = new GameSceneGUI();
	guiMgr->CreateLayer(m_gamesceneGUI, "game_scene_gui");

	UpdateGUI();
}

//
// updates GameScene's childen
//
void GameScene::Update(float dt) {

	InputModule* iMgr = Engine::GetInstance()->GetModuleByType<InputModule>(EngineModuleType::INPUTER);
	GUIManager* guiMgr = Engine::GetInstance()->GetModuleByType<OpenGLRenderModule>(EngineModuleType::RENDERER)->GetGUIManager();

	if (iMgr)
	{
		if (guiMgr && iMgr->IsKeyPressed(InputModule::ControlElement::Pause) && !m_pausePressed)
		{
			m_pausePressed = true;
			m_pauseMenuOpened = !guiMgr->GetLayer<PauseGUI>("pause_menu_gui")->IsVisible();
			guiMgr->SendVisMsg(SetVisibilityMsg("pause_menu_gui", m_pauseMenuOpened));
		}
		else if (!iMgr->IsKeyPressed(InputModule::ControlElement::Pause))
		{
			m_pausePressed = false;
		}
	}

	if (!m_pauseMenuOpened)
	{
		Node::Update(dt);
	}
}

//
// updates gui elements
//
void GameScene::UpdateGUI() {

	if (m_gamesceneGUI)
	{
		Character* localPlayer = GetLocalCharacter();
		if (localPlayer && localPlayer->GetActiveWeapon())
		{ 
			UpdateLabelsMsg uMsg(localPlayer->GetActiveWeapon()->GetWeaponStats().Ammo, localPlayer->GetActiveWeapon()->GetWeaponStats().TotalAmmo,
				localPlayer->GetCharacterStats().HP, localPlayer->GetCharacterStats().Stamina, localPlayer->GetCharacterStats().Armour);
		
			// update labels with character's stats
			m_gamesceneGUI->SetUpdateLabelsMsg(uMsg);
		}
	}
}

//
// removes the player from the scene
//
void GameScene::RemovePlayer(Character* player)
{
	std::vector<Character*>::iterator end = m_players.end();

	for (std::vector<Character*>::iterator it = m_players.begin(); it != end; ++it)
	{
		if (*it == player)
		{
			m_players.erase(it);
			break;
		}
	}

	RemoveChild(player);
}

//
// returns a pointer to the local player's character
//
void GameScene::SetLevel(LevelNode* level)						
{ 
	// TODO: previous should be freed
	m_level = level;
	AddChild(level);
}

//
// creates reticle for the main character
//
void GameScene::CreateReticle() {

	// TODO: change that stuff once we have a custom cursor
	float* vertexData = NULL;
	unsigned short* indexData = NULL;
	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;
		
	MeshHelper::MakeLine( 2250.0f, &vertexData, &indexData, vertexCount, indexCount );
		
	DynamicModelNode* shootDirection = DynamicModelNode::CreateWithMesh( vertexData, indexData, GL_UNSIGNED_SHORT, vertexCount, indexCount, VertexComponents::POSITION, glm::vec3( 0.0f, 0.0f, 1.0f ), glm::vec3( 0.0f, 0.0f, 500.0f ), glm::vec3( 1.0f, 1.0f, 2250.0f ) );
	shootDirection->SetOutlineOnly( true );
	shootDirection->SetLighting( false );
	shootDirection->SetPosition( glm::vec3( -5.0f, CHARACTER_HEIGHT / 2, 0.0f ) );

	Character* local = GetLocalCharacter();

	local->GetModel()->AddChild(shootDirection);

	if (local->GetRTTI() == SniperCharacter::rtti) {
		ShowReticle(false);
	}
}

//
// shows/hides the reticle
//
void GameScene::ShowReticle(const bool show) {
	// TODO: change that stuff once we have a custom cursor
	if (m_players.size() > 0) {
		
		Character* local = GetLocalCharacter();

		if (local) {

			DynamicModelNode* reticle = local->GetModel()->GetChildOfType<DynamicModelNode>();

			if (reticle && reticle->IsVisible() != show) {
				reticle->SetVisible(show);
			}
		}
	}
}

/*BEGIN_CALLBACK_FUNCTION( merrymen::GameScene, playAnim )
	( ( GameScene* )Engine::GetInstance()->GetRunningScene() )->GetLocalCharacter()->GetChildOfType<AnimationNode>()->PlayAnimation( args[0] );
END_CALLBACK_FUNCTION*/