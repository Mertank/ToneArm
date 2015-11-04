/*
========================

Copyright (c) 2014 Vinyl Games Studio

	FogOfWarPass

		Render pass for the fog of war.

		Created by: Karl Merten
		Created on: 01/07/2014

========================
*/

#ifndef __FOGOFWARPASS_H__
#define __FOGOFWARPASS_H__

#include "../ToneArmEngine/RenderPass.h"
#include "../ToneArmEngine/RTTI.h"
#include <GL\glew.h>
#include <glm\glm.hpp>

namespace vgs {
	class Frustum;
	class CameraProjectionData;
}

namespace merrymen {

class FogOfWarPass : public vgs::RenderPass {
										DECLARE_RTTI
public:
	virtual								~FogOfWarPass( void );
	static FogOfWarPass*				CreateWithPriority( unsigned int priority );
private:
										FogOfWarPass( void );

	virtual bool						InitializePass( void* passData );

	virtual void						BeginPass( void );
	virtual void						FinishPass( void );
	virtual void						ProcessRenderable( vgs::Renderable* obj );

	GLuint								m_frustumMesh;
	glm::vec3							m_fogPosition;
	const vgs::CameraProjectionData*	m_cameraData;
};

}

#endif //__FOGOFWARPASS_H__