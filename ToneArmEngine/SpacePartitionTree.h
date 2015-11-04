/*
========================

Copyright (c) 2014 Vinyl Games Studio

	GenericSpacePartitionTree

		A space partition tree.
		Templated so that it can store different object types.

		Created by: Karl Merten
		Created on: 07/07/2014

========================
*/
#ifndef __SPACEPARTITIONTREE_H__
#define __SPACEPARTITIONTREE_H__

#include <cstdlib>
#include <vector>
#include <set>
#include <glm\glm.hpp>

#include "BoxCollider.h"
#include "CylinderCollider.h"

#include "CollisionHelper.h"
#include "Frustum.h"

namespace vgs {

template <typename T>
struct TreeNode {
	typedef Shape& ( T::*BoundingDimensionsFuncPointer )( void );

	BoxShape										boundingBox;
	std::vector<TreeNode*>							treeNodes;
	TreeNode*										parent;
	std::vector<std::pair<T*, Shape*>>				children;

	TreeNode( BoundingDimensionsFuncPointer funcPtr ) :
		parent( NULL ),
		m_boundingDataFunc( funcPtr )
	{}

	~TreeNode( void ) {
		DELETE_STD_VECTOR_POINTER( TreeNode, treeNodes );
	}

	void								Insert( T* object, Shape* boundingShape, unsigned int subdivisions, unsigned int maxObjectsInLeaf );
	bool								Contains( T* object );	
	void								GetObjects( std::vector<T*>& dest );
	void								GetObjects( std::set<T*>& dest );
private:
	BoundingDimensionsFuncPointer		m_boundingDataFunc;
};

template <typename T>
class SpacePartitionTree {
	typedef Shape& ( T::*BoundingDimensionsFuncPointer )( void );

public:
										~SpacePartitionTree( void );

	static SpacePartitionTree<T>*		CreateEmptyTree( BoundingDimensionsFuncPointer dataFunc, unsigned int subdivisions = 4, unsigned int maxObjectsPerLeaf = 20 );
	static SpacePartitionTree<T>*		CreateSpacePartitionTree( const std::vector<T*>& objects, BoundingDimensionsFuncPointer dataFunc, unsigned int subdivisions = 4, unsigned int maxObjectsPerLeaf = 20 );
	const TreeNode<T>* const			GetRootNode( void ) const { return m_rootNode; }
	void								GetLeafNodes( std::vector<TreeNode<T>* const>& dest ) const;

	void								CullFrustum( const Frustum& frustum, std::vector<TreeNode<T>*>& destination );
	void								CullCylinder( const CylinderShape& cylinder, std::vector<TreeNode<T>*>& destination );
	void								CullBox( const BoxShape& cube, std::vector<TreeNode<T>*>& destination );

	void								InsertObject( T* obj );
private:
										SpacePartitionTree( void );
	bool								InitializeTree( const std::vector<T*>& objects, BoundingDimensionsFuncPointer dataFunc, unsigned int subdivisions, unsigned int maxObjectsPerLeaf );
	bool								InitializeTree( BoundingDimensionsFuncPointer dataFunc, unsigned int subdivisions, unsigned int maxObjectsPerLeaf );
	
	void								CalculateBoundingBox( const std::vector<T*>& objects, BoundingDimensionsFuncPointer dataFunc, BoxShape& dest );
	
	void								GetLeafNodes( TreeNode<T>* const curNode, std::vector<TreeNode<T>* const>& dest ) const;

	void								CullFrustum( const Frustum& frustum, TreeNode<T>* const curNode, std::vector<TreeNode<T>*>& destination );
	void								CullBox( const BoxShape& frustum, TreeNode<T>* const curNode, std::vector<TreeNode<T>*>& destination );

	BoundingDimensionsFuncPointer		m_boundingDataFunc;
	TreeNode<T>*						m_rootNode;

	unsigned int						m_subdivisions;
	unsigned int						m_maxObjectsPerLeaf;
};
/*
==========
SpacePartitionTree::SpacePartitionTree

	SpacePartitionTree default constructor
==========
*/
template <typename T>
SpacePartitionTree<T>::SpacePartitionTree( void ) :
	m_rootNode( NULL ),
	m_boundingDataFunc( NULL ),
	m_maxObjectsPerLeaf( 0 ),
	m_subdivisions( 0 )
{}
/*
==========
SpacePartitionTree::SpacePartitionTree

	SpacePartitionTree destructor
==========
*/
template <typename T>
SpacePartitionTree<T>::~SpacePartitionTree( void ) {
	/*std::set<Shape*> shapes;
	std::vector<TreeNode<T>* const> leaves;
	GetLeafNodes( leaves );

	BEGIN_STD_VECTOR_ITERATOR( TreeNode<T>* const, leaves )
		for ( std::vector<std::pair<T*, Shape*>>::iterator iter = currentItem->children.begin();
			  iter != currentItem->children.end(); ++iter ) {
			  shapes.insert( iter->second );
		}
	END_STD_VECTOR_ITERATOR

	for ( std::set<ColliderComponent*>::iterator iter = shapes.begin();
		  iter != shapes.end(); ++iter ) {
		delete ( *iter );
	}*/

	delete m_rootNode;
}
/*
==========
SpacePartitionTree::CreateSpacePartitionTree

	Creates a SpacePartitionTree from objects.
	A function pointer is required pointing to a function returning the bounding shape for the object.
	Number of sections before splitting, and max nodes in a branch can be passed in as well.
	Defaults are 4 sections, and 20 nodes before a split.
==========
*/
template <typename T>
SpacePartitionTree<T>* SpacePartitionTree<T>::CreateSpacePartitionTree( const std::vector<T*>& objects, BoundingDimensionsFuncPointer dataFunc, unsigned int subdivisions, unsigned int maxObjectsPerLeaf ) {
	SpacePartitionTree<T>* tree = new SpacePartitionTree<T>();
	if ( tree && tree->InitializeTree( objects, dataFunc, subdivisions, maxObjectsPerLeaf ) ) {
		return tree;
	} else {
		delete tree;
		return NULL;
	}
}
/*
==========
SpacePartitionTree::CreateEmptyTree

	Creates a SpacePartitionTree that is empty.
	A function pointer is required pointing to a function returning the bounding shape for the object.
	Number of sections before splitting, and max nodes in a branch can be passed in as well.
	Defaults are 4 sections, and 20 nodes before a split.
==========
*/
template <typename T>
SpacePartitionTree<T>* SpacePartitionTree<T>::CreateEmptyTree( BoundingDimensionsFuncPointer dataFunc, unsigned int subdivisions, unsigned int maxObjectsPerLeaf ) {
	SpacePartitionTree<T>* tree = new SpacePartitionTree<T>();
	if ( tree && tree->InitializeTree( dataFunc, subdivisions, maxObjectsPerLeaf ) ) {
		return tree;
	} else {
		delete tree;
		return NULL;
	}
}
/*
==========
SpacePartitionTree::InitializeTree

	Initializes a SpacePartitionTree with the objects.
==========
*/
template <typename T>
bool SpacePartitionTree<T>::InitializeTree( const std::vector<T*>& objects, BoundingDimensionsFuncPointer dataFunc, unsigned int subdivisions, unsigned int maxObjectsPerLeaf ) {
	m_rootNode = new TreeNode<T>( dataFunc );
	m_boundingDataFunc = dataFunc;
	m_subdivisions = subdivisions;
	m_maxObjectsPerLeaf = maxObjectsPerLeaf;
	
	CalculateBoundingBox( objects, dataFunc, m_rootNode->boundingBox );

	for( std::vector<T*>::const_iterator iter = objects.begin();
		 iter != objects.end(); ++iter ) {
		////They need to be duplicated because the collider owns these shapes, but we need them too.
		//Shape* boundingShape = NULL;
		//if ( ( *iter )->GetRTTI() == BoxShape::rtti ) {
		//	BoxShape& orig = ( BoxShape& )( dataFunc ? ( ( *iter )->*dataFunc )() : *( *iter ) );
		//	boundingShape = BoxShape::CreateBoxShape( orig.GetDimensions() );
		//	boundingShape->SetPosition( orig.GetImmutableTransform().GetPosition() );
		//	boundingShape->SetRotation( orig.GetImmutableTransform().GetRotation() );
		//} else {
		//	CylinderShape& orig = ( CylinderShape& )( dataFunc ? ( ( *iter )->*dataFunc )() : *( *iter ) );
		//	boundingShape = CylinderShape::CreateCylinderShape( orig.GetRadius(), orig.GetHeight(), orig.GetSubdivisions() );
		//	boundingShape->SetPosition( orig.GetImmutableTransform().GetPosition() );
		//}

		m_rootNode->Insert( ( *iter ), ( *iter )->GetShape(), subdivisions, maxObjectsPerLeaf );
	}

	return true;
}
/*
==========
SpacePartitionTree::InsertObject

	Adds an object to the tree
==========
*/
template <typename T>
void SpacePartitionTree<T>::InsertObject( T* obj ) {
	if ( m_rootNode->treeNodes.size() == 0 && m_rootNode->children.size() == 0 ) {
		m_rootNode->boundingBox.SetDimensions( ( obj->( *m_boundingDataFunc ) )().GetDimensions() );
		m_rootNode->boundingBox.SetPosition( ( obj->( *m_boundingDataFunc ) )().GetPosition() );
	} else {
		if ( CollisionHelper::BoxBoxCollisionTest( ( obj->( *m_boundingDataFunc ) )(), m_rootNode->boundingBox ) ) {
			m_rootNode->Insert( obj, ( obj->( *m_boundingDataFunc ) )(), m_subdivisions, m_maxObjectsPerLeaf );
		} else {

		}
	}
}
/*
==========
SpacePartitionTree::InitializeTree

	Initializes a SpacePartitionTree with the objects.
==========
*/
template <typename T>
bool SpacePartitionTree<T>::InitializeTree( BoundingDimensionsFuncPointer dataFunc, unsigned int subdivisions, unsigned int maxObjectsPerLeaf ) {
	m_rootNode = new TreeNode<T>( dataFunc );
	m_boundingDataFunc = dataFunc;

	m_subdivisions = subdivisions;
	m_maxObjectsPerLeaf = maxObjectsPerLeaf;
	
	return true;
}
/*
==========
SpacePartitionTree::CalculateBoundingBoxForColliders

	Calculates the bounding box for all the objects.
	Will find proper size for colliders that are rotated.
==========
*/
template <typename T>
void SpacePartitionTree<T>::CalculateBoundingBox( const std::vector<T*>& objects, BoundingDimensionsFuncPointer dataFunc, BoxShape& dest ) {
	glm::vec3	minBounds( 0.0f );
	glm::vec3	maxBounds( 0.0f );
	glm::vec3	boxDimensions( 0.0f );

	for( std::vector<T*>::const_iterator iter = objects.begin();
		 iter != objects.end(); ++iter ) {

		const Shape& boundingShape = dataFunc ? ( ( *iter )->*dataFunc )() : *( *iter )->GetShape();
		const glm::vec3& colliderPosition = (*iter)->GetPosition();

		if ( boundingShape.GetRTTI() == BoxShape::rtti ) {
			BoxShape*		boxCollider		= ( ( BoxShape* ) &boundingShape );
			float			rotation		= boxCollider->GetRotation().y;
							boxDimensions	= boxCollider->GetDimensions();

			//Find the new bounding box size, if the collider is rotated.
			if ( abs( rotation ) > 0.01f ) {
				boxDimensions = MathHelper::GetAABBDimensions( *boxCollider );
			}
		} else {
			boxDimensions = boundingShape.GetBoundingDimensions();
		}
		
		float minX = colliderPosition.x - ( boxDimensions.x * 0.5f );
		float maxX = colliderPosition.x + ( boxDimensions.x * 0.5f );

		float minY = colliderPosition.y - ( boxDimensions.y * 0.5f );
		float maxY = colliderPosition.y + ( boxDimensions.y * 0.5f );

		float minZ = colliderPosition.z - ( boxDimensions.z * 0.5f );
		float maxZ = colliderPosition.z + ( boxDimensions.z * 0.5f );

		//Update new mins/maxes
		if ( minX < minBounds.x ) {
			minBounds.x = minX;
		}
		if ( maxX > maxBounds.x ) {
			maxBounds.x = maxX;
		}

		if ( minY < minBounds.y ) {
			minBounds.y = minY;
		}
		if ( maxY > maxBounds.y ) {
			maxBounds.y = maxY;
		}

		if ( minZ < minBounds.z ) {
			minBounds.z = minZ;
		}
		if ( maxZ > maxBounds.z ) {
			maxBounds.z = maxZ;
		}
	}

	glm::vec3 boundingBoxDimensions( maxBounds.x - minBounds.x, maxBounds.y - minBounds.y, maxBounds.z - minBounds.z );
	
	//Set the bounding box size/position
	dest.SetPosition( glm::vec3( ( minBounds.x + maxBounds.x ) * 0.5f,
								 ( minBounds.y + maxBounds.y ) * 0.5f,
								 ( minBounds.z + maxBounds.z ) * 0.5f ) );
	dest.SetDimensions( boundingBoxDimensions );
}
/*
==========
SpacePartitionTree::GetLeafNodes

	Gets all of the nodes that have colliders in them
==========
*/
template <typename T>
void SpacePartitionTree<T>::GetLeafNodes( std::vector<TreeNode<T>* const>& dest ) const {
	GetLeafNodes( m_rootNode, dest );
}
/*
==========
SpacePartitionTree::GetLeafNodes

	Gets all of the nodes that have colliders in them
==========
*/
template <typename T>
void SpacePartitionTree<T>::GetLeafNodes( TreeNode<T>* const curNode, std::vector<TreeNode<T>* const>& dest ) const {
	if ( curNode->treeNodes.size() == 0 ) {
		dest.push_back( curNode );
	} else {
		for( std::vector<TreeNode<T>*>::const_iterator iter = curNode->treeNodes.begin();
			 iter != curNode->treeNodes.end(); ++iter ) {
			GetLeafNodes( *iter, dest );
		}
	}
}
/*
==========
TreeNode::InsertColliders

	Inserts the colliders into the node.
==========
*/
template <typename T>
void TreeNode<T>::Insert( T* object, Shape* boundingShape, unsigned int subdivisions, unsigned int maxCollidersInLeaf ) {
	if ( Contains( object ) ) {//Collider is in this section
		if ( treeNodes.size() == 0 ) { //This is a leaf without branches
			children.push_back( std::pair<T*, Shape*>( object, boundingShape ) );		
			if ( children.size() > maxCollidersInLeaf ) {
				//TreeNode** nodes  = new TreeNode*[subdivisions];
				glm::vec3 nodeDimensions;

				nodeDimensions.x = boundingBox.GetDimensions().x * 0.5f;
				nodeDimensions.z = boundingBox.GetDimensions().z;
				nodeDimensions.y = boundingBox.GetDimensions().y;

				if ( subdivisions > 2 ) {
					nodeDimensions.z *= 0.5f;
				}

				if ( subdivisions > 4 ) {
					nodeDimensions.y *= 0.5f;
				}

				glm::vec3 dimensionDifference( 0.0f );
				dimensionDifference.x = ( boundingBox.GetDimensions().x - nodeDimensions.x );
				dimensionDifference.y = ( boundingBox.GetDimensions().y - nodeDimensions.y );
				dimensionDifference.z = ( boundingBox.GetDimensions().z - nodeDimensions.z );

				dimensionDifference *= 0.5f;

				const glm::vec3& parentPosition = boundingBox.GetPosition();

				for ( unsigned int i = 0; i < subdivisions; ++i ) {
					TreeNode<T>* node = new TreeNode<T>( m_boundingDataFunc );
					glm::vec3 pos = boundingBox.GetPosition();

					if ( IS_EVEN( unsigned int, i ) ) {
						pos.x -= dimensionDifference.x;
					} else {
						pos.x += dimensionDifference.x;
					}

					if ( i < 4 ) {
						pos.y -= dimensionDifference.y;
					} else {
						pos.y += dimensionDifference.y;
					}

					if ( i % 4 < 2 ) {
						pos.z -= dimensionDifference.z;
					} else {
						pos.z += dimensionDifference.z;
					}

					node->boundingBox.SetDimensions( nodeDimensions );
					node->boundingBox.SetPosition( pos );
					node->parent = this;

					treeNodes.push_back( node );
				}

				for ( std::vector<std::pair<T*,Shape*>>::iterator colIter = children.begin();
					  colIter != children.end(); ++colIter ) {					
					for( std::vector<TreeNode<T>*>::iterator treeIter = treeNodes.begin();
						 treeIter != treeNodes.end(); ++treeIter ) {
						( *treeIter )->Insert( colIter->first, colIter->second, subdivisions, maxCollidersInLeaf );
					}
				}

				children.clear();
			}
		} else { //Insert it into a branch
			for( std::vector<TreeNode<T>*>::iterator iter = treeNodes.begin();
				 iter != treeNodes.end(); ++iter ) {
				( *iter )->Insert( object, boundingShape, subdivisions, maxCollidersInLeaf );
			}
		}
	}
}
/*
==========
TreeNode::ContainsCollider

	Returns if the collider is in the node.
==========
*/
template <typename T>
bool TreeNode<T>::Contains( T* object ) {
	Shape& collider = ( m_boundingDataFunc ? ( object->*m_boundingDataFunc )() : *object->GetShape() );

	if ( collider.GetRTTI() == BoxShape::rtti ) {
		const BoxShape& box = ( BoxShape& )collider;
		glm::vec3 boxDimensions = box.GetDimensions();
		if ( abs( box.GetRotation().y ) > 0.01f ) {
			boxDimensions = MathHelper::GetAABBDimensions( box );
		}

		if ( box.GetPosition().x + ( boxDimensions.x * 0.5f ) < boundingBox.GetPosition().x - ( boundingBox.GetDimensions().x * 0.5f ) ) return false; // collider is left of node
		if ( box.GetPosition().x - ( boxDimensions.x * 0.5f ) > boundingBox.GetPosition().x + ( boundingBox.GetDimensions().x * 0.5f ) ) return false; // collider is right of node
		if ( box.GetPosition().z + ( boxDimensions.z * 0.5f ) < boundingBox.GetPosition().z - ( boundingBox.GetDimensions().z * 0.5f ) ) return false; // collider is in front of node
		if ( box.GetPosition().z - ( boxDimensions.z * 0.5f ) > boundingBox.GetPosition().z + ( boundingBox.GetDimensions().z * 0.5f ) ) return false; // collider is behind of node
		
		return true;
	} else {
		const CylinderShape& cylinder = ( CylinderShape& )collider;

		float distanceX = abs( cylinder.GetPosition().x - boundingBox.GetPosition().x );
		float distanceZ = abs( cylinder.GetPosition().z - boundingBox.GetPosition().z );

		if ( distanceX > ( boundingBox.GetDimensions().x * 0.5f + cylinder.GetRadius() ) ) { return false; }
		if ( distanceZ > ( boundingBox.GetDimensions().z * 0.5f + cylinder.GetRadius() ) ) { return false; }

		if ( distanceX <= ( boundingBox.GetDimensions().x * 0.5f ) ) { return true; } 
		if ( distanceZ <= ( boundingBox.GetDimensions().z * 0.5f ) ) { return true; }

		float cornerDistance_sq = pow( ( distanceX - boundingBox.GetDimensions().x * 0.5f ), 2 ) +
								  pow( ( distanceZ - boundingBox.GetDimensions().z * 0.5f ), 2 );

		return ( cornerDistance_sq <= pow( cylinder.GetRadius(), 2) );
	}
}
/*
==========
SpacePartitionTree::CullBox

	Culls the octree to a box.
	Adds the relevant nodes into the vector passed in
==========
*/
template <typename T>
void SpacePartitionTree<T>::CullBox( const BoxShape& box, std::vector<TreeNode<T>*>& dest ) {
	CullBox( box, m_rootNode, dest );
}
/*
==========
SpacePartitionTree::CullBox

	Culls the octree to a box.
	Adds the relevant nodes into the vector passed in
==========
*/
template <typename T>
void SpacePartitionTree<T>::CullBox( const BoxShape& box, TreeNode<T>* const curNode, std::vector<TreeNode<T>*>& dest ) {
	if ( CollisionHelper::BoxBoxCollisionTest( box, curNode->boundingBox ) ) {
		if ( curNode->treeNodes.size() == 0 ) {
			dest.push_back( curNode );
		} else {
			for ( std::vector<TreeNode<T>*>::const_iterator iter = curNode->treeNodes.begin();
				  iter != curNode->treeNodes.end(); ++iter ) {
				CullBox( box, *iter, dest );
			}
		}
	}
}
/*
==========
SpacePartitionTree::CullCylinder

	Culls the octree to a cylinder.
	Adds the relevant nodes into the vector passed in
==========
*/
template <typename T>
void SpacePartitionTree<T>::CullCylinder( const CylinderShape& cyl, std::vector<TreeNode<T>*>& dest ) {
	BoxShape* bounding = BoxShape::CreateFromCylinder( cyl );
	CullBox( *bounding , m_rootNode, dest );
	delete bounding;
}
/*
==========
SpacePartitionTree::CullFrustum

	Culls the octree to a frustum.
	Adds the relevant nodes into the vector passed in
==========
*/
template <typename T>
void SpacePartitionTree<T>::CullFrustum( const Frustum& frustum, std::vector<TreeNode<T>*>& dest ) {
	CullFrustum( frustum, m_rootNode, dest );
}
/*
==========
SpacePartitionTree::CullFrustum

	Culls the octree to a frustum.
	Adds the relevant nodes into the vector passed in
==========
*/
template <typename T>
void SpacePartitionTree<T>::CullFrustum( const Frustum& frustum, TreeNode<T>* const curNode, std::vector<TreeNode<T>*>& dest ) {
	if ( frustum.BoxInFrustum( curNode->boundingBox ) != FrustumResult::OUTSIDE ) {
		if ( curNode->treeNodes.size() == 0 ) {
			dest.push_back( curNode );
		} else {
			for ( std::vector<TreeNode<T>*>::const_iterator iter = curNode->treeNodes.begin();
				  iter != curNode->treeNodes.end(); ++iter ) {
				CullFrustum( frustum, *iter, dest );
			}
		}
	}
}
/*
==========
TreeNode::GetObjects

	Gets the objects from the tree node and adds them to the passed in vector.
==========
*/
template <typename T>
void TreeNode<T>::GetObjects( std::vector<T*>& dest ) {
	for( std::vector<std::pair<T*, Shape*>>::iterator iter = children.begin();
		 iter != children.end(); ++iter ) {
		dest.push_back( iter->first );
	}
}
/*
==========
TreeNode::GetObjects

	Gets the objects from the tree node and adds them to the passed in set.
==========
*/
template <typename T>
void TreeNode<T>::GetObjects( std::set<T*>& dest ) {
	for( std::vector<std::pair<T*, Shape*>>::iterator iter = children.begin();
		 iter != children.end(); ++iter ) {
		dest.insert( iter->first );
	}
}
}

#endif //__TSPACEPARTITIONTREE_H__