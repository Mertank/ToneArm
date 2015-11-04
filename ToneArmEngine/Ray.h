/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	09/29/2014 07:39:01 PM
------------------------------------------------------------------------------------------
*/

#ifndef __RAY_H__
#define __RAY_H__

#include <glm/glm.hpp>

namespace vgs {

namespace RayCasting {

/*
------------------------------------------------------------------------------------------
	Ray

	Represents a ray in 3D space.
------------------------------------------------------------------------------------------
*/

struct Ray {
	
	glm::vec3 Origin;
	glm::vec3 Direction;
	float	  Length;

	Ray() :
		Origin(glm::vec3()),
		Direction(glm::vec3()),
		Length( 0.0f )
	{}

	Ray(const glm::vec3& origin, const glm::vec3& direction) :
		Origin(origin),
		Direction(direction),
		Length( 5000.0f )
	{}

	Ray(const glm::vec3& origin, const glm::vec3& direction, float length) :
		Origin(origin),
		Direction(direction),
		Length( length )
	{}
};

}

}

#endif __RAY_H__