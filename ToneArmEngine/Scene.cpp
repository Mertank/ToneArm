/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	Scene

	Created by: Karl Merten
	Created on: 25/04/2014

========================
*/
#include "Scene.h"
#include "Camera.h"

using namespace vgs;
/*
==========
Scene::Scene

	Scene default constructor
==========
*/
Scene::Scene()
	: m_currentCamera(nullptr)
{
}
/*
==========
Scene::Init

	Scene initialization
==========
*/
bool Scene::Init() {
	return true;
}
/*
==========
Scene::Update

	Scene Update
==========
*/
void Scene::Update(float dt) {
	Node::Update(dt);
}
/*
==========
Scene::SetCurrentCamera

	Scene set camera
==========
*/
void Scene::SetCurrentCamera( Camera* camera ) {
	m_currentCamera = camera;
	AddChild(camera); 
}