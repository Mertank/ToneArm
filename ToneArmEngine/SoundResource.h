/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/23/2014 5:34:28 PM
------------------------------------------------------------------------------------------
*/

#ifndef __SOUND_RESOURCE_H__
#define __SOUND_RESOURCE_H__

#include "Resource.h"

#include <SFML\Audio.hpp>

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	SoundResource

	A sound effect stored in a buffer.
------------------------------------------------------------------------------------------
*/
class SoundResource : public Resource {

	friend class CachedResourceLoader;

	DECLARE_RTTI;
	DECLARE_PROTOTYPE(SoundResource)

public:
								~SoundResource();

	virtual bool				Load(const char* path, const char key = 0x00);

	sf::SoundBuffer* const		GetSoundBuffer() const								{ return m_sound; }
	void						SetSoundBuffer(sf::SoundBuffer* const buffer)		{ m_sound = buffer; }

private:
								SoundResource();

	sf::SoundBuffer*			m_sound;
};

}

#endif __SOUND_RESOURCE_H__