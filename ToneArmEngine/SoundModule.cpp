/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/23/2014 3:35:20 PM
------------------------------------------------------------------------------------------
*/

#include "SoundModule.h"
#include "CachedResourceLoader.h"
#include "SoundResource.h"
#include "Sound.h"
#include "Engine.h"
#include "EngineModule.h"

#include <iostream>

#define	MAX_VOLUME					100.0f
#define	VOLUME_DROP_PACE			100.0f	// volume for all sounds drops by VOLUME_DROP_RATE,
#define	VOLUME_DROP_RATE			8.5f;	// when the source sound moves away from the sound listener by a distance of VOLUME_DROP_PACE

using namespace vgs;

//
// default constructor
//
SoundModule::SoundModule()
{}

//
// destructor
//
SoundModule::~SoundModule() {

	for (auto sound : m_combatSounds) {
		delete sound.second;
	}

	m_combatSounds.clear();
	m_soundPtrs.clear();
}

//
// plays a sound once with the specified volume and stops it after the specified time
//
void SoundModule::PlaySound(const char* fileName, const float volume, const float duration) {

	// find out if that sound was already created
	sf::SoundBuffer* buffer = FindSoundBufferByName(fileName);

	// if this sound has already been played, but "free" right now, play it
	std::pair<SoundMap::iterator, SoundMap::iterator> equalRange = m_combatSounds.equal_range(buffer);

	for (SoundMap::iterator soundsItr = equalRange.first; soundsItr != equalRange.second; ++soundsItr) {

		Sound* sound = soundsItr->second;

		if (sound->GetStatus() != sf::Sound::Playing) {

			sound->SetVolume(volume);
			sound->SetDuration(duration);
			sound->Play();
			return;
		}
	}

	// otherwise, create a new sound from the same buffer
	Sound* sound = new Sound(new sf::Sound(*buffer), duration);

	sound->SetVolume(volume);
	sound->Play();

	m_combatSounds.insert(std::pair<sf::SoundBuffer*, Sound*>(buffer, sound));
}

//
// only plays the sound if no sound with the same SoundBuffer is being played at the moment
//
void SoundModule::PlayUniqueSound(const char* fileName, const float volume, const float duration) {

	// find out if that sound was already created
	sf::SoundBuffer* buffer = FindSoundBufferByName(fileName);

	SoundMap::iterator soundItr = m_combatSounds.find(buffer);

	if (soundItr != m_combatSounds.end()) {

		Sound* sound = soundItr->second;

		// play the sound if it is not already being played
		if (sound->GetStatus() != sf::Sound::Playing) {

			sound->SetDuration(duration);
			sound->SetVolume(volume);
			sound->Play();
		}
	}
	// this sound was never played before, create it, play, and store for future use
	else {
		Sound* sound = new Sound(new sf::Sound(*buffer), duration);

		sound->SetVolume(volume);
		sound->Play();

		m_combatSounds.insert(std::pair<sf::SoundBuffer*, Sound*>(buffer, sound));
	}
}

//
// finds a sound and tries to pause it
//
void SoundModule::PauseUniqueSound(const char* fileName) {

	sf::SoundBuffer* buffer = FindSoundBufferByName(fileName);

	SoundMap::iterator soundItr = m_combatSounds.find(buffer);

	if (soundItr != m_combatSounds.end()) {
		soundItr->second->Pause();
	}
}

//
// finds a sound and tries to resume it
//
void SoundModule::ResumeUniqueSound(const char* fileName) {

	sf::SoundBuffer* buffer = FindSoundBufferByName(fileName);

	SoundMap::iterator soundItr = m_combatSounds.find(buffer);

	if (soundItr != m_combatSounds.end()) {
		soundItr->second->Resume();
	}
}

//
// finds a sound and tries to stop it
//
void SoundModule::StopUniqueSound(const char* fileName) {

	sf::SoundBuffer* buffer = FindSoundBufferByName(fileName);

	SoundMap::iterator soundItr = m_combatSounds.find(buffer);

	if (soundItr != m_combatSounds.end()) {
		soundItr->second->Stop();
	}
}

//
// calculates volume for a sound based on the distance between its source and the listener
//
const float SoundModule::CalculateVolume(const float distanceToSound) {

	float result = distanceToSound / VOLUME_DROP_PACE * VOLUME_DROP_RATE;
	return std::max(MAX_VOLUME - result, 0.0f);
}

//
// starts up
//
void SoundModule::Startup(void)
{
}

//
// manages currently played sounds
//
void SoundModule::Update(float dt) {

	// let each sound update itself
	for (SoundMap::iterator soundsItr = m_combatSounds.begin(); soundsItr != m_combatSounds.end();) {
		
		if ((*soundsItr).second->Update(dt)) {
			soundsItr = m_combatSounds.erase(soundsItr);
		}
		else {
			++soundsItr;
		}
	}
}

//
// shuts down
//
void SoundModule::Shutdown(void)
{
}

//
// queries the ResourceLoader for a SoundResource with a passed name and returns a SoundBuffer from that SoundResource
//
sf::SoundBuffer* SoundModule::FindSoundBufferByName(const char* fileName) {

	// load and save a sound resource
	CachedResourceLoader* loader = Engine::GetInstance()->GetModuleByType<CachedResourceLoader>(EngineModuleType::RESOURCELOADER);

	std::shared_ptr<SoundResource> srPtr = loader->LoadResource<SoundResource>(fileName);
	m_soundPtrs.insert(srPtr);

	return srPtr.get()->GetSoundBuffer();
}