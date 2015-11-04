/*
========================

Copyright (c) 2014 Vinyl Games Studio

	CharacterController.h

		Created by: Vladyslav Dolhopolov
		Created on: 9/15/2014 4:12:17 PM

========================
*/
#ifndef __CHARACTERCONTROLLER_H__
#define __CHARACTERCONTROLLER_H__

namespace vgs
{
	class CharacterEntity;

	/*
	========================

		CharacterController

			Base class for either human player or AI.

			Created by: Vladyslav Dolhopolov
			Created on: 9/15/2014 4:12:17 PM

	========================
	*/
	class CharacterController
	{
	public:
							CharacterController(unsigned char charClass);
		virtual				~CharacterController();

		CharacterEntity*	GetCharacter()	{ return m_character; }

	protected:
		CharacterEntity*	m_character;
	};

} // namespace vgs

#endif __CHARACTERCONTROLLER_H__