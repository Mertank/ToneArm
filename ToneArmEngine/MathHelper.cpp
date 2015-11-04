/*
========================

Copyright (c) 2014 Vinyl Games Studio

	MathHelper.cpp

		MathHelper helps.

		Created by: Vladyslav Dolhopolov
		Created on: 4/25/2014 4:01:56 PM

========================
*/
#include "MathHelper.h"
#include "Node.h"
#include "BoxCollider.h"
#include "Scene.h"
#include "Camera.h"
#include "RunningInfo.h"

#ifndef MERRY_SERVER
#include "Engine.h"
#endif

#include <algorithm>
#include <amp.h>

namespace vgs {

glm::quat MathHelper::CreateQuaternion(float radians, const glm::vec3& axis)
{
	// CAUTION: axis cannot be zero-length!
	float len = glm::length(axis);

	// if axis length or angle are (almost) zero, return identity quaternion
	if (len < 0.0001f || std::fabs(radians) < 0.0001f) 
	{
		// glm::quat(1.0f, 0.0f, 0.0f, 0.0f)
		return glm::quat();  
	} 
	else
	{
		glm::vec3 v = (1.0f / len) * axis;		// normalized axis
		float c		= std::cos(0.5f * radians); // cos of half angle
		float s		= std::sin(0.5f * radians); // sin of half angle
		return glm::quat(c, s * v.x, s * v.y, s * v.z);
	}
}

#ifndef MERRY_SERVER
//
// method that returns a projection of a point in 3D space (4th coordinate is homogenious) onto the screen
//
glm::vec2 MathHelper::ProjectPointOntoScreen(const glm::vec4& point) {

	// position the point relative to the camera
	Scene*			scene	= Engine::GetInstance()->GetRunningScene();
	Camera*			cam		= scene->GetCurrentCamera();
	RunningInfo*	info	= Engine::GetInstance()->GetRunningInfo();

	glm::vec4 positionProjected = cam->GetViewMatrix() * point;

	// project it
	positionProjected = cam->GetProjectionMatrix() * positionProjected;

	// apply the viewport transformation
	positionProjected /= positionProjected.w;

	glm::vec2 halfWindow( info->contextWidth * 0.5f, info->contextHeight * 0.5f );

	glm::vec2 positionOnScreen = glm::vec2(positionProjected.x * halfWindow.x + halfWindow.x, -positionProjected.y * halfWindow.y + halfWindow.y );
	//positionOnScreen *= screenSize;

	return positionOnScreen;
}
#endif
	
//
// returns an angle in degrees between the two given vectors (2D space)
//
float MathHelper::AngleBetweenVectors(const glm::vec2& v1, const glm::vec2& v2) {

	float magnitude1 = sqrt(v1.x * v1.x + v1.y * v1.y);
	float magnitude2 = sqrt(v2.x * v2.x + v2.y * v2.y);

	float scalar = v1.x * v2.x + v1.y * v2.y;

	float cos = scalar / (magnitude1 * magnitude2);

	return acosf(cos) * glm::degrees(1.0f);;
}

//
// returns an angle in degrees between the two given vectors (3D space)
//
float MathHelper::AngleBetweenVectors(const glm::vec3& v1, const glm::vec3& v2) {

	float magnitude1 = sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
	float magnitude2 = sqrt(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z);

	float scalar = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

	float cos = scalar / (magnitude1 * magnitude2);

	float angle = acosf(cos) * glm::degrees(1.0f);

	return acosf(cos) * glm::degrees(1.0f);
}

//
// checks if a ray intersects a circle (3D space)
//
bool MathHelper::RayCircleIntersectionCheck(const RayCasting::Ray& ray, const Circle& circle) {

	// http://stackoverflow.com/questions/1073336/circle-line-collision-detection
	// solve a quadratic equasion (if there is a solution then the ray MIGHT intersect the circle)
	glm::vec3 rayEndPoint = ray.Origin + ray.Direction * ray.Length;
	glm::vec3 rayVector = rayEndPoint - ray.Origin;
	glm::vec3 toRayStart = ray.Origin - circle.Center;

	float a = glm::dot(rayVector, rayVector);
	float b = 2.0f * glm::dot(toRayStart, rayVector);
	float c = glm::dot(toRayStart, toRayStart) - circle.Radius * circle.Radius;

	float discriminant = b * b - 4.0f * a * c;

	// no intersection
	if (discriminant < -FLOAT_EPSILON) {
		return false;
	}
	else {
		discriminant = sqrt(discriminant);

		float t1 = (-b - discriminant) / (2.0f * a);
		float t2 = (-b + discriminant) / (2.0f * a);

		if (t1 >= 0.0f && t1 <= 1.0f) {
			return true;
		}
			
		if (t2 >= 0 && t2 <= 1.0f) {
			return true;
		}
	}

	return false;
}

//
// checks if a ray intersects a rectangle (2D space)
//
bool MathHelper::RayRectIntersectionCheck2D(const RayCasting::Ray& ray, RectF& rect, glm::vec3& intersection) {
	std::vector<glm::vec3> intersects;
	GetIntersections( ray, rect, intersects );

	if ( intersects.size() > 0 ) {
		float closest = glm::distance( ray.Origin, intersects[0] );
		intersection = intersects[0];

		for( std::vector<glm::vec3>::iterator iter = intersects.begin() + 1;
			 iter != intersects.end(); ++iter ) {
			float thisDist = glm::distance( ray.Origin, *iter );
			if ( thisDist < closest ) {
				closest = thisDist;
				intersection = *iter;
			}
		}
	}

	return intersects.size() > 0;
}
/*
=========
MathHelper::GetIntersections

	Gets all intersections with a rectangle
=========
*/
void MathHelper::GetIntersections( const RayCasting::Ray& ray, RectF& rect, std::vector<glm::vec3>& intersects ) {
	LineSegment lsRay = LineSegment(ray);
	const LineSegment* const lines = rect.ToLineSegments();

	glm::vec3 intersect;
	// a ray intersects a rectangle in it intersects one of it's sides
	for ( unsigned int i = 0; i < 4; ++i ) {
		if ( LineSegmentsIntersectionCheck2D( lsRay, lines[i], intersect ) ) {
			intersects.push_back( intersect );
		}
	}
}
/*
=========
MathHelper::GetIntersection

	Gets this intersection of 2 line segments
=========
*/
bool MathHelper::GetIntersection( const LineSegment& ls1, const LineSegment& ls2, glm::vec3& intersect ) {
	//http://community.topcoder.com/tc?module=Static&d1=tutorials&d2=geometry2
	// a quadratic equasion has to be solved in order to perform the check
	float a1 = ls1.StartPoint.z - ls1.EndPoint.z;
	float b1 = ls1.StartPoint.x - ls1.EndPoint.x;
	float c1 = a1 * ls1.StartPoint.x + b1 * ls1.StartPoint.z;

	float a2 = ls2.StartPoint.z - ls2.EndPoint.z;
	float b2 = ls2.StartPoint.x - ls2.EndPoint.x;
	float c2 = a2 * ls2.StartPoint.x + b2 * ls2.StartPoint.z;

	float discriminant = a1 * b2 - a2 * b1;

	// no intersection
	if ( abs( discriminant ) < 0.000001f ) {
		return false;
	}
	else {
		intersect.x = (b2 * c1 - b1 * c2) / discriminant;
		intersect.z = (a1 * c2 - a2 * c1) / discriminant;
	}

	return true;
}
/*
========
MathHelper::CrossProduct2D

	Cross product of 2d vectors.
	Magnitude of a 3d cross.
========
*/
float MathHelper::CrossProduct2D( const glm::vec2& a, const glm::vec2& b ) {
	return ( a.x * b.y ) - ( a.y * b.x );
}
//
// checks if two line segments intersect (CAUTION: this method ignores 3rd dimension!)
// http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
//
bool MathHelper::LineSegmentsIntersectionCheck2D(const LineSegment& ls1, const LineSegment& ls2, glm::vec3& intersection) {
	
	glm::vec2 p( ls1.StartPoint.x, ls1.StartPoint.z );
	glm::vec2 q( ls2.StartPoint.x, ls2.StartPoint.z );
	
	glm::vec2 r( ls1.EndPoint.x - ls1.StartPoint.x, ls1.EndPoint.z - ls1.StartPoint.z );
	glm::vec2 s( ls2.EndPoint.x - ls2.StartPoint.x, ls2.EndPoint.z - ls2.StartPoint.z );

	float uNumerator	= CrossProduct2D( ( q - p ), r );
	float uDenominator	= CrossProduct2D( r, s );

	if ( uNumerator == 0.0f && uDenominator == 0.0f ) {
		//colinear
		return ( ( q.x - p.x < 0.0f ) != ( q.x - ls1.EndPoint.x < 0.0f ) != ( ls2.EndPoint.x - p.x < 0.0f ) != ( ls2.EndPoint.x - ls1.EndPoint.x < 0.0f ) ) ||
			   ( ( q.y - p.y < 0.0f ) != ( q.y - ls1.EndPoint.z < 0.0f ) != ( ls2.EndPoint.z - p.y < 0.0f ) != ( ls2.EndPoint.z - ls1.EndPoint.z < 0.0f ) );
	}

	if ( uDenominator == 0.0f ) {
		//Lines are parallel
		return false;
	}

	float u = uNumerator / uDenominator;
	float t = CrossProduct2D( ( q - p ), s ) / uDenominator;

	if ( ( t >= -FLOAT_EPSILON ) && ( t <= ( 1.0f + FLOAT_EPSILON ) ) && ( u >= -FLOAT_EPSILON ) && ( u <= ( 1.0f + FLOAT_EPSILON ) ) ) {
		intersection.x = p.x + ( t * r.x );
		intersection.z = p.y + ( t * r.y );
		return true;
	}

	return false;
	/*
	float discriminant = ( ls1.StartPoint.x - ls1.EndPoint.x ) * ( ls2.StartPoint.z - ls2.EndPoint.z ) - ( ls1.StartPoint.z - ls1.EndPoint.z ) * ( ls2.StartPoint.x - ls2.EndPoint.x );
	
	if ( fabs( discriminant ) < 0.000001f ) { 
		return false; 
	}	

	float pre  = ( ls1.StartPoint.x * ls1.EndPoint.z - ls1.StartPoint.z * ls1.EndPoint.x );
	float post = ( ls2.StartPoint.x * ls2.EndPoint.z - ls2.StartPoint.z * ls2.EndPoint.x );

	float x = ( pre * ( ls2.StartPoint.x - ls2.EndPoint.x ) - ( ls1.StartPoint.x - ls1.EndPoint.x ) * post ) / discriminant;
	float y = ( pre * ( ls2.StartPoint.z - ls2.EndPoint.z ) - ( ls1.StartPoint.z - ls1.EndPoint.z ) * post ) / discriminant;

	if ( x < ( min( ls1.StartPoint.x, ls1.EndPoint.x ) - FLOAT_EPSILON ) || x > ( max( ls1.StartPoint.x, ls1.EndPoint.x ) + FLOAT_EPSILON ) ||
		 x < ( min( ls2.StartPoint.x, ls2.EndPoint.x ) - FLOAT_EPSILON ) || x > ( max( ls2.StartPoint.x, ls2.EndPoint.x ) + FLOAT_EPSILON ) ) {
		return false;
	}

	if ( y < ( min( ls1.StartPoint.z, ls1.EndPoint.z ) - FLOAT_EPSILON ) || y > ( max( ls1.StartPoint.z, ls1.EndPoint.z ) + FLOAT_EPSILON ) ||
		 y < ( min( ls2.StartPoint.z, ls2.EndPoint.z ) - FLOAT_EPSILON ) || y > ( max( ls2.StartPoint.z, ls2.EndPoint.z ) + FLOAT_EPSILON ) ) {
		return false;
	}

	intersection.x = x;
	intersection.z = y;

	return true;*/

	/*if ( GetIntersection( ls1, ls2, intersection ) ) {
		// lines intersect, let's find out if the intersection point is on both line segments
		if (PointOnLineSegmentCheck2D(intersection, ls1) && PointOnLineSegmentCheck2D(intersection, ls2)) {
			return true;
		}
	}

	return false;*/
}

//
// checks if a point is on the line segment or not (3D space)
//
bool MathHelper::PointOnLineSegmentCheck(const glm::vec3& point, const LineSegment& ls) {

	if (min(ls.StartPoint.x, ls.EndPoint.x) <= point.x && point.x <= max(ls.StartPoint.x, ls.EndPoint.x) &&
		min(ls.StartPoint.y, ls.EndPoint.y) <= point.y && point.y <= max(ls.StartPoint.y, ls.EndPoint.y) &&
		min(ls.StartPoint.z, ls.EndPoint.z) <= point.z && point.z <= max(ls.StartPoint.z, ls.EndPoint.z)) {
			return true;
	}

	return false;
}

//
// checks if a point is on the line segment or not (CAUTION: method ignores y coordinate)
//
bool MathHelper::PointOnLineSegmentCheck2D(const glm::vec3& point, const LineSegment& ls) {
	if (min(ls.StartPoint.x, ls.EndPoint.x) <= point.x && point.x <= max(ls.StartPoint.x, ls.EndPoint.x) &&
		min(ls.StartPoint.z, ls.EndPoint.z) <= point.z && point.z <= max(ls.StartPoint.z, ls.EndPoint.z)) {
		return true;
	}	

	return false;
}

//
// checks if a circle intersects a rectangle and if it does, returns the side of the rectangle that intersects the circle
//
std::vector<std::shared_ptr<MathHelper::LineSegment>> MathHelper::CircleRectangleIntersectionCheck2D(const Circle& circle, RectF& rect) {

	// a circle intersect a rectanle if either it's center is inside of that rectangle or one of the sides of the rectangle intersects the circle
	const LineSegment* const lines = rect.ToLineSegments();
	std::vector<std::shared_ptr<MathHelper::LineSegment>> result = std::vector<std::shared_ptr<MathHelper::LineSegment>>();

	for ( unsigned int i = 0; i < 4; ++i ) {
		if (LineSegmentCircleIntersectionCheck2D( lines[i], circle)) {
			result.push_back( std::shared_ptr<MathHelper::LineSegment>( new MathHelper::LineSegment( lines[i] ) ) );
		}
	}

	return result;
}

//
// checks if a point is inside of a rectangle formed by the passed 4 sides (2D space)
//
bool MathHelper::PointInRectangleCheck2D(const glm::vec3& point, const LineSegment* const rectSides) {

	float maxX = max(rectSides[0].StartPoint.x, max(rectSides[1].StartPoint.x, max(rectSides[2].StartPoint.x, rectSides[3].StartPoint.x)));
	float maxZ = max(rectSides[0].StartPoint.z, max(rectSides[1].StartPoint.z, max(rectSides[2].StartPoint.z, rectSides[3].StartPoint.z)));
	float minX = min(rectSides[0].StartPoint.x, min(rectSides[1].StartPoint.x, min(rectSides[2].StartPoint.x, rectSides[3].StartPoint.x)));
	float minZ = min(rectSides[0].StartPoint.z, min(rectSides[1].StartPoint.z, min(rectSides[2].StartPoint.z, rectSides[3].StartPoint.z)));

	if (point.x >= minX && point.x <= maxX &&
		point.z >= minZ && point.z <= maxZ) {
			return true;
	}

	return false;
}

//
// checks if a line segment intersects a circle (2D space)
//
bool MathHelper::LineSegmentCircleIntersectionCheck2D(const LineSegment& ls, const Circle& circle, glm::vec3* intersection) {

	// http://blog.csharphelper.com/2010/03/28/determine-where-a-line-intersects-a-circle-in-c.aspx
	// solve a quardatic equasion in order to find possible intersection points

	float dx, dz, a, b, c, discriminant, t;

	dx = ls.EndPoint.x - ls.StartPoint.x;
	dz = ls.EndPoint.z - ls.StartPoint.z;

	a = dx * dx + dz * dz;
	b = 2 * (dx * (ls.StartPoint.x - circle.Center.x) + dz * (ls.StartPoint.z - circle.Center.z));
	c = (ls.StartPoint.x - circle.Center.x) * (ls.StartPoint.x - circle.Center.x) +
		(ls.StartPoint.z - circle.Center.z) * (ls.StartPoint.z - circle.Center.z) - circle.Radius * circle.Radius;

	discriminant = b * b - 4 * a * c;

	glm::vec3 intersectionPoint1, intersectionPoint2;

	// no intersection
	if ((a <= 0.0) || (discriminant < -FLOAT_EPSILON))
	{
		return false;
	}
	// one intersection
	else if (abs( discriminant ) < FLOAT_EPSILON)
	{
		t = -b / (2 * a);
		intersectionPoint1 = glm::vec3(ls.StartPoint.x + t * dx, ls.StartPoint.y, ls.StartPoint.z + t * dz);

		// check if the intersection point lies on the line segment that we are checking
		if (PointOnLineSegmentCheck(intersectionPoint1, ls)) {
			if ( intersection ) {
				*intersection = intersectionPoint1;
			}
			return true;
		}
	}
	// two intersections
	else
	{
		t = (float)((-b + sqrt(discriminant)) / (2 * a));
		intersectionPoint1 = glm::vec3(ls.StartPoint.x + t * dx, ls.StartPoint.y, ls.StartPoint.z + t * dz);
		t = (float)((-b - sqrt(discriminant)) / (2 * a));
		intersectionPoint2 = glm::vec3(ls.StartPoint.x + t * dx, ls.StartPoint.y, ls.StartPoint.z + t * dz);

		// check if at least one of the intersection points lies on the line segment that we are checking
		if (PointOnLineSegmentCheck(intersectionPoint1, ls) || PointOnLineSegmentCheck(intersectionPoint2, ls)) {
			if ( intersection ) {
				if ( glm::distance( intersectionPoint1, ls.StartPoint ) < glm::distance( intersectionPoint2, ls.StartPoint ) ) {
					*intersection = intersectionPoint1;
				} else {
					*intersection = intersectionPoint2;
				}
			}
			return true;
		}
	}

	return false;
}

//
// checks if two circles intersect (2D space)
//
bool MathHelper::CircleCircleIntersectionCheck2D(const Circle& circle1, const Circle& circle2) {

	if (glm::distance(glm::vec3(circle1.Center.x, 0.0f, circle1.Center.z), glm::vec3(circle2.Center.x, 0.0f, circle2.Center.z)) < circle1.Radius + circle2.Radius) {
		return true;
	}

	return nullptr;
}

//
// method that normalizes a vector and at least cares if it's lenght is 0 or not (fuck glm!)
//
glm::vec3 MathHelper::Normalize(const glm::vec3& vector) {

	glm::vec3 result = vector;

	float length = sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

	if (length > 0) {
		return result / length;
	}

	return result;
}

glm::vec2 MathHelper::Normalize(const glm::vec2& vector) {

	glm::vec2 result = vector;

	float length = sqrtf(vector.x * vector.x + vector.y * vector.y);

	if (length > 0) {
		return result / length;
	}

	return result;
}

// excluded because of the Nodes
#ifndef MERRY_SERVER
//
// calculates the distance between two Nodes (3D space)
//
float MathHelper::CalculateDistance(Node& node1, Node& node2) {

	glm::mat4 globalMatrix = node1.GetGlobalModelMatrix();
	glm::vec3 position1 = glm::vec3(globalMatrix[3][0], globalMatrix[3][1], globalMatrix[3][2]);

	globalMatrix = node2.GetGlobalModelMatrix();
	glm::vec3 position2 = glm::vec3(globalMatrix[3][0], globalMatrix[3][1], globalMatrix[3][2]);

	return glm::distance(position1, position2);
}

//
// calculates the distance between a point and a Node (3D space)
//
float MathHelper::CalculateDistance(const glm::vec3& point, Node& node) {

	glm::mat4 globalMatrix = node.GetGlobalModelMatrix();
	glm::vec3 position = glm::vec3(globalMatrix[3][0], globalMatrix[3][1], globalMatrix[3][2]);

	return glm::distance(point, position);
}
#endif

/*
==========
GetAABB

	Gets the AABB for a rotated cube
==========
*/
glm::vec3 MathHelper::GetAABBDimensions( const BoxShape& box ) {
	if ( abs( box.GetRotation().y ) < FLOAT_EPSILON ) {
		return box.GetBoundingDimensions();
	}
	glm::vec3 boxDimensions = box.GetDimensions();
	glm::vec3 corners[4];

	corners[0] = glm::vec3(  ( boxDimensions.x * 0.5f ), 0.0f,  ( boxDimensions.z * 0.5f ) );
	corners[1] = glm::vec3( -( boxDimensions.x * 0.5f ), 0.0f,  ( boxDimensions.z * 0.5f ) );
	corners[2] = glm::vec3(  ( boxDimensions.x * 0.5f ), 0.0f, -( boxDimensions.z * 0.5f ) );
	corners[3] = glm::vec3( -( boxDimensions.x * 0.5f ), 0.0f, -( boxDimensions.z * 0.5f ) );

	glm::mat4 rotMat = glm::rotate( glm::mat4( 1.0 ), box.GetRotation().y, glm::vec3( 0.0f, 1.0f, 0.0f ) );
				
	for ( int i = 0; i < 4; ++i ) {
		corners[i] = glm::vec3( rotMat * glm::vec4( corners[i], 1.0f ) );
	}

	float minX = corners[0].x;
	float maxX = corners[0].x;
	float minZ = corners[0].z;
	float maxZ = corners[0].z;

	for ( int i = 1; i < 4; ++i ) {
		if ( corners[i].x < minX ) {
			minX = corners[i].x;
		}
		if ( corners[i].x > maxX ) {
			maxX = corners[i].x;
		}

		if ( corners[i].z < minZ ) {
			minZ = corners[i].z;
		}
		if ( corners[i].z > maxZ ) {
			maxZ = corners[i].z;
		}
	}

	boxDimensions.x = maxX - minX;
	boxDimensions.z = maxZ - minZ;

	return boxDimensions;
}

//
// a method that calculates new positions for all the verticies of the passed line segments with respect to the passed rotation value (in radians)
// CAUTION: method assumes that the rotation passed to it is rotation around y-axis
//
void MathHelper::ApplyRotation(LineSegment* lines, unsigned int count, const glm::vec3 origin, const float angle) {
	glm::mat3 rotationMatrix = glm::mat3( glm::rotate( glm::mat4( 1.0 ), glm::degrees( angle ), glm::vec3( 0.0f, 1.0f, 0.0f ) ) );
	for ( unsigned int i = 0; i < count; ++i ) {
		/*LineSegment originalLs = *ls;
		float cos = std::cosf(-angle);
		float sin = std::sinf(-angle);

		ls->StartPoint.x = (originalLs.StartPoint.x - origin.x) * cos - (originalLs.StartPoint.z - origin.z) * sin + origin.x;
		ls->StartPoint.z = (originalLs.StartPoint.x - origin.x) * sin + (originalLs.StartPoint.z - origin.z) * cos + origin.z;

		ls->EndPoint.x = (originalLs.EndPoint.x - origin.x) * cos - (originalLs.EndPoint.z - origin.z) * sin + origin.x;
		ls->EndPoint.z = (originalLs.EndPoint.x - origin.x) * sin + (originalLs.EndPoint.z - origin.z) * cos + origin.z;*/
		
		lines[i].StartPoint	= ( lines[i].StartPoint - origin ) * rotationMatrix + origin;
		lines[i].EndPoint	= ( lines[i].EndPoint - origin ) * rotationMatrix + origin;
	}
}

//
// returns one of the two passed vectors that is more collinear with the target vector (3D space)
//
const glm::vec3& MathHelper::MostCollinear(const glm::vec3& target, const glm::vec3& vector1, const glm::vec3& vector2) {

	if (glm::dot(target, vector1) > glm::dot(target, vector2)) {
		return vector1;
	}
	else {
		return vector2;
	}
}

//
// performs linear interpolation between two vectors with a default coefficient of 0.5
//
glm::vec3 MathHelper::Lerp(const glm::vec3& from, const glm::vec3& to) {

	return from + (to - from) * 0.5f;
}

//
// performs linear interpolation between two vectors with a passed coefficient
//
glm::vec3 MathHelper::Lerp(const glm::vec3& from, const glm::vec3& to, const float amount) {

	return from + (to - from) * amount;
}

//
// performs linear interpolation between several points
//
glm::vec3 MathHelper::Lerp(std::deque<glm::vec3>& points) {

	glm::vec3 result;
	
	for (unsigned int i = 0; i < points.size(); i++) {
		result += points.back();
		points.push_front(points.back());
		points.pop_back();
	}

	result = result / (float)points.size();

	points.pop_back();

	return result;
}
/*
=======
MathHelper::GetUp

	Gets up vector from a quaternion
=======
*/
glm::vec3 MathHelper::GetUp( const glm::quat& quat ) {
	return glm::vec3( 2 * ( quat.x * quat.y - quat.w * quat.z ),
					  1 - 2 * ( quat.x * quat.x + quat.z * quat.z ),
					  2 * ( quat.y * quat.z + quat.w * quat.x ) );
}
/*
=======
MathHelper::GetRight

	Gets right vector from a quaternion
=======
*/
glm::vec3 MathHelper::GetRight( const glm::quat& quat ) {
	return glm::vec3( 1 - 2 * ( quat.y * quat.y + quat.z * quat.z ),
					  2 * ( quat.x * quat.y + quat.w * quat.z ),
					  2 * ( quat.x * quat.z - quat.w * quat.y ) );
}
/*
=======
MathHelper::GetForward

	Gets forward vector from a quaternion
=======
*/
glm::vec3 MathHelper::GetForward( const glm::quat& quat ) {
	return glm::vec3( 2 * ( quat.x * quat.z + quat.w * quat.y ),
					  2 * ( quat.y * quat.x - quat.w * quat.x ),
					  1 - 2 * ( quat.x * quat.x + quat.y * quat.y ) );
}
/*
=======
MathHelper::RayPlaneIntersection

	Finds the intersection point of a line, and a plane
	http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-7-intersecting-simple-shapes/ray-plane-and-ray-disk-intersection/
=======
*/
bool MathHelper::RayPlaneIntersection( const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& planeNormal, const glm::vec3 planePoint, glm::vec4& intersection ) {
	float dot = glm::dot( direction, planeNormal );
	if ( dot > FLOAT_EPSILON ) {
		glm::vec3 toPlane = planePoint - origin;
		float t = glm::dot( toPlane, planeNormal ) / dot;
		intersection = glm::vec4( origin + ( direction * t ), 1.0f );
		return true;
	} else { //Parallel
		return false;
	}
}
/*
=======
MathHelper::ComputeQuaternionW

	Finds the W value for the quaternion.
=======
*/
void MathHelper::ComputeQuaternionW( glm::quat& quaternion ) {
	float w = 1.0f - ( quaternion.x * quaternion.x ) - ( quaternion.y * quaternion.y ) - ( quaternion.z * quaternion.z );
	if ( w < 0.0f ) {
		quaternion.w = 0.0f;
	} else {
		quaternion.w = sqrt( w );
	}
}
}