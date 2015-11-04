/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/19/2014 4:30:15 PM
------------------------------------------------------------------------------------------
*/

#include "TankCharacter.h"

#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/EngineModule.h"
#include "../ToneArmEngine/CachedResourceLoader.h"
#include "../ToneArmEngine/InitializationFileResource.h"

using namespace merrymen;
using namespace vgs;

IMPLEMENT_CHARACTER_CLASS(TankCharacter, Character);

//
// initializes the TankCharacter object with the passed values
//
bool TankCharacter::Init(const char* modelFile, const char* iniFile, const char* categoryName, bool online) {

	if (Character::Init(modelFile, iniFile, categoryName, online)) {

		// fill in arrays with names of the sound files
		char** shotgunSoundFiles = new char*[4];
		
		shotgunSoundFiles[0] = "Audio/ShotgunShot.wav";
		shotgunSoundFiles[1] = "Audio/ShotgunLoad.wav";
		shotgunSoundFiles[2] = "Audio/ShotgunCock.wav";
		shotgunSoundFiles[3] = "Audio/TriggerClick.wav";

		char** pistolSoundFiles = new char*[4];
		
		pistolSoundFiles[0] = "Audio/PistolShot.wav";
		pistolSoundFiles[1] = "Audio/PistolLoad.wav";
		pistolSoundFiles[2] = "Audio/PistolCock.wav";
		pistolSoundFiles[3] = "Audio/TriggerClick.wav";

		// add weapons
		AddWeapon(WeaponID::Shotgun, WeaponCS::Create(WeaponID::Shotgun, this, "Models/TankShotgun.vgsModel", shotgunSoundFiles, "Data/Weapons.ini", "Shotgun"));
		AddWeapon(WeaponID::Pistol, WeaponCS::Create(WeaponID::Pistol, this, "Models/TankPistol.vgsModel", pistolSoundFiles, "Data/Weapons.ini", "Pistol"));

		CachedResourceLoader* loader = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>(EngineModuleType::RESOURCELOADER);
		std::shared_ptr<InitializationFileResource> ammoIni = loader->LoadResource<InitializationFileResource>("Data/Projectiles.ini");

		m_weapons[WeaponID::Shotgun]->SetProjectileStats(ProjectileStats(ProjectileType::Shot, *ammoIni->GetCategory("Shot")));
		m_weapons[WeaponID::Pistol]->SetProjectileStats(ProjectileStats(ProjectileType::HollowPointBullet, *ammoIni->GetCategory("Hollow_Point_Bullet")));

		SetActiveWeapon(WeaponID::Shotgun);

		// TODO: delete char**
		delete shotgunSoundFiles;
		delete pistolSoundFiles;
		return true;
	}

	return false;
}

//
// TankCharacter object updates it's state
//
void TankCharacter::Update(float dt) {

	Character::Update(dt);
}