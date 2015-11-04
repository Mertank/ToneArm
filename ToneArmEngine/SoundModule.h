/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/23/2014 3:35:20 PM
------------------------------------------------------------------------------------------
*/

#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

#include "EngineModule.h"

#include <SFML/Audio.hpp>
#include <memory>
#include <map>

namespace vgs
{

class Sound;
class SoundResource;

/*
------------------------------------------------------------------------------------------
	SoundModule

	Engine module that plays sounds using SFML audio module.
------------------------------------------------------------------------------------------
*/
class SoundModule : public EngineModule {

	typedef std::multimap<sf::SoundBuffer*, Sound*> SoundMap;

public:
													SoundModule();
													~SoundModule();

	static SoundModule*								Create()			{ return new SoundModule(); }

	virtual void									Startup(void)		override;
	virtual void									Update(float dt)	override;
	virtual void									Shutdown(void)		override;

	void											PlaySound(const char* fileName, const float volume, const float duration = -1.0f);
	void											PlayUniqueSound(const char* fileName, const float volume, const float duration = -1.0f);
	void											PauseUniqueSound(const char* filename);
	void											ResumeUniqueSound(const char* filename);
	void											StopUniqueSound(const char* fileName);

	sf::SoundBuffer*								FindSoundBufferByName(const char* fileName);

	static const float								CalculateVolume(const float distanceToSound);

private:
	std::set<std::shared_ptr<SoundResource>>		m_soundPtrs;
	SoundMap										m_combatSounds;
};

}

#endif __SOUND_MANAGER_H__