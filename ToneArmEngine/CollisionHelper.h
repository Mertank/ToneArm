/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/27/2014 10:31:10 AM
------------------------------------------------------------------------------------------
*/

#ifndef __COLLISION_HELPER_H__
#define __COLLISION_HELPER_H__

#include "MathHelper.h"

#include <memory>

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	CollisionHelper

	What more can I say?
------------------------------------------------------------------------------------------
*/

class CylinderShape;
class BoxShape;

typedef std::shared_ptr<MathHelper::LineSegment> Intersection;

namespace CollisionHelper {

	std::vector<Intersection>	CylinderBoxCollisionTest(const CylinderShape& cylinder, const BoxShape& box);
	bool						CylinderCylinderCollisionTest(const CylinderShape& cylinder1, const CylinderShape& cylinder2);
	bool						BoxBoxCollisionTest(const BoxShape& box1, const BoxShape& box2);
};

}

#endif