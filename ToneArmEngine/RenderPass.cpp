/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	RenderPass

	Created by: Karl Merten
	Created on: 30/06/2014

========================
*/
#include "RenderPass.h"
#include "Log.h"
#include "Renderable.h"
#include "RenderableModel.h"
#include "RenderModule.h"
#include "CameraProjectionData.h"

namespace vgs {
IMPLEMENT_BASE_RTTI( RenderPass )
/*
========
RenderPass::RenderPass

	RenderPass default constructor
========
*/
RenderPass::RenderPass( void ) :
	m_passType( PassType::NONE )
{}
/*
========
RenderPass::~RenderPass

	RenderPass destructor
========
*/
RenderPass::~RenderPass( void ) 
{}
/*
========
RenderPass::DoPass

	Does this pass.
========
*/
void RenderPass::DoPass( std::unordered_map<unsigned int, Renderable*>* renderObjects, RenderModule* renderer ) {
#ifdef TONEARM_DEBUG
	if ( m_passType == PassType::NONE ) {
		Log::GetInstance()->WriteToLog( "RenderPass", "No pass type was set" );
		return;
	}
#endif
	BeginPass(); //Do some setup for this pass
	
	for ( std::unordered_map<unsigned int, Renderable*>::iterator iter = renderObjects->begin();
		  iter != renderObjects->end(); ++iter ) {
		if ( iter->second->GetAttachedPass() & m_passType ) {
			if ( iter->second->GetRTTI() == RenderableModel::rtti ) {
				RenderableModel* model = ( RenderableModel* )iter->second;
				if ( renderer->GetCameraFrustum().BoxInFrustum( ( BoxShape& )model->GetBoundingBox() ) == FrustumResult::OUTSIDE ) {
					ProcessChildren( iter->second, renderer );
					continue;
				}
			}

			ProcessRenderable( iter->second );		
			ProcessChildren( iter->second, renderer );
		}
	}
	
	FinishPass(); //Clean up pass resource stuff.
}
/*
========
RenderPass::DoPass

	Does this pass.
========
*/
void RenderPass::ProcessChildren( Renderable* obj, RenderModule* renderer ) {
	std::vector<Renderable*>& children = obj->GetChildren();
	for ( std::vector<Renderable*>::iterator iter = children.begin();
		  iter != children.end(); ++iter ) {
		if ( ( *iter )->GetAttachedPass() & m_passType ) {
			if ( ( *iter )->GetRTTI() == RenderableModel::rtti ) {
				RenderableModel* model = ( RenderableModel* )( *iter );
				if ( renderer->GetCameraFrustum().BoxInFrustum( ( BoxShape& )model->GetBoundingBox() ) == FrustumResult::OUTSIDE ) {
					ProcessChildren( ( *iter ), renderer );
					continue;
				}
			}

			ProcessRenderable( ( *iter ) );		
			ProcessChildren( ( *iter ), renderer );
		}
	}
}
}