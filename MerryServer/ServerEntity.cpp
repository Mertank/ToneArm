/*
========================

Copyright (c) 2014 Vinyl Games Studio

	ServerEntity.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 5/26/2014 12:51:51 PM

========================
*/
#include "ServerEntity.h"

using namespace vgs;

IMPLEMENT_RTTI(ServerEntity, GameObject);

//
// updates the state of the ServerEntity
//
void ServerEntity::Update(float dt) {

	GameObject::Update(dt);
}