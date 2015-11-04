/*
========================

Copyright (c) 2014 Vinyl Games Studio

	CharacterController.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 9/15/2014 4:12:17 PM

========================
*/
#include "CharacterController.h"
#include "CharacterEntity.h"

using namespace vgs;

CharacterController::CharacterController(unsigned char charClass)
	: m_character(new CharacterEntity(charClass, this))
{
}

CharacterController::~CharacterController()
{
}