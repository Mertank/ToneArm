/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Scene

		Scene class.

		Created by: Karl Merten
		Created on: 25/04/2014

========================
*/

#ifndef __SCENE_H__
#define __SCENE_H__

#include "Node.h"

namespace vgs {

class Camera;

class Scene : public Node {
public:

					CREATE_METH(Scene);

					Scene();

	bool			Init() override;
	void			Update(float dt) override;

	virtual void	EnterScene( void ) {}
	virtual void	ExitScene( void ) {}

	virtual void	LostFocus( void ) {}
	virtual void	GainedFocus( void ) {}
	virtual void	WillClose( void ) {}

	Camera*			GetCurrentCamera()					{ return m_currentCamera; }
	void			SetCurrentCamera(Camera* camera);

protected:
	Camera*			m_currentCamera;
};

} //namespace vgs

#endif //__SCENE_H__