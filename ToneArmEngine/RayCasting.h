/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/27/2014 10:31:10 AM
------------------------------------------------------------------------------------------
*/

#ifndef __RAY_CASTING_H__
#define __RAY_CASTING_H__

#include "Ray.h"

#include <glm/glm.hpp>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <memory>

namespace vgs
{
/*
------------------------------------------------------------------------------------------
	Raycasting

	Ray Casting helper.
------------------------------------------------------------------------------------------
*/

class Shape;
class ColliderComponent;

namespace RayCasting {

template <class T>
struct RayCastResult {
	
	T* Object;
	float Distance;
	glm::vec3 Intersection;

	RayCastResult() :
		Object(nullptr),
		Distance(0.0f),
		Intersection(glm::vec3())
	{}

	RayCastResult(T* const object, float distance, const glm::vec3& inter) :
		Object(object),
		Distance(distance),
		Intersection(inter)
	{}

	bool operator<(const RayCastResult<T>& other) const { return Distance < other.Distance; }

	template <class T>
	struct SortFunctor {

		bool operator()(std::shared_ptr<RayCastResult<T>>& object1, std::shared_ptr<RayCastResult<T>>& object2) {
			return object1.get()->Distance < object2.get()->Distance;
		}
	};
};

typedef std::shared_ptr<RayCastResult<Shape>>				ShapeResult;
typedef std::shared_ptr<RayCastResult<ColliderComponent>>	ColliderResult;


ShapeResult					RayCastFirst(const std::vector<Shape*>& shapesToCheck, const glm::vec3& position, const glm::vec3& direction,
										 float distance = 5000.0f);

ColliderResult				RayCastFirst(const std::vector<ColliderComponent*>& collidersToCheck, const glm::vec3& position,
										 const glm::vec3& direction, float distance = 5000.0f);

std::vector<ShapeResult>	RayCastAll(const std::vector<Shape*>& shapesToCheck, const glm::vec3& position, const glm::vec3& direction,
									   float distance = 5000.0f);

std::vector<ColliderResult>	RayCastAll(const std::vector<ColliderComponent*>& collidersToCheck, const glm::vec3& position,
									   const glm::vec3& direction, float distance = 5000.0f);

ColliderResult				RayCastShot(const glm::vec3& position,
										const glm::vec3& direction,
										const std::vector<ColliderComponent*>& shapesToCheck,
										ColliderComponent* const target);
}

} // namespace vgs
	
#endif __RAY_CASTING_H__