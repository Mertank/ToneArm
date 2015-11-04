/*
========================

Copyright (c) 2014 Vinyl Games Studio

	Renderable

		Base class for a renderable object.

		Created by: Karl Merten
		Created on: 01/05/2014

========================
*/

#ifndef __RENDEREABLE_H__
#define __RENDEREABLE_H__

#include "Transform.h"
#include "RenderPass.h"

namespace vgs {

namespace RenderableFeatures {
	enum Value {
		NONE				= 0x00,
		TEX_COORDS			= 0x01,
		NORMALS				= 0x02,
		VERTEX_LIGHTING		= 0x04,
		FRAGMENT_LIGHTING	= 0x08,
		DIFFUSE_TEXTURE		= 0x10,
		NORMAL_MAP			= 0x20,
		SPEC_MAP			= 0x40
	};
}

namespace LightingType {
	enum Value {
		VERTEX_LIT		= 0x00,
		FRAGMENT_LIT	= 0x01
	};
}

//class RenderMessage;
struct TransformMessage;

class Renderable {
								DECLARE_RTTI
public:
								Renderable( void );
	virtual						~Renderable( void );

	virtual void				UpdateTransform( TransformMessage* msg );
	virtual void				UpdateTransform( const glm::mat3& rotation, const glm::vec3& position );

	inline unsigned int			GetNodeID( void ) const { return m_nodeID; }
	inline void					SetNodeID( unsigned int newID ) { m_nodeID = newID; }

	inline Renderable*			GetParentRenderable( void ) const { return m_parent; }
	inline void					SetParentRenderable( Renderable* renderable ) { m_parent = renderable; m_transformChanged = true; }

	void						AddChild( Renderable* renderable );
	void						RemoveChild( unsigned int id );

	virtual void				PreRender( void ) = 0;
	virtual void				Render( void ) = 0;
	virtual void				PostRender( void ) = 0;

	virtual float				GetOpacity( void ) = 0;

	virtual void				SetDrawingOutlineOnly( bool outline = true ) { m_drawOutlineOnly = outline; }
	inline bool					IsDrawingOutlineOnly( void ) { return m_drawOutlineOnly; }

	void						SetVisible( bool visible = true ) { m_visible = visible; }
	bool						IsVisible( void ) { return m_visible; }

	virtual void				IsLit( bool lit ) { m_useLights = lit; }
	inline bool					IsLit( void ) const { return m_useLights; }

	bool						DidTransformChange( void ) const;

	inline PassType::Value		GetAttachedPass( void ) const { return m_attachedPass; }
	inline void					SetAttachedPass( PassType::Value pass ) { m_attachedPass = pass; }

	inline LightingType::Value	GetLightingType( void ) const { return m_lightingType; }
	virtual void				SetLightingType( LightingType::Value lightingType ) { m_lightingType = lightingType; }

	const RenderModule*			GetRenderer( void ) const { return m_renderer; }
	void						SetRenderer( const RenderModule* renderer ) { m_renderer = renderer; }

	const glm::mat4&			GetModelMatrix( void ) const { return m_modelMatrix; }
	const glm::mat4&			GetCompositeMatrix( void );
	void						UpdateCompositeMatrix( void );

	virtual void				UpdatedCompositeMatrix( void ) {}

	const unsigned long long&	GetFrameStamp() const { return m_frameStamp; }
	
	virtual void				Update( float dt );

	Renderable*					GetChildRenderable( unsigned int node );
	std::vector<Renderable*>&	GetChildren( void ) { return m_children; }
protected:
	bool						m_visible;
	bool						m_drawOutlineOnly;
	bool						m_useLights;
	bool						m_transformChanged;
	unsigned long long			m_frameStamp;

	unsigned int				m_nodeID;
	glm::mat4					m_modelMatrix;
	glm::mat4					m_compositeMatrix;
	RenderableFeatures::Value	m_requiredFeatures;
	LightingType::Value			m_lightingType;
	PassType::Value				m_attachedPass;

	Renderable*					m_parent;
	std::vector<Renderable*>	m_children;

	const RenderModule* 		m_renderer;
};

} //namespace vgs

#endif //__RENDEREABLE_H__