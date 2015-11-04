/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/25/2014 4:40:54 PM
------------------------------------------------------------------------------------------
*/

#include "Sound.h"

using namespace vgs;

//
// parametrized onstructor
//
Sound::Sound(sf::Sound* const sound, const float duration):
	m_sound(sound),
	m_duration(duration),
	m_playTimer(-1.0f) {
		
		if (m_duration < 0.0f) {
			m_duration = (float)GetBuffer()->getDuration().asMilliseconds();
		}
}

//
// destructor
//
Sound::~Sound() {

	delete m_sound;
}

//
// plays the sound
//
void Sound::Play() {

	m_playTimer = 0.0f; // start the timer

	if (GetStatus() == sf::Sound::Playing) {
		Stop();
	}

	m_sound->play();
}

//
// pauses the sound
//
void Sound::Pause() {

	m_sound->pause();
}

//
// resumes the sound
//
void Sound::Resume() {

	if (GetStatus() == sf::Sound::Paused) {
		m_sound->play();
	}
}

//
// stops the sound
//
void Sound::Stop() {

	m_playTimer = -1.0f; // start the timer
	m_sound->stop();
}

//
// updates the state of the sound
//
bool Sound::Update(float dt) {

	if (GetStatus() != sf::Sound::Paused && m_playTimer >= 0.0f) {
		
		// imcrease the timer
		m_playTimer += dt;

		// time to stop playing the sound
		if (m_playTimer >= m_duration) {
			
			m_sound->stop();
			m_playTimer = -1.0f; // set the timer to a default value
			return true;
		}
	}

	// nothing to do, if the sound is not being played
	return false;
}

//
// duration setter
//
void Sound::SetDuration(const float duration) {

	// if the default was passed, set the duration of the sound to the duration of the sound from the buffer
	if (duration < 0.0f) {
		m_duration = (float)GetBuffer()->getDuration().asMilliseconds();
	}
	else {
		m_duration = duration;
	}
}