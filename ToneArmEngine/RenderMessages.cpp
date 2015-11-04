/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	RenderMessages

	Created by: Karl Merten
	Created on: 5/8/2014 12:59:03 PM

========================
*/
#include "RenderMessages.h"

namespace vgs {

IMPLEMENT_BASE_RTTI( RenderMessage )

IMPLEMENT_RTTI( TransformMessage				, RenderMessage )
IMPLEMENT_RTTI( VisibilityMessage				, RenderMessage )
IMPLEMENT_RTTI( UpdateOpacityMessage			, RenderMessage )
IMPLEMENT_RTTI( UpdateDiffuseColorMessage		, RenderMessage )
IMPLEMENT_RTTI( OutlineMessage					, RenderMessage )
IMPLEMENT_RTTI( LightingMessage					, RenderMessage )
IMPLEMENT_RTTI( CameraTransformMessage			, RenderMessage )
IMPLEMENT_RTTI( DuplicateModelMessage			, RenderMessage )
IMPLEMENT_RTTI( CreateMeshMessage				, RenderMessage )
IMPLEMENT_RTTI( UpdateMeshMessage				, RenderMessage )
IMPLEMENT_RTTI( CreateModelMessage				, RenderMessage )
IMPLEMENT_RTTI( CreateTextureMessage			, RenderMessage )
IMPLEMENT_RTTI( DestroyMessage					, RenderMessage )
IMPLEMENT_RTTI( UpdatePassTypeMessage			, RenderMessage )
IMPLEMENT_RTTI( AddPassMessage					, RenderMessage )
IMPLEMENT_RTTI( DeletePassMessage				, RenderMessage )
IMPLEMENT_RTTI( UpdateCameraProjectionMessage	, RenderMessage )
IMPLEMENT_RTTI( CreateDynamicMeshMessage		, RenderMessage )
IMPLEMENT_RTTI( AddAnimationMessage				, RenderMessage )
IMPLEMENT_RTTI( InverseBoneMatriciesMessage		, RenderMessage )
IMPLEMENT_RTTI( PlayAnimationMessage			, RenderMessage )
IMPLEMENT_RTTI( CreateParticleEmitterMessage	, RenderMessage )
IMPLEMENT_RTTI( EmitParticlesMessage			, RenderMessage )
IMPLEMENT_RTTI( PlayBlendedAnimationsMessage	, RenderMessage )
IMPLEMENT_RTTI( SetParentBoneMessage			, RenderMessage )
IMPLEMENT_RTTI( UpdateEmitterMessage			, RenderMessage )
IMPLEMENT_RTTI( PlayAdditiveAnimationMessage	, RenderMessage )

}