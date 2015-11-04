/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RenderPass

		An abstract class for a pass of the renderer.

		Created by: Karl Merten
		Created on: 30/06/2014

========================
*/

#ifndef __RENDERPASS_H__
#define __RENDERPASS_H__

#include <unordered_map>
#include "RTTI.h"

namespace vgs {

namespace PassType {
	enum Value {
		NONE				= 0x00,
		DIFFUSE				= 0x01,
		TRANSPARENCY		= 0x02,
		GUI					= 0x04,
		USER_DEFINED_PASS	= 0x08
	};
}

class Renderable;
class RenderModule;

class RenderPass {
					DECLARE_RTTI
public:
					RenderPass( void );
	virtual			~RenderPass( void );

	virtual void	DoPass( std::unordered_map<unsigned int, Renderable*>* renderObjects, RenderModule* renderer );
	
	inline void		SetPriority( unsigned int priority ) { m_priority = priority; }
	inline void		SetPassType( PassType::Value passType ) { m_passType = passType; };

	virtual bool	InitializePass( void* passData ) { return true; }

	struct RenderPassComparator {
		bool operator()( const RenderPass* a, const RenderPass* b ) {
			return a->m_priority < b->m_priority;
		}

	};	
private:
	virtual void	BeginPass( void ) = 0;
	virtual void	FinishPass( void ) = 0;
	virtual void	ProcessRenderable( Renderable* obj ) = 0;

	void			ProcessChildren( Renderable* obj, RenderModule* renderer );
protected:
	PassType::Value	m_passType;
	unsigned int	m_priority;
};
}

#endif //__RENDERPASS_H__