/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/25/2014 4:40:54 PM
------------------------------------------------------------------------------------------
*/

#ifndef __SOUND_H__
#define __SOUND_H__

#include <SFML\Audio.hpp>

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	Sound

	VGS wrapper for sf::Sound class
------------------------------------------------------------------------------------------
*/
class Sound {

public:
									Sound(sf::Sound* const sound, const float duration);
									~Sound();

	bool							Update(float dt);
	void							Play();
	void							Pause();
	void							Resume();
	void							Stop();

	// accessors
	sf::Sound* const				GetSound() const							{ return m_sound; }
	void							SetSound(sf::Sound* const sound)			{ m_sound = sound; }

	const float						GetDuration() const							{ return m_duration; }
	void							SetDuration(const float duration);

	const float						GetVolume() const							{ return m_sound->getVolume(); }
	void							SetVolume(const float volume)				{ m_sound->setVolume(volume); }

	const sf::SoundSource::Status	GetStatus() const							{ return m_sound->getStatus(); }

	const sf::SoundBuffer*			GetBuffer() const							{ return m_sound->getBuffer(); }
	void							SetBuffer(const sf::SoundBuffer& buffer)	{ m_sound->setBuffer(buffer); }

private:
									Sound();

	sf::Sound*						m_sound;
	float							m_duration;
	float							m_playTimer;
};

}

#endif __SOUND_H__