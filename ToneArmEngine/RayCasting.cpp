/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/27/2014 10:31:10 AM
------------------------------------------------------------------------------------------
*/

#include "Raycasting.h"
#include "MathHelper.h"
#include "BoxCollider.h"
#include "CylinderCollider.h"

#include <map>

using namespace vgs;

//
// method that casts a ray from the specified point in 3D space with the specidied direction
// and returns the first Shape object which was hit
//
RayCasting::ShapeResult RayCasting::RayCastFirst(const std::vector<Shape*>& shapesToCheck, const glm::vec3& position, const glm::vec3& direction,
												 float distance) {

	std::vector<RayCasting::ShapeResult> hitObjects = RayCastAll(shapesToCheck, position, direction, distance);

	if (hitObjects.size() > 0) {
		return *hitObjects.begin();
	}

	return nullptr;
}

//
// method that casts a ray from the specified point in 3D space with the specidied direction
// and returns the first Collider object which was hit
//
RayCasting::ColliderResult RayCasting::RayCastFirst(const std::vector<ColliderComponent*>& collidersToCheck, const glm::vec3& position,
													const glm::vec3& direction, float distance) {

	std::vector<RayCasting::ColliderResult> hitObjects = RayCasting::RayCastAll(collidersToCheck, position, direction, distance);

	if (hitObjects.size() > 0) {
		return *hitObjects.begin();
	}

	return nullptr;
}

//
// casts a ray from the specified point in 3D space with the specidied direction
// and returns a vector of Shape objects which were hit
//
std::vector<RayCasting::ShapeResult> RayCasting::RayCastAll(const std::vector<Shape*>& shapesToCheck, const glm::vec3& position,
															const glm::vec3& direction, float distance) {

	RayCasting::Ray ray = RayCasting::Ray(position, direction, distance);

	std::vector<RayCasting::ShapeResult> hitObjects(0);

	for (auto shape : shapesToCheck) {

		// check ray-to-rectangle intersection if a box shape is being checked
		if (shape->GetRTTI() == BoxShape::rtti) {

			BoxShape* box = static_cast<BoxShape*>(shape);
			
			// make sure vertical dimensions match
			glm::vec3 boxPosition = box->GetPosition();

			glm::vec3 endPoint = ray.Origin + ray.Direction * ray.Length;

			if (boxPosition.y - box->GetDimensions().y / 2 <= endPoint.y && endPoint.y <= boxPosition.y + box->GetDimensions().y / 2) {
				glm::vec3 intersect;
				if (MathHelper::RayRectIntersectionCheck2D(ray, box->GetRect(), intersect)) {
					intersect.y = endPoint.y;
					hitObjects.push_back(RayCasting::ShapeResult(new RayCasting::RayCastResult<Shape>(box,
																									  glm::distance(ray.Origin, intersect),
																									  intersect)));
				}
			}
		}
		// check ray-to-circle intersection if a cylinder shape is being checked
		else if (shape->GetRTTI() == CylinderShape::rtti) {

			CylinderShape* cylinder = static_cast<CylinderShape*>(shape);

			// make sure vertical dimensions match
			glm::vec3 cylinderPosition = cylinder->GetPosition();

			glm::vec3 endPoint = ray.Origin + ray.Direction * ray.Length;

			if (cylinderPosition.y - cylinder->GetHeight() / 2 <= endPoint.y && endPoint.y <= cylinderPosition.y + cylinder->GetHeight() / 2) {
				glm::vec3 intersect;
				if (MathHelper::LineSegmentCircleIntersectionCheck2D(ray, MathHelper::Circle(cylinderPosition, cylinder->GetRadius()), &intersect )) {	
					intersect.y = endPoint.y;
					hitObjects.push_back(RayCasting::ShapeResult(new RayCasting::RayCastResult<Shape>(cylinder,
																									  glm::distance(ray.Origin, intersect),
																									  intersect)));
				}
			}
		}
	}

	std::sort(hitObjects.begin(), hitObjects.end(), RayCasting::RayCastResult<Shape>::SortFunctor<Shape>());
	
	return hitObjects;
}

//
// casts a ray from the specified point in 3D space with the specidied direction
// and returns a vector of Collider objects which were hit
//
std::vector<RayCasting::ColliderResult> RayCasting::RayCastAll(const std::vector<ColliderComponent*>& collidersToCheck, const glm::vec3& position,
															   const glm::vec3& direction, float distance) {

	RayCasting::Ray ray = RayCasting::Ray(position, direction, distance);

	std::vector<RayCasting::ColliderResult> hitObjects(0);

	for (auto collider : collidersToCheck) {

		// check ray-to-rectangle intersection if a box shape is being checked
		if (collider->GetRTTI() == BoxCollider::rtti) {

			BoxCollider* box = static_cast<BoxCollider*>(collider);
			BoxShape* shape = static_cast<BoxShape*>(box->GetShape());
			
			// make sure vertical dimensions match
			glm::vec3 boxPosition = box->GetPosition();

			glm::vec3 endPoint = ray.Origin + ray.Direction * ray.Length;

			if (boxPosition.y - box->GetDimensions().y / 2 <= endPoint.y && endPoint.y <= boxPosition.y + box->GetDimensions().y / 2) {
				glm::vec3 intersect;
				if (MathHelper::RayRectIntersectionCheck2D(ray, shape->GetRect(), intersect)) {
					intersect.y = endPoint.y;
					hitObjects.push_back(RayCasting::ColliderResult(new RayCasting::RayCastResult<ColliderComponent>(box,
																													 glm::distance(ray.Origin, intersect),
																													 intersect)));
				}
			}
		}
		// check ray-to-circle intersection if a cylinder shape is being checked
		else if (collider->GetRTTI() == CylinderCollider::rtti) {

			CylinderCollider* cylinder = static_cast<CylinderCollider*>(collider);

			// make sure vertical dimensions match
			glm::vec3 cylinderPosition = cylinder->GetPosition();

			glm::vec3 endPoint = ray.Origin + ray.Direction * ray.Length;

			if (cylinderPosition.y - cylinder->GetHeight() / 2 <= endPoint.y && endPoint.y <= cylinderPosition.y + cylinder->GetHeight() / 2) {
				glm::vec3 intersect;
				if (MathHelper::LineSegmentCircleIntersectionCheck2D(ray, MathHelper::Circle(cylinderPosition, cylinder->GetRadius()), &intersect )) {	
					intersect.y = endPoint.y;
					hitObjects.push_back(RayCasting::ColliderResult(new RayCasting::RayCastResult<ColliderComponent>(cylinder,
																													 glm::distance(ray.Origin, intersect),
																												     intersect)));
				}
			}
		}
	}

	std::sort(hitObjects.begin(), hitObjects.end(), RayCasting::RayCastResult<ColliderComponent>::SortFunctor<ColliderComponent>());
	
	return hitObjects;
}

//
// the only normal implementation of RayCastShot()
//
RayCasting::ColliderResult RayCasting::RayCastShot(const glm::vec3& position,
												   const glm::vec3& direction,
												   const std::vector<ColliderComponent*>& shapesToCheck,
												   ColliderComponent* const target) {
	
	RayCasting::RayCastResult<ColliderComponent>* result = nullptr;

	RayCasting::Ray shotRay = RayCasting::Ray(position, direction);
	float distance;

	// check if the ray hits the target
	if (target->GetRTTI() == CylinderCollider::rtti) {

		CylinderCollider* targetCollider = static_cast<CylinderCollider*>(target);
		glm::vec3 targetPosition = targetCollider->GetPosition();

		// make sure vertical dimensions macth
		distance = glm::distance(shotRay.Origin, targetPosition) + targetCollider->GetRadius();
		shotRay.Length = distance;
		glm::vec3 endPoint = shotRay.Origin + shotRay.Direction * distance;

		if (endPoint.y >= targetPosition.y - targetCollider->GetHeight() / 2 && endPoint.y <= targetPosition.y + targetCollider->GetHeight() / 2) {

			// y coordinate doesn't matter anymore!
			targetPosition.y = shotRay.Origin.y;

			if (MathHelper::RayCircleIntersectionCheck(shotRay, MathHelper::Circle(targetPosition, targetCollider->GetRadius()))) {
				result = new RayCasting::RayCastResult<ColliderComponent>(targetCollider, distance, glm::vec3());
			}
		}
	} 

	// check if there are any objects between the target that was hit and the shooter
	RayCasting::ColliderResult objectInTheWay = RayCastFirst(shapesToCheck, position, direction);

	if (objectInTheWay) {

		if (result) {

			float distanceToObject = glm::distance(shotRay.Origin, objectInTheWay.get()->Intersection);
			float distanceToTarget = glm::distance(shotRay.Origin, target->GetPosition());

			// target is closer than the obstacle, so target was hit
			if (distanceToTarget < distanceToObject) {
				return RayCasting::ColliderResult(result);
			}
		}
		else {
			return objectInTheWay;
		}
	}
	else if (result) {
		// nothing in the way, target was hit
		return RayCasting::ColliderResult(result);
	}

	//// nothing at all was hit
	return nullptr;
}