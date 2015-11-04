/*
========================

Copyright (c) 2014 Vinyl Games Studio

	FogOfWarNode

		A node to show the fog of war.

		Based on:
		http://ncase.me/sight-and-light/

		Created by: Karl Merten
		Created on: 04/06/2014

========================
*/

#include "../ToneArmEngine/DynamicModelNode.h"
#include "../ToneArmEngine/Shape.h"
#include "../ToneArmEngine/RayCasting.h"
#include "../ToneArmEngine/Ray.h"

#include <utility>

namespace merrymen {
	class Character;

class FogOfWarNode : public vgs::Node {								
public:
								CREATE_METH( FogOfWarNode )
	virtual void				Update( float dt );
private:
								FogOfWarNode( void );
	bool						Init( void );

	void						UpdateFog( const glm::vec3& playerPosition, const std::vector<std::pair<glm::vec4*, vgs::Shape*>>& points, std::vector<glm::vec3>& fogMesh, const std::vector<vgs::Shape*>& clippingShapes );	
	unsigned int				FindClosestPoint( const glm::vec3& point, const std::vector<glm::vec3>& points );

	void						GetShapesInView( std::vector<vgs::Shape*>& shapes );
	void						GetPointsInView( std::vector<std::pair<glm::vec4*, vgs::Shape*>>& points, const std::vector<vgs::Shape*>& shapes );
	void						PrepLists( std::vector<std::pair<glm::vec4*, vgs::Shape*>>& points, std::vector<vgs::Shape*>& shapes );
	
	void						RemoveUnnecessaryVerticies( std::vector<glm::vec3>& points );
	void						UpdateMesh( const std::vector<glm::vec3>& points );

	bool						ShouldCastToPoint( const glm::vec3& characterForward, const glm::vec3& direction, const float& viewAngle );
	void						CheckForCorner( const vgs::RayCasting::Ray& origRay, const vgs::RayCasting::RayCastResult<vgs::Shape>& rayResult, std::vector<glm::vec3>& fogMesh, const std::vector<vgs::Shape*>& clippingShapes );

	glm::vec3					m_prevPosition;
	glm::vec3					m_prevForward;

	//vgs::DynamicModelNode*		m_nodes[50];
	vgs::DynamicModelNode*		m_fogOfWarMeshNode;
	Character*					m_character;

	struct GLMVecEquality {
		bool operator()( glm::vec4* a, glm::vec4* b ) {
			return ( abs( a->x - b->x ) < FLOAT_EPSILON &&
				     abs( a->y - b->y ) < FLOAT_EPSILON &&
					 abs( a->z - b->z ) < FLOAT_EPSILON &&
					 abs( a->w - b->w ) < FLOAT_EPSILON );
		}

		bool operator()( const glm::vec3* const a, const glm::vec3* const b ) {
			return ( *this )( *a, *b );
		}

		bool operator()( const glm::vec3& a, const glm::vec3& b ) {
			return ( abs( a.x - b.x ) < FLOAT_EPSILON &&
				     abs( a.y - b.y ) < FLOAT_EPSILON &&
					 abs( a.z - b.z ) < FLOAT_EPSILON );
		}

		bool operator()( const std::pair<glm::vec4*, vgs::Shape*>& a, const std::pair<glm::vec4*, vgs::Shape*>& b ) {
			return ( *this )( a.first, b.first );
		}

	} vectorEqualityComparator;

	struct GLMVec4Sort {
		bool operator()( glm::vec4* a, glm::vec4* b ) {
			if ( abs( a->x - b->x ) > FLOAT_EPSILON ) {
				return a->x < b->x;
			} else if ( abs( a->y - b->y ) > FLOAT_EPSILON ) {
				return a->y < b->y;
			} else if ( abs( a->z - b->z ) > FLOAT_EPSILON ) {
				return a->z < b->z;
			} else {
				return a->w < b->w;
			}
		}

		bool operator()( const glm::vec3& a, const glm::vec3& b ) {
			if ( abs( a.x -b.x ) > FLOAT_EPSILON ) {
				return a.x < b.x;
			} else if ( abs( a.y - b.y ) > FLOAT_EPSILON ) {
				return a.y < b.y;
			} else {
				return a.z < b.z;
			}
		}

		bool operator()( const glm::vec3* a, const glm::vec3* b ) {
			if ( abs( a->x -b->x ) > FLOAT_EPSILON ) {
				return a->x < b->x;
			} else if ( abs( a->y - b->y ) > FLOAT_EPSILON ) {
				return a->y < b->y;
			} else {
				return a->z < b->z;
			}
		}

		bool operator()( const std::pair<glm::vec4*, vgs::Shape*>& a, const std::pair<glm::vec4*, vgs::Shape*>& b ) {
			return ( *this )( a.first, b.first );
		}

	} vectorSortComparator;

	struct ShapeEqualityComparator {
		bool operator()( vgs::Shape* shapeA, vgs::Shape* shapeB ) {
			return ( *shapeA == *shapeB );
		}
	} shapeEqualityComparator;

	struct ShapeSortComparator {
		bool operator()( vgs::Shape* shapeA, vgs::Shape* shapeB ) {
			return ( *shapeA < *shapeB );
		}
	} shapeSortComparator;
};

}