/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RenderableModel

		Class representing a model.
		Contains a series of meshes to render.

		Created by: Karl Merten
		Created on: 06/05/2014

========================
*/

#ifndef __RENDERABLEMODEL_H__
#define __RENDERABLEMODEL_H__

#include "Renderable.h"
#include "BoxShape.h"

namespace vgs {

struct CreateModelMessage;
struct CreateMeshMessage;
class RenderableMesh;
class Texture;
class Animation;
class Animator;

class RenderableModel : public Renderable {
						DECLARE_RTTI
public:
						RenderableModel( const RenderableModel& other );
	virtual				~RenderableModel( void );

	virtual void		AttachMesh( RenderableMesh* mesh );
	virtual void		AttachTexture( Texture* texture, unsigned int meshIndex );	

	RenderableMesh*		GetMesh( unsigned int index );

	virtual void		IsLit( bool lit );
	virtual void		SetDrawingOutlineOnly( bool outline = true );
	virtual void		SetDiffuseColor( const glm::vec3& diffColor );

	virtual void		SetOpacity( float opacity );
	virtual float		GetOpacity( void );

	virtual void		UpdateTransform( TransformMessage* msg );
	virtual void		UpdateTransform( const glm::mat3& rot, const glm::vec3& loc );

	Shape&				GetBoundingBox( void ) { UpdatedCompositeMatrix(); return m_boundingBox; }

	inline Animator*	GetAnimator( void ) { return m_animator; }
	virtual void		AttachAnimator( Animator* animator ) { m_animator = animator; }

	virtual void		AddAnimation( Animation* anim );
	void				SetBindMatricies( glm::mat4* matricies, unsigned int count );

	virtual void		Update( float dt );
	void				SetMatchBoneHash( unsigned long hash ) { m_matchBoneHash = hash; }
	virtual void		UpdatedCompositeMatrix( void );

	unsigned int		GetNextMeshIndex( void );	
protected:
						RenderableModel( void );

	RenderableMesh**	m_meshes;
	unsigned int		m_meshCount;	

	glm::vec3			m_boxOffset;
	BoxShape			m_boundingBox;

	Animator*			m_animator;
	unsigned long		m_matchBoneHash;
};

}

#endif //__RENDERABLEMODEL_H__