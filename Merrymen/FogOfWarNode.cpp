/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	FogOfWarNode

	Created by: Karl Merten
	Created on: 04/06/2014

========================
*/
#include "FogOfWarNode.h"
#include "GameScene.h"
#include "CustomRenderPasses.h"

#include "../ToneArmEngine/Log.h"
#include "../ToneArmEngine/Shape.h"
#include "../ToneArmEngine/Engine.h"
#include "../ToneArmEngine/PhysicsModule.h"
#include "../ToneArmEngine/BoxCollider.h"
#include "../ToneArmEngine/CylinderShape.h"
#include "../ToneArmEngine/SpacePartitionTree.h"
#include "../ToneArmEngine/MeshHelper.h"
#include "../ToneArmEngine/LevelNode.h"
#include "../ToneArmEngine/NetworkModule.h"
#include "../ToneArmEngine/RenderModule.h"
#include "../ToneArmEngine/NetworkNode.h"
#include "../ToneArmEngine/RunningInfo.h"

#include <algorithm>

namespace merrymen {
/*
============
FogOfWarNode::FogOfWarNode

	FogOfWarNode default constructor
============
*/
FogOfWarNode::FogOfWarNode( void ) :
	m_fogOfWarMeshNode( NULL ),
	m_character( NULL )
{}
/*
============
FogOfWarNode::Init

	FogOfWarNode initialization
============
*/
bool FogOfWarNode::Init( void ) {	
	if ( !Node::Init() ) {
		return false;
	}

	//Debug stuff
	/*for ( int i = 0; i < 50; ++i ) {
		m_nodes[i] = DynamicModelNode::CreateWithMesh( NULL, NULL, GL_UNSIGNED_SHORT, 0, 0, VertexComponents::POSITION, glm::vec3( 0.0f, 1.0f, 0.0f ) );
		m_nodes[i]->SetPosition( glm::vec3( 0.0f, 150.0f, 0.0f ) );
		m_nodes[i]->SetOutlineOnly();
		AddChild( m_nodes[i] );
	}*/

	m_fogOfWarMeshNode = DynamicModelNode::CreateWithMesh( NULL, NULL, GL_UNSIGNED_SHORT, 0, 0, VertexComponents::POSITION, glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3( 0.0f ), glm::vec3( 10.0f ) );
	m_fogOfWarMeshNode->SetOpacity( 0.7f );

	m_fogOfWarMeshNode->SetPassType( ( PassType::Value )( CustomRenderPasses::FOG_OF_WAR_PASS ) );
	AddChild( m_fogOfWarMeshNode );

	return true;
}
/*
============
FogOfWarNode::Update

	Updates the fog of war node
============
*/
void FogOfWarNode::Update( float dt ) {
	GameScene*	scene		= ( ( GameScene* )vgs::Engine::GetInstance()->GetRunningScene() );
	if ( !m_character ) {
		m_character	= scene->GetLocalCharacter();
	}

	if ( !scene->GetCurrentCamera() || scene->GetCurrentCamera()->GetRTTI() != FixedCamera::rtti ) {
		return;
	}

	if ( !scene->GetLevel() ) {
		return;
	}

	if ( m_character ) {

		//This will rarely be false, but why not?
		bool requiresUpdate = ( m_prevPosition != m_character->GetAbsoluteWorldPosition() ) || 
								( m_prevForward != m_character->GetTransform().GetForward() );

		if ( requiresUpdate ) {
		
			const Frustum&		frustum				= scene->GetCurrentCamera()->GetFrustum();		
			glm::vec3			playerPosition		= m_character->GetAbsoluteWorldPosition();		
			float				viewHeight			= m_character->GetViewHeight();
								playerPosition.y	+= viewHeight;

			float				viewAngle			= m_character->GetCharacterStats().FOVAngle;

			FixedCamera*		cam				= ( FixedCamera* )scene->GetCurrentCamera();
			const Frustum&		camFrustum		= cam->GetFrustum();
			float				distanceToLook	= glm::distance( cam->GetAbsoluteWorldPosition(), cam->GetLookAtPoint() );
			glm::vec2			frustumSize		= camFrustum.GetFrustumSizeAtDistance( distanceToLook );
			float				viewDistance	= glm::distance( glm::vec2( -frustumSize * 0.5f ), glm::vec2( frustumSize * 0.5f ) );

			//These are the edges of the view cone if nothing is in the way
			glm::vec3 forward = m_character->GetForward();			
			glm::vec4 viewCone[2];

			glm::vec3 toConePoint = glm::vec3( forward.x * cos( glm::radians( viewAngle * 0.5f ) ) - forward.z * sin( glm::radians( viewAngle * 0.5f ) ),
												0.0f,
												forward.z * cos( glm::radians( viewAngle * 0.5f ) ) + forward.x * sin( glm::radians( viewAngle * 0.5f ) ) );
			toConePoint *= viewDistance;
			viewCone[0] = glm::vec4( playerPosition + toConePoint, 1.0f );

			toConePoint = glm::vec3( forward.x * cos( glm::radians( -viewAngle * 0.5f ) ) - forward.z * sin( glm::radians( -viewAngle * 0.5f ) ),
									 0.0f,
									 forward.z * cos( glm::radians( -viewAngle * 0.5f ) ) + forward.x * sin( glm::radians( -viewAngle * 0.5f ) ) );
			toConePoint *= viewDistance;
			viewCone[1] = glm::vec4( playerPosition + toConePoint, 1.0f );

			//Get all the points to raycast
			//Shape ptr is the shape parent
			std::vector<std::pair<glm::vec4*, Shape*>> points;
				
			points.push_back( std::pair<glm::vec4*, Shape*>( &viewCone[0], NULL ) );
			points.push_back( std::pair<glm::vec4*, Shape*>( &viewCone[1], NULL ) );

			std::vector<Shape*> clippingShapes;

			GetShapesInView( clippingShapes );
			GetPointsInView( points, clippingShapes );
		
			MathHelper::polarCoordinateComparator.origin.x = playerPosition.x;
			MathHelper::polarCoordinateComparator.origin.y = playerPosition.z;
		
			toConePoint = MathHelper::Normalize( toConePoint );
			MathHelper::polarCoordinateComparator.direction = glm::normalize( glm::vec2( toConePoint.x * cos( -0.01f ) - toConePoint.z * sin( -0.01f ),
																						 toConePoint.z * cos( -0.01f ) + toConePoint.x * sin( -0.01f ) ) );
			//Remove duplicates and sort them.
			PrepLists( points, clippingShapes );

			std::vector<glm::vec3> fogMesh;

			UpdateFog( playerPosition, points, fogMesh, clippingShapes );
			
				
			//Remove duplicates them, sort the points clockwise.
			std::sort( fogMesh.begin(), fogMesh.end(), vectorSortComparator );
			fogMesh.erase( std::unique( fogMesh.begin(), fogMesh.end(), vectorEqualityComparator ), fogMesh.end() );
			std::sort( fogMesh.begin(), fogMesh.end(), MathHelper::polarCoordinateComparator );
			
			RemoveUnnecessaryVerticies( fogMesh );

			std::sort( fogMesh.begin(), fogMesh.end(), vectorSortComparator );
			fogMesh.erase( std::unique( fogMesh.begin(), fogMesh.end(), vectorEqualityComparator ), fogMesh.end() );
			std::sort( fogMesh.begin(), fogMesh.end(), MathHelper::polarCoordinateComparator );

			//Make the mesh
			UpdateMesh( fogMesh );

			m_prevForward  = m_character->GetTransform().GetForward();
			m_prevPosition = m_character->GetAbsoluteWorldPosition();
		}
	}

	Node::Update( dt );
}
/*
============
FogOfWarNode::PrepLists

	Removes duplicates and sorts the points clockwise
============
*/
void FogOfWarNode::PrepLists( std::vector<std::pair<glm::vec4*, Shape*>>& points, std::vector<Shape*>& shapes ) {
	std::sort( points.begin(), points.end(), vectorSortComparator );
	points.erase( std::unique( points.begin(), points.end(), vectorEqualityComparator ), points.end() );
		
	//Make sure there aren't any duplicate shapes
	std::sort( shapes.begin(), shapes.end(), shapeSortComparator );
	shapes.erase( std::unique( shapes.begin(), shapes.end(), shapeEqualityComparator ), shapes.end() );
}
/*
============
FogOfWarNode::GetPointsInView

	Gets all the points that are actually in the view
============
*/
void FogOfWarNode::GetPointsInView( std::vector<std::pair<glm::vec4*, Shape*>>& points, const std::vector<Shape*>& shapes ) {
	GameScene*			scene			= ( ( GameScene* )vgs::Engine::GetInstance()->GetRunningScene() );
	const Frustum&		frustrum		= scene->GetCurrentCamera()->GetFrustum();
	glm::vec3			playerPosition	= m_character->GetAbsoluteWorldPosition();
	glm::vec3			forward			= m_character->GetForward();
	
	FixedCamera*		cam				= ( FixedCamera* )scene->GetCurrentCamera();
	const Frustum&		camFrustum		= cam->GetFrustum();
	float				distanceToLook	= glm::distance( cam->GetAbsoluteWorldPosition(), cam->GetLookAtPoint() );
	glm::vec2			frustumSize		= camFrustum.GetFrustumSizeAtDistance( distanceToLook );

	float				viewDistance	= glm::distance( glm::vec2( -frustumSize * 0.5f ), glm::vec2( frustumSize * 0.5f ) );
	float				viewAngle		= m_character->GetCharacterStats().FOVAngle;	
						viewAngle		*= 0.5f;
	
	for( std::vector<Shape*>::const_iterator iter = shapes.begin();
		 iter != shapes.end(); ++iter ) {
		glm::vec4* shapePoints = ( *iter )->GetPoints();
		unsigned int halfPoints = ( unsigned int )( ( *iter )->GetPointCount() * 0.5f );

		for( unsigned int i = 0; i < halfPoints; ++i ) {
			//if ( frustrum.PointInFrustum( shapePoints[i] ) ) {
			if ( ( *iter )->GetRTTI() == BoxShape::rtti ) {
				glm::vec3 toPoint = glm::vec3( shapePoints[i].x, playerPosition.y, shapePoints[i].z );
				if ( glm::distance( playerPosition, toPoint ) < viewDistance ) {
					toPoint = toPoint - playerPosition;
				
					toPoint = glm::normalize( toPoint );

					if ( glm::degrees( acos( glm::dot( toPoint, forward ) ) ) < viewAngle ) {
						points.push_back( std::pair<glm::vec4*, Shape*>( &shapePoints[i], *iter ) );
					}
				}
			} else {
				glm::vec3 directionOnCircle = glm::vec3( shapePoints[i] ) - ( *iter )->GetPosition();
				directionOnCircle.y = 0.0f;
				directionOnCircle = glm::normalize( directionOnCircle );

				float dotOnCircle = glm::dot( directionOnCircle, forward );
				if ( dotOnCircle < FLOAT_EPSILON ) {
					glm::vec3 toPoint = glm::vec3( shapePoints[i].x, playerPosition.y, shapePoints[i].z );
					if ( glm::distance( playerPosition, toPoint ) < viewDistance ) {
						toPoint = toPoint - playerPosition;
				
						toPoint = glm::normalize( toPoint );

						if ( glm::degrees( acos( glm::dot( toPoint, forward ) ) ) < viewAngle ) {
							points.push_back( std::pair<glm::vec4*, Shape*>( &shapePoints[i], *iter ) );
						}
					}
				}
			}
			//}
		}
	}
}
/*
============
FogOfWarNode::GetShapesInView

	Gets all the shapes in the frustum
============
*/
void FogOfWarNode::GetShapesInView( std::vector<Shape*>& shapes ) {
	GameScene*									scene			= ( ( GameScene* )vgs::Engine::GetInstance()->GetRunningScene() );
	const Frustum&								frustum			= scene->GetCurrentCamera()->GetFrustum();
	const LevelNode*							levelNode		= scene->GetLevel();
	std::vector<TreeNode<ColliderComponent>*>	frustumNodes;
		
	glm::vec3									playerPosition			= m_character->GetAbsoluteWorldPosition();
	float										viewHeight				= m_character->GetViewHeight();
												playerPosition.y		+= viewHeight;


	PhysicsModule* physics = Engine::GetInstance()->GetModuleByType<PhysicsModule>(EngineModuleType::PHYSICS);
	physics->GetSpacePartitionTree()->CullFrustum(frustum, frustumNodes);

	BEGIN_STD_VECTOR_ITERATOR( TreeNode<ColliderComponent>*, frustumNodes )
		for( std::vector<std::pair<ColliderComponent*, Shape*>>::iterator shapeIter = currentItem->children.begin();
				shapeIter != currentItem->children.end(); ++shapeIter ) {
			if ( shapeIter->first->GetRTTI() == BoxCollider::rtti ) {
				if ( frustum.BoxInFrustum( *static_cast<BoxShape*>(shapeIter->second) ) ) {
					shapes.push_back( shapeIter->first->GetShape() );				
				}
			} else {
				if ( frustum.CylinderInFrustum( *static_cast<CylinderShape*>(shapeIter->second) ) ) {
					shapes.push_back( shapeIter->first->GetShape());
				}
			}
		}			
	END_STD_VECTOR_ITERATOR

	std::sort( shapes.begin(), shapes.end(), shapeSortComparator );
	shapes.erase( std::unique( shapes.begin(), shapes.end(), shapeEqualityComparator ), shapes.end() );
}
/*
============
FogOfWarNode::FindClosestPoint

	Finds the closest point in a list of points.
============
*/
unsigned int FogOfWarNode::FindClosestPoint( const glm::vec3& point, const std::vector<glm::vec3>& points ) {
	unsigned int closest = 0;
	float closestDistance = glm::distance( point, points[0] );
	for ( unsigned int i = 1; i < points.size(); ++i ) {
		float thisDist = glm::distance( point, points[i] );
		if ( thisDist < closestDistance ) {
			closestDistance = thisDist;
			closest = i;
		}
	}

	return closest;
}
/*
============
FogOfWarNode::UpdateFog

	Updates the actual fog mesh
============
*/
void FogOfWarNode::UpdateFog( const glm::vec3& playerPosition, const std::vector<std::pair<glm::vec4*, Shape*>>& points, std::vector<glm::vec3>& fogMesh, const std::vector<Shape*>& clippingShapes ) {	
	GameScene*			scene			= ( ( GameScene* )vgs::Engine::GetInstance()->GetRunningScene() );
	FixedCamera*		cam				= ( FixedCamera* )scene->GetCurrentCamera();
	const Frustum&		camFrustum		= cam->GetFrustum();
	float				distanceToLook	= glm::distance( cam->GetAbsoluteWorldPosition(), cam->GetLookAtPoint() );
	glm::vec2			frustumSize		= camFrustum.GetFrustumSizeAtDistance( distanceToLook );
	float				viewDistance	= glm::distance( glm::vec2( -frustumSize * 0.5f ), glm::vec2( frustumSize * 0.5f ) );

	RayCasting::Ray toCast;

	for ( std::vector<std::pair<glm::vec4*, Shape*>>::const_iterator point = points.begin(); 
		  point != points.end(); ++point ) {
	
		//Put the point level with player
		glm::vec3 leveledPoint( ( *point ).first->x, playerPosition.y, ( *point ).first->z );
		
		toCast.Origin = playerPosition;			
		toCast.Direction = glm::normalize( leveledPoint - playerPosition );		

		//Should we bother?
		//if ( ShouldCastToPoint( characterParent->GetForward(), toCast.Direction, characterParent->GetCharacterStats().FOVAngle ) ) {
			//Make sure we're not going further than the player can actually see.
			toCast.Length = viewDistance;		
			std::shared_ptr<RayCasting::RayCastResult<Shape>> ptr = RayCasting::RayCastFirst( clippingShapes, toCast.Origin, toCast.Direction, toCast.Length );

			//Raycast hit nothing. This point is visible, keep it for later.
			if (!ptr) {
				fogMesh.push_back( toCast.Origin + ( toCast.Direction * toCast.Length ) );
			} else { //Raycast hit something.
				//We hit something
				fogMesh.push_back( glm::vec3( ptr->Intersection.x, playerPosition.y, ptr->Intersection.z ) );

				if ( ptr->Object == ( *point ).second ) {
				//Check to see if we can see around the corner
					CheckForCorner( toCast, *ptr.get(), fogMesh, clippingShapes );
				}
			}
		//}
	}
}
/*
============
FogOfWarNode::CheckForCorner

	This will cast a ray just next to the orignal to see if the character can see around the corner
============
*/
void FogOfWarNode::CheckForCorner( const RayCasting::Ray& origRay, const RayCasting::RayCastResult<Shape>& rayResult, std::vector<glm::vec3>& meshDest, const std::vector<Shape*>& clippingShapes ) {
	GameScene*	scene				= ( ( GameScene* )vgs::Engine::GetInstance()->GetRunningScene() );
	float		viewAngle			= m_character->GetCharacterStats().FOVAngle;
	glm::vec3	forward				= m_character->GetForward();

	FixedCamera*	cam				= ( FixedCamera* )scene->GetCurrentCamera();
	const Frustum&	camFrustum		= cam->GetFrustum();
	float			distanceToLook	= glm::distance( cam->GetAbsoluteWorldPosition(), cam->GetLookAtPoint() );
	glm::vec2		frustumSize		= camFrustum.GetFrustumSizeAtDistance( distanceToLook );

	RayCasting::Ray toCast;
	toCast.Origin = origRay.Origin;

	toCast.Length = glm::distance( glm::vec2( -frustumSize * 0.5f ), glm::vec2( frustumSize * 0.5f ) );

	//Slightly in one direction
	toCast.Direction = glm::vec3( origRay.Direction.x * cos( 0.05f ) - origRay.Direction.z * sin( 0.05f ),
								  0.0f,
								  origRay.Direction.z * cos( 0.05f ) + origRay.Direction.x * sin( 0.05f ) );
	
	std::shared_ptr<RayCasting::RayCastResult<Shape>> ptr;
	if ( ShouldCastToPoint( forward, toCast.Direction, viewAngle ) ) {
		ptr = RayCasting::RayCastFirst( clippingShapes, toCast.Origin, toCast.Direction, toCast.Length );
		if ( ptr ) { //Hit something
			if ( glm::distance( ptr->Intersection, rayResult.Intersection ) > 5.0f ) { // Make sure the intersection point is not just not next to this one.
				meshDest.push_back( glm::vec3( ptr->Intersection.x, origRay.Origin.y, ptr->Intersection.z ) );
			}
		} else {
			meshDest.push_back( toCast.Origin + ( toCast.Direction * toCast.Length ) );
		}
	}

	//Slightly in the other.
	toCast.Direction = glm::vec3( origRay.Direction.x * cos( -0.05f ) - origRay.Direction.z * sin( -0.05f ),
								  0.0f,
								  origRay.Direction.z * cos( -0.05f ) + origRay.Direction.x * sin( -0.05f ) );
	
	if ( ShouldCastToPoint( forward, toCast.Direction, viewAngle ) ) {
		ptr = RayCasting::RayCastFirst( clippingShapes, toCast.Origin, toCast.Direction, toCast.Length );
		if ( ptr ) { //Hit something
			if ( glm::distance( ptr->Intersection, rayResult.Intersection ) > 5.0f ) { // Make sure the intersection point is not just not next to this one.
				meshDest.push_back( glm::vec3( ptr->Intersection.x, origRay.Origin.y, ptr->Intersection.z ) );
			}
		} else {
			meshDest.push_back( toCast.Origin + ( toCast.Direction * toCast.Length ) );
		}
	}
}
/*
============
FogOfWarNode::ShouldCastRay

	Returns if a ray should be cast in that direction.
============
*/
bool FogOfWarNode::ShouldCastToPoint( const glm::vec3& characterForward, const glm::vec3& direction, const float& angle ) {
	return glm::degrees( acos( glm::dot( direction, characterForward ) ) ) < ( angle * 0.5f );
}
/*
============
FogOfWarNode::RemoveUnnecessaryVerticies

	Removes unecessary verticies based on if they are on the same angle as the previous 2.
============
*/
void FogOfWarNode::RemoveUnnecessaryVerticies( std::vector<glm::vec3>& verticies ) {
	std::vector<glm::vec3>::iterator startingPoint	= verticies.begin();
	std::vector<glm::vec3>::iterator point			= verticies.begin() + 1;

	glm::vec3 startingDirection = glm::normalize( ( *point ) - ( *startingPoint ) );
	
	++point;

	glm::vec3 compareDirection;

	while ( point != verticies.end() ) {
		compareDirection = MathHelper::Normalize( ( *point ) - *( point - 1 ) );

		if ( glm::dot( compareDirection, startingDirection ) > 0.99f ) {
			*( point - 1 ) = ( *startingPoint );
			++point;
		} else {
			startingPoint = point - 1;
			startingDirection = MathHelper::Normalize( ( *point ) - ( *startingPoint ) );
			++point;
		}
	}
}
/*
============
FogOfWarNode::UpdateMesh

	Updates the actual mesh
============
*/
void FogOfWarNode::UpdateMesh( const std::vector<glm::vec3>& points ) {
	GameScene*			scene					= ( ( GameScene* )vgs::Engine::GetInstance()->GetRunningScene() );
	glm::vec3			playerPosition			= m_character->GetAbsoluteWorldPosition();
	float				viewHeight				= m_character->GetViewHeight();
						playerPosition.y		+= viewHeight;


	unsigned int vertexCount = points.size() + + 1; //Points + player
	unsigned int indexCount = ( points.size() - 1 ) * 3;

	float* verticies = new float[vertexCount * 3];
	unsigned short* indicies = new unsigned short[indexCount];

	unsigned int vertexDataOffset = 0;
	unsigned int indexDataOffset = 0;

	memcpy( &verticies[vertexDataOffset], &glm::vec3( 0.0f ), sizeof( float ) * 3 );
	vertexDataOffset += 3;

	glm::vec3 minVert;
	glm::vec3 maxVert;

	unsigned int count = 1;
	for( std::vector<glm::vec3>::const_iterator iter = points.begin();
		 iter != points.end(); ++iter ) {
		memcpy( &verticies[vertexDataOffset], &( ( *iter ) - playerPosition ), sizeof( float ) * 3 );
		vertexDataOffset += 3;

		if ( ( *iter ).x < minVert.x ) {
			minVert.x = ( *iter ).x;
		} else if ( ( *iter ).x > maxVert.x ) {
			minVert.x = ( *iter ).x;
		}

		if ( ( *iter ).y < minVert.y ) {
			minVert.y = ( *iter ).y;
		} else if ( ( *iter ).y < maxVert.y ) {
			minVert.y = ( *iter ).y;
		}

		if ( ( *iter ).z < minVert.z ) {
			minVert.z = ( *iter ).z;
		} else if ( ( *iter ).z < maxVert.z ) {
			minVert.z = ( *iter ).z;
		}

		if ( count > 1 ) {
			indicies[indexDataOffset++] = 0;
			indicies[indexDataOffset++] = count - 1;
			indicies[indexDataOffset++] = count;
		}

		++count;
	}

	m_fogOfWarMeshNode->UpdateMesh( verticies, indicies, vertexCount, indexCount, ( maxVert - minVert ), ( maxVert + minVert ) * 0.5f );
	m_fogOfWarMeshNode->SetPosition( playerPosition - glm::vec3( 0.0f, 5.0f, 0.0f ) );
	//std::cout << "fow update: (id" << m_id << ") " << std::setprecision(16) << RakNet::GetTime() * 0.001 << std::endl;
}
}