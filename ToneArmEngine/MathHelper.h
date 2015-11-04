/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MathHelper.h

		MathHelper helps.

		Created by: Vladyslav Dolhopolov
		Created on: 4/25/2014 4:01:56 PM

========================
*/
#ifndef __MATH_HELPER_H__
#define __MATH_HELPER_H__

// gl math
#include <glm/glm.hpp>                      // glm::vec3, glm::vec4, glm::ivec4, glm::mat4, ...
#include <glm/gtc/matrix_transform.hpp>     // glm::translate, glm::rotate, glm::scale, glm::perspective, ...
#include <glm/gtc/type_ptr.hpp>             // glm::value_ptr
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <queue>

#include "Ray.h"
#include "VGSMacro.h"

namespace vgs
{
class BoxShape;
class Node;

namespace MathHelper {

	const float		PI				= 3.14159265359f;
	const float		TWO_PI			= 2.0f * PI;
	const float		HALF_PI			= 0.5f * PI;
	const glm::mat4 IDENTITY_MATRIX = glm::mat4( 1.0 );
	
	glm::vec2 Normalize(const glm::vec2& vecvtor);

	struct PolarCoordinateComparator {
		glm::vec2 origin;
		glm::vec2 direction;

		bool operator()( const glm::vec3& a, const glm::vec3& b ) {
			return lessThan( a, b );
		}

		bool operator()( const glm::vec4* const a, const glm::vec4* const b ) {
			return lessThan( glm::vec3( *a ), glm::vec3( *b ) );
		}

		bool operator()( glm::vec3* a, glm::vec3* b ) {
			return lessThan( *a, *b );
		}

	private:
		bool lessThan( const glm::vec3& a, const glm::vec3& b ) {
			glm::vec2 rightDirection( direction.x * cos( glm::radians( -90.0f ) ) - direction.y * sin( glm::radians( -90.0f ) ),
									  direction.y * cos( glm::radians( -90.0f ) ) + direction.x * sin( glm::radians( -90.0f ) ) );

			glm::vec2 A2d( a.x - origin.x, a.z - origin.y );
			glm::vec2 B2d( b.x - origin.x, b.z - origin.y );

			A2d = MathHelper::Normalize( A2d );
			B2d = MathHelper::Normalize( B2d );

			float rightDotA = glm::dot( rightDirection, A2d );
			float rightDotB = glm::dot( rightDirection, B2d );

			if ( rightDotA > 0.0f && rightDotB < 0.0f ) {
				return false;
			} else if ( rightDotA < 0.0f && rightDotB > 0.0f ) {
				return true;
			} else {
				float dotA = glm::dot( direction, A2d );
				float dotB = glm::dot( direction, B2d );

				if ( dotA == dotB ) {
					return glm::distance( origin, glm::vec2( a.x, a.z ) ) < glm::distance( origin, glm::vec2( b.x, b.z ) );
				} else {
					if ( rightDotA > 0.0f ) {
						return dotA > dotB;
					} else{
						return dotA < dotB;
					}
				}
			}
		}
	};
	static PolarCoordinateComparator polarCoordinateComparator;

	//
	// a circle in 3D space
	//
	struct Circle {

		glm::vec3	Center;
		float		Radius;

		Circle() :
			Center(glm::vec3()),
			Radius(0.0f)
		{}

		Circle(const glm::vec3& center, const float radius) :
			Center(center),
			Radius(radius)
		{}
	};

	// a line segment in 3D space
	struct LineSegment {

		glm::vec3	StartPoint;
		glm::vec3	EndPoint;

		LineSegment() :
			StartPoint(glm::vec3()),
			EndPoint(glm::vec3())
		{}

		LineSegment(const glm::vec3& start, const glm::vec3& end) :
			StartPoint(start),
			EndPoint(end)
		{}

		LineSegment(const RayCasting::Ray& ray) :
			StartPoint(ray.Origin),
			EndPoint(ray.Origin + ray.Direction * ray.Length)
		{}

		LineSegment(const LineSegment& orig) :
			StartPoint(orig.StartPoint),
			EndPoint(orig.EndPoint)
		{}

		void Revert() {
			glm::vec3 tmp = StartPoint;
			StartPoint = EndPoint;
			EndPoint = tmp;
		}

		bool operator== (const LineSegment& other) { return StartPoint == other.StartPoint && EndPoint == other.EndPoint; }
	};

	void ApplyRotation(LineSegment* lines, unsigned int count, const glm::vec3 origin, const float angle);

	// rectangle in 3D space (float coordinates, float dimensions)
	struct RectF {
		glm::vec3	Center;
		float		Width;
		float		Height;
		float		Angle;

		RectF() :
			Center(glm::vec3()),
			Width(0.0f),
			Height(0.0f),
			Angle(0.0f),
			prevAngle( Angle ),
			prevWidth( Width ),
			prevHeight( Height ),
			prevCenter( Center )
		{
			CalculateSegments();
		}

		RectF(const glm::vec3& center, const float width, const float height, const float angle) :
			Center(center),
			Width(width),
			Height(height),
			Angle(angle),
			prevAngle( Angle ),
			prevWidth( Width ),
			prevHeight( Height ),
			prevCenter( Center )
		{
			CalculateSegments();
		}

		const LineSegment* const ToLineSegments() {	
			if ( abs( prevWidth - Width ) > FLOAT_EPSILON || 
				 abs( prevHeight - Height ) > FLOAT_EPSILON ||
				 abs( prevAngle - Angle ) > FLOAT_EPSILON ||
				 prevCenter != Center ) {
				CalculateSegments();
				prevAngle = Angle;
				prevHeight = Height;
				prevCenter = Center;
				prevWidth = Width;
			}
			return lineSegments;
		}
	private:
		glm::vec3	prevCenter;
		float		prevWidth;
		float		prevHeight;
		float		prevAngle;

		LineSegment	lineSegments[4];

		void CalculateSegments( void ) {
			lineSegments[0].StartPoint = glm::vec3(Center.x - Width / 2, Center.y, Center.z - Height / 2);
			lineSegments[0].EndPoint = glm::vec3(Center.x - Width / 2, Center.y, Center.z + Height / 2);

			lineSegments[1].StartPoint = glm::vec3(Center.x - Width / 2, Center.y, Center.z - Height / 2);
			lineSegments[1].EndPoint = glm::vec3(Center.x + Width / 2, Center.y, Center.z - Height / 2);

			lineSegments[2].StartPoint = glm::vec3(Center.x - Width / 2, Center.y, Center.z + Height / 2);
			lineSegments[2].EndPoint = glm::vec3(Center.x + Width / 2, Center.y, Center.z + Height / 2);

			lineSegments[3].StartPoint = glm::vec3(Center.x + Width / 2, Center.y, Center.z + Height / 2);
			lineSegments[3].EndPoint = glm::vec3(Center.x + Width / 2, Center.y, Center.z - Height / 2);

			if (Angle != 0.0f) {
				MathHelper::ApplyRotation( lineSegments, 4, Center, glm::radians( Angle ) );
			}
		}
	};

	glm::quat												CreateQuaternion(float radians, const glm::vec3& axis);
	void													ComputeQuaternionW( glm::quat& q );

// sorry, no screen on server 
#ifndef MERRY_SERVER
	glm::vec2												ProjectPointOntoScreen(const glm::vec4& point);
#endif

	float													AngleBetweenVectors(const glm::vec2& v1, const glm::vec2& v2);
	float													AngleBetweenVectors(const glm::vec3& v1, const glm::vec3& v2);

	// intersections
	void													GetIntersections( const RayCasting::Ray& ray, RectF& rect, std::vector<glm::vec3>& intersects );
	bool													GetIntersection( const LineSegment& ls1, const LineSegment& ls2, glm::vec3& intersect );
	bool													RayCircleIntersectionCheck(const RayCasting::Ray& ray, const Circle& circle);
	bool													RayRectIntersectionCheck2D(const RayCasting::Ray& ray, RectF& rect, glm::vec3& intersection);
	bool													LineSegmentsIntersectionCheck2D(const LineSegment& ls1, const LineSegment& ls2, glm::vec3& intersection);
	bool													LineSegmentCircleIntersectionCheck2D(const LineSegment& ls, const Circle& circle, glm::vec3* intersection = NULL);
	bool													PointOnLineSegmentCheck(const glm::vec3& point, const LineSegment& ls);
	bool													PointOnLineSegmentCheck2D(const glm::vec3& point, const LineSegment& ls);
	bool													PointInRectangleCheck2D(const glm::vec3& point, const LineSegment* const rectSides);
	std::vector<std::shared_ptr<MathHelper::LineSegment>>	CircleRectangleIntersectionCheck2D(const Circle& circle, RectF& rect);
	bool													CircleCircleIntersectionCheck2D(const Circle& circle1, const Circle& circl2);
	bool													RayPlaneIntersection( const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& planeNormal, const glm::vec3 planePoint, glm::vec4& intersection );

	glm::vec3												Normalize(const glm::vec3& vecvtor);
	glm::vec2												Normalize(const glm::vec2& vecvtor);
	const glm::vec3&										MostCollinear(const glm::vec3& target, const glm::vec3& vector1, const glm::vec3& vector2);
#ifndef MERRY_SERVER
	float													CalculateDistance(Node& node1, Node& node2);
	float													CalculateDistance(const glm::vec3& point, Node& node);
#endif
	float													CrossProduct2D( const glm::vec2& a, const glm::vec2& b );

	glm::vec3												GetAABBDimensions( const BoxShape& rotatedBox );

	glm::vec3												Lerp(const glm::vec3& from, const glm::vec3& to);
	glm::vec3												Lerp(const glm::vec3& from, const glm::vec3& to, const float amount);
	glm::vec3												Lerp(std::deque<glm::vec3>& points);

	glm::vec3												GetUp( const glm::quat& quaternion );
	glm::vec3												GetForward( const glm::quat& quaternion );
	glm::vec3												GetRight( const glm::quat& quaternion );

	template <class T>
	std::shared_ptr<BoxShape>								CreateBoundingBox(const std::vector<T*>& objects);
}

template <class T>
std::shared_ptr<BoxShape> MathHelper::CreateBoundingBox(const std::vector<T*>& objects) {

	glm::vec3 minObject, maxObject;
	minObject = maxObject = (*objects.begin())->GetPosition();

	for (auto object : objects) {

		if (object->GetPosition().x < minObject.x ) {
			minObject.x = object->GetPosition().x;
		}
		else if (object->GetPosition().x > maxObject.x ) {
			maxObject.x = object->GetPosition().x;
		}

		if (object->GetPosition().y < minObject.y) {
			minObject.y = object->GetPosition().y;
		}
		else if (object->GetPosition().y > maxObject.y) {
			maxObject.y = object->GetPosition().y;
		}

		if (object->GetPosition().z < minObject.z) {
			minObject.z = object->GetPosition().z;
		}
		else if (object->GetPosition().z > maxObject.z) {
			maxObject.z = object->GetPosition().z;
		}
	}

	std::vector<TreeNode<Shape>*> treeNodes;
	BoxShape* boundingBox = BoxShape::CreateBoxShape((minObject + maxObject) * 0.5f, maxObject - minObject);

	return std::shared_ptr<BoxShape>(boundingBox);
}

}

#endif __MATH_HELPER_H__