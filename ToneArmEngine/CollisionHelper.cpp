/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/27/2014 10:31:10 AM
------------------------------------------------------------------------------------------
*/

#include "CollisionHelper.h"
#include "CylinderShape.h"
#include "BoxShape.h"

using namespace vgs;

//
// checks if a cylinder collides with a box (3D space) and, if so, returns a set of the line segments corresponding to each side of the box,
// that the cylinder collides with
//
std::vector<std::shared_ptr<MathHelper::LineSegment>> CollisionHelper::CylinderBoxCollisionTest(const CylinderShape& cylinder, const BoxShape& box) {

	// first of all check if vertical coordinates of the two colliders overlap
	std::vector<std::shared_ptr<MathHelper::LineSegment>> result = std::vector<std::shared_ptr<MathHelper::LineSegment>>();

	glm::vec3 cylinderPosition = cylinder.GetPosition();
	glm::vec3 boxPosition = box.GetPosition();

	if (cylinderPosition.y - cylinder.GetHeight() / 2 >= boxPosition.y - box.GetDimensions().y / 2 &&
		cylinderPosition.y - cylinder.GetHeight() / 2 <= boxPosition.y + box.GetDimensions().y / 2 ||
		cylinderPosition.y + cylinder.GetHeight() / 2 >= boxPosition.y - box.GetDimensions().y / 2 &&
		cylinderPosition.y + cylinder.GetHeight() / 2 <= boxPosition.y + box.GetDimensions().y / 2 ||
		cylinderPosition.y - cylinder.GetHeight() / 2 <= boxPosition.y - box.GetDimensions().y / 2 &&
		cylinderPosition.y + cylinder.GetHeight() / 2 >= boxPosition.y - box.GetDimensions().y / 2) {

			// vertical coordinates overlap and collision check can be simplified to a 2D circle-rectangle check
			result = MathHelper::CircleRectangleIntersectionCheck2D(MathHelper::Circle(cylinderPosition, cylinder.GetRadius()),
																	MathHelper::RectF(boxPosition, box.GetDimensions().x, box.GetDimensions().z, -box.GetRotation().y));
	}

	return result;
}

//
// check if two cylinders in 3D space collide with each other
//
bool CollisionHelper::CylinderCylinderCollisionTest( const CylinderShape& cylinder1, const CylinderShape& cylinder2 ) {

	glm::vec3 cylinder1Position = cylinder1.GetPosition();
	glm::vec3 cylinder2Position = cylinder2.GetPosition();

	// first of all check if vertical coordinates of the two colliders overlap
	if (cylinder1Position.y - cylinder1.GetHeight() / 2 >= cylinder2Position.y - cylinder2.GetHeight() / 2 &&
		cylinder1Position.y - cylinder1.GetHeight() / 2 <= cylinder2Position.y + cylinder2.GetHeight() / 2 ||
		cylinder1Position.y + cylinder1.GetHeight() / 2 >= cylinder2Position.y - cylinder2.GetHeight() / 2 &&
		cylinder1Position.y + cylinder1.GetHeight() / 2 <= cylinder2Position.y + cylinder2.GetHeight() / 2 ||
		cylinder2Position.y - cylinder2.GetHeight() / 2 >= cylinder1Position.y - cylinder1.GetHeight() / 2 &&
		cylinder2Position.y - cylinder2.GetHeight() / 2 <= cylinder1Position.y + cylinder1.GetHeight() / 2) {

			// vertical coordinates overlap and collision check can be simplified to a 2D circle-circle check
			return MathHelper::CircleCircleIntersectionCheck2D(MathHelper::Circle(cylinder1Position, cylinder1.GetRadius()),
															MathHelper::Circle(cylinder2Position, cylinder2.GetRadius()));
	}

	return false;
}
/*
=============
CollisionHelper::BoxBoxCollisionTest

	Checks if 2 AABBs intersect in 3D.
=============
*/
bool CollisionHelper::BoxBoxCollisionTest( const BoxShape& box1, const BoxShape& box2 ) {
	glm::vec3 minValuesBox1 = box1.GetPosition() - ( box1.GetDimensions() * 0.5f );
	glm::vec3 maxValuesBox1 = box1.GetPosition() + ( box1.GetDimensions() * 0.5f );

	glm::vec3 minValuesBox2 = box2.GetPosition() - ( box2.GetDimensions() * 0.5f );
	glm::vec3 maxValuesBox2 = box2.GetPosition() + ( box2.GetDimensions() * 0.5f );

	if ( maxValuesBox1.x < minValuesBox2.x ) return false; // a is left of b
    if ( minValuesBox1.x > maxValuesBox2.x ) return false; // a is right of b
    if ( maxValuesBox1.y < minValuesBox2.y ) return false; // a is above b
    if ( minValuesBox1.y > maxValuesBox2.y ) return false; // a is below b
    if ( maxValuesBox1.z < minValuesBox2.z ) return false; // a is in front b
    if ( minValuesBox1.z > maxValuesBox2.z ) return false; // a is behind b
    return true; // boxes overlap
}