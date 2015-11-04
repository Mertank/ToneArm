/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/16/2014 3:14:02 PM
------------------------------------------------------------------------------------------
*/

#include "SniperCharacter.h"
#include "GameScene.h"
#include "SinglePLayerGameScene.h"

#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/EngineModule.h"
#include "../ToneArmEngine/CachedResourceLoader.h"
#include "../ToneArmEngine/InitializationFileResource.h"

using namespace merrymen;
using namespace vgs;

IMPLEMENT_CHARACTER_CLASS(SniperCharacter, Character);

//
// initializes the SniperCharacter object with the passed values
//
bool SniperCharacter::Init(const char* modelFile, const char* iniFile, const char* categoryName, bool online) {

	if (Character::Init(modelFile, iniFile, categoryName, online)) {

		// fill in arrays with names of the sound files
		char** sniperRifleSoundFiles = new char*[4];
		
		sniperRifleSoundFiles[0] = "Audio/SniperRifleShot.wav";
		sniperRifleSoundFiles[1] = "Audio/SniperRifleLoad.wav";
		sniperRifleSoundFiles[2] = "Audio/SniperRifleCock.wav";
		sniperRifleSoundFiles[3] = "Audio/TriggerClick.wav";

		char** pistolSoundFiles = new char*[4];
		
		pistolSoundFiles[0] = "Audio/PistolShot.wav";
		pistolSoundFiles[1] = "Audio/PistolLoad.wav";
		pistolSoundFiles[2] = "Audio/PistolCock.wav";
		pistolSoundFiles[3] = "Audio/TriggerClick.wav";

		// add weapons
		AddWeapon(WeaponID::SniperRifle, WeaponCS::Create(WeaponID::SniperRifle, this, "Models/Sniper.vgsModel", sniperRifleSoundFiles, "Data/Weapons.ini", "Sniper_Rifle"));
		AddWeapon(WeaponID::Pistol, WeaponCS::Create(WeaponID::Pistol, this, "Models/TankPistol.vgsModel", pistolSoundFiles, "Data/Weapons.ini", "Pistol"));

		std::shared_ptr<InitializationFileResource> ammoIni = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>(EngineModuleType::RESOURCELOADER)->LoadResource<InitializationFileResource>("Data/Projectiles.ini");

		m_weapons[WeaponID::SniperRifle]->SetProjectileStats(ProjectileStats(ProjectileType::HollowPointBullet, *ammoIni->GetCategory("Hollow_Point_Bullet")));
		m_weapons[WeaponID::Pistol]->SetProjectileStats(ProjectileStats(ProjectileType::HollowPointBullet, *ammoIni->GetCategory("Hollow_Point_Bullet")));

		SetActiveWeapon(WeaponID::SniperRifle);

		delete sniperRifleSoundFiles;
		delete pistolSoundFiles;

		return true;
	}

	return false;
}

//
// SniperCharacter object updates it's state
//
void SniperCharacter::Update(float dt) {

	Character::Update(dt);
}

//
// changes the FOV of the player, sets the m_aiming value used for the animation system and shows/hides reticle
//
void SniperCharacter::SetAiming(const bool aiming) {

	GameScene * gs = static_cast<GameScene*>(Engine::GetRunningScene());

	if (gs && this == gs->GetLocalCharacter() ) {
		if ( !aiming && ( m_activeWeapon == m_weapons.begin()->second ) ) {
			gs->ShowReticle( false );
		} else {
			gs->ShowReticle( true );
		}
	}

	Character::SetAiming(aiming);
}

void SniperCharacter::SetActiveWeapon( WeaponID wid ) {
	Character::SetActiveWeapon( wid );

	Scene* runningScene = Engine::GetRunningScene();

	if (runningScene->GetRTTI() == GameScene::rtti) {
		GameScene * gs = static_cast<GameScene*>(Engine::GetRunningScene());
		if ( gs && this == gs->GetLocalCharacter() ) {
			if ( wid == WeaponID::SniperRifle ) {
				gs->ShowReticle( false );
			} else {
				gs->ShowReticle( true );
			
			}
		}
	}
}