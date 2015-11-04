/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RenderableNode

		Node for setting up parenting on the renderer.
		You cannot make an instance of this one.

		Created by: Karl Merten
		Created on: 28/07/2014

========================
*/

#ifndef __RENDERABLENODE_H__
#define __RENDERABLENODE_H__

#include "Node.h"
#include "Renderable.h"
#include "RenderPass.h"

namespace vgs {

class RenderModule;

class RenderableNode : public Node {
					DECLARE_RTTI
public:
					~RenderableNode( void );

	virtual void	AddChild( Node* node );
	virtual void	RemoveChild( Node* child );
	virtual void	RemoveChild( const unsigned int id );
	virtual void	RemoveFromParent( void );

	void			Update( float dt );

	void			SendTransformMessage( void );
	void			SendUpdateDiffuseColorMessage( const glm::vec3& color );

	void			SetVisible( bool visible = true );
	bool			IsVisible( void ) { return m_visible; };

	void			SetOutlineOnly( bool outline = true );
	void			SetOpacity( float alpha );
	void			SetLighting( bool lit, LightingType::Value lighting = LightingType::VERTEX_LIT );

	const float		GetOpacity() { return m_opacity; }

	void			SetPassType( PassType::Value passType );
	void			SetParentBone( unsigned long hash );
protected:
					RenderableNode( void );
	bool			m_visible;
	float			m_opacity;
	RenderModule*	m_renderModule;	
};

}

#endif //__RENDERABLENODE_H__