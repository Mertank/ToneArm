/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	7/23/2014 5:34:28 PM
------------------------------------------------------------------------------------------
*/

#include "SoundResource.h"

using namespace vgs;

IMPLEMENT_RTTI(SoundResource, Resource);
IMPLEMENT_PROTOTYPE(SoundResource)

SoundResource::SoundResource()
	: m_sound(nullptr)
{
	REGISTER_PROTOTYPE(SoundResource, "wav")
}

//
// destructor
//
SoundResource::~SoundResource() {

	delete m_sound;
}

//
// reads a sound effect file from the disk
//
bool SoundResource::Load(const char* path, const char key) {
	
	m_sound = new sf::SoundBuffer();

	if (m_sound->loadFromFile(path)) {
		return true;
	}

	return false;
}