#include "Geometry.h"
#include <functional>

typedef bool(*CollisionDetector)(const Geometry& a_shape1, const Geometry& a_shape2,
								 Geometry::Collision* a_collision);

static bool Flip(CollisionDetector a_detector,
				 const Geometry& a_shape1, const Geometry& a_shape2,
				 Geometry::Collision* a_collision)
{
	bool result = a_detector(a_shape2, a_shape1, a_collision);
	if (result && nullptr != a_collision)
	{
		Geometry* temp = a_collision->shape1().Clone();
		a_collision->shape1(a_collision->shape2());
		a_collision->shape2(*temp);
		delete temp;
		a_collision->normal *= -1.0f;
	}
	return result;
}

static bool PlanePlane(const Geometry& a_shape1, const Geometry& a_shape2,
					   Geometry::Collision* a_collision);
static bool PlaneSphere(const Geometry& a_shape1, const Geometry& a_shape2,
						Geometry::Collision* a_collision);
static bool PlaneBox(const Geometry& a_shape1, const Geometry& a_shape2,
					 Geometry::Collision* a_collision);

static bool SpherePlane(const Geometry& a_shape1, const Geometry& a_shape2,
						Geometry::Collision* a_collision)
{
	return Flip(PlaneSphere, a_shape1, a_shape2, a_collision);
}
static bool SphereSphere(const Geometry& a_shape1, const Geometry& a_shape2,
						 Geometry::Collision* a_collision);
static bool SphereBox(const Geometry& a_shape1, const Geometry& a_shape2,
					  Geometry::Collision* a_collision);

static bool BoxPlane(const Geometry& a_shape1, const Geometry& a_shape2,
					 Geometry::Collision* a_collision)
{
	return Flip(PlaneBox, a_shape1, a_shape2, a_collision);
}
static bool BoxSphere(const Geometry& a_shape1, const Geometry& a_shape2,
					  Geometry::Collision* a_collision)
{
	return Flip(SphereBox, a_shape1, a_shape2, a_collision);
}
static bool BoxBox(const Geometry& a_shape1, const Geometry& a_shape2,
				   Geometry::Collision* a_collision);

static CollisionDetector g_collisionFunctions[Geometry::SHAPE_COUNT][Geometry::SHAPE_COUNT] =
{
	{ nullptr, nullptr, nullptr, nullptr },
	{ nullptr, PlanePlane, PlaneSphere, PlaneBox },
	{ nullptr, SpherePlane, SphereSphere, SphereBox },
	{ nullptr, BoxPlane, BoxSphere, BoxBox }
};

bool Geometry::DetectCollision(const Geometry& a_shape1, const Geometry& a_shape2,
							   Geometry::Collision* a_collision)
{
	// validity check
	if (&a_shape1 == &a_shape2 ||
		Geometry::SHAPE_COUNT <= a_shape1.GetShape() ||
		Geometry::SHAPE_COUNT <= a_shape2.GetShape())
		return false;

	// call appropriate function
	CollisionDetector f = g_collisionFunctions[a_shape1.GetShape()][a_shape2.GetShape()];
	if (nullptr == f)
		return false;
	return f(a_shape1, a_shape2, a_collision);
}

bool PlanePlane(const Geometry& a_shape1, const Geometry& a_shape2,
				Geometry::Collision* a_collision)
{
	// type check
	const Geometry::Plane* plane1 = dynamic_cast<const Geometry::Plane*>(&a_shape1);
	const Geometry::Plane* plane2 = dynamic_cast<const Geometry::Plane*>(&a_shape2);
	if (nullptr == plane1 || nullptr == plane2)
		return false;

	// the only non-colliding planes are parallel planes with distance between them
	glm::vec3 normal1 = plane1->normal();
	glm::vec3 normal2 = plane2->normal();
	glm::vec3 cross = glm::cross(normal1, normal2);
	if (glm::vec3(0) == cross &&
		0 != glm::dot(normal1, plane2->position - plane1->position))
		return false;

	// otherwise, planes always collide
	if (nullptr != a_collision)
	{
		a_collision->shape1(a_shape1);
		a_collision->shape2(a_shape2);
		a_collision->interpenetration = 0;
		glm::vec3 midpoint = (plane1->position + plane2->position) * 0.5f;
		if (glm::vec3(0) == cross)
		{
			a_collision->normal = normal1;
			a_collision->point = midpoint;
		}
		else
		{
			a_collision->normal = glm::normalize(normal1 + (0 > glm::dot(normal1, normal2) ? -normal2 : normal2));
			glm::vec3 p;
			float d1 = glm::dot(plane1->position, normal1);
			float d2 = glm::dot(plane2->position, normal2);
			if (0 != cross.z)
			{
				p.x = (d1*normal2.y - d2*normal1.y) / cross.z;
				p.y = (d2*normal1.x - d1*normal2.x) / cross.z;
				p.z = 0;
			}
			else if (0 != cross.y)
			{
				p.z = (d1*normal2.x - d2*normal1.x) / cross.y;
				p.x = (d2*normal1.z - d1*normal2.z) / cross.y;
				p.y = 0;
			}
			else // 0 != cross.x
			{
				p.y = (d1*normal2.z - d2*normal1.z) / cross.x;
				p.z = (d2*normal1.y - d1*normal2.y) / cross.x;
				p.x = 0;
			}
			cross = glm::normalize(cross);
			a_collision->point = p + cross*glm::dot(cross, midpoint - p);
		}
	}
	return true;
}

bool PlaneSphere(const Geometry& a_shape1, const Geometry& a_shape2,
				 Geometry::Collision* a_collision)
{
	// type check
	const Geometry::Plane* plane = dynamic_cast<const Geometry::Plane*>(&a_shape1);
	const Geometry::Sphere* sphere = dynamic_cast<const Geometry::Sphere*>(&a_shape2);
	if (nullptr == plane || nullptr == sphere)
		return false;

	// sphere and plane collide if distance between <= radius
	glm::vec3 normal = plane->normal();
	glm::vec3 displacement = sphere->position - plane->position;
	float distance = glm::dot(normal, displacement);
	if (0 > distance)
	{
		normal *= -1.0f;
		distance *= -1;
	}
	if (distance <= sphere->radius)
	{
		if (nullptr != a_collision)
		{
			a_collision->shape1(a_shape1);
			a_collision->shape2(a_shape2);
			a_collision->normal = normal;
			a_collision->interpenetration = sphere->radius - distance;
			a_collision->point = sphere->position - a_collision->normal * distance;
		}
		return true;
	}
	return false;
}

static void GetMinAndMax(const std::vector<glm::vec3>& a_points,
						 std::function<float(const glm::vec3&)> a_distanceFunction,
						 float& a_min, float& a_max,
						 glm::vec3& a_minPoint, glm::vec3& a_maxPoint,
						 float a_tolerance = 0.0001f)
{
	if (0 > a_tolerance)
		a_tolerance *= -1;
	std::vector<glm::vec3> maxPoints, minPoints;
	bool start = true;
	for (auto point : a_points)
	{
		float distance = a_distanceFunction(point);
		if (start || distance >= a_max - a_tolerance)
		{
			if (distance > a_max + a_tolerance)
				maxPoints.clear();
			maxPoints.push_back(point);
			a_max = distance;
		}
		if (start || distance <= a_min + a_tolerance)
		{
			if (distance < a_min - a_tolerance)
				minPoints.clear();
			minPoints.push_back(point);
			a_min = distance;
		}
		start = false;
	}
	a_minPoint = glm::vec3(0);
	for (auto point : minPoints)
		a_minPoint += point;
	a_minPoint /= (float)(minPoints.size());
	a_maxPoint = glm::vec3(0);
	for (auto point : maxPoints)
		a_maxPoint += point;
	a_maxPoint /= (float)(maxPoints.size());
}

bool PlaneBox(const Geometry& a_shape1, const Geometry& a_shape2,
			  Geometry::Collision* a_collision)
{
	// type check
	const Geometry::Plane* plane = dynamic_cast<const Geometry::Plane*>(&a_shape1);
	const Geometry::Box* box = dynamic_cast<const Geometry::Box*>(&a_shape2);
	if (nullptr == plane || nullptr == box)
		return false;

	// get distances from plane to vertices, with positive distances in the normal
	// direction and negative distances in the opposite direction
	glm::vec3 normal = plane->normal();
	float max, min;
	glm::vec3 minPoint, maxPoint;
	GetMinAndMax(box->vertices(),
				 [&](const glm::vec3& a_point)
				 {
					return glm::dot(normal, a_point - plane->position);
				 },
				 min, max, minPoint, maxPoint);

	// if there are points on both sides, there's an intersection
	if (0 >= max * min)
	{
		if (nullptr != a_collision)
		{
			a_collision->shape1(a_shape1);
			a_collision->shape2(a_shape2);
			a_collision->normal = normal * (fabs(min) > max ? -1.0f : 1.0f);
			a_collision->interpenetration = fmin(fabs(min), max);
			glm::vec3 p = (fabs(min) > max ? maxPoint : minPoint);
			a_collision->point = p - normal * a_collision->interpenetration *0.5f;
		}
		return true;
	}
	return false;
}

bool SphereSphere(const Geometry& a_shape1, const Geometry& a_shape2,
				  Geometry::Collision* a_collision)
{
	// type check
	const Geometry::Sphere* sphere1 = dynamic_cast<const Geometry::Sphere*>(&a_shape1);
	const Geometry::Sphere* sphere2 = dynamic_cast<const Geometry::Sphere*>(&a_shape2);
	if (nullptr == sphere1 || nullptr == sphere2)
		return false;

	// spheres collide if center-center distance is <= sum of radii
	float squareDistance = glm::distance2(sphere1->position, sphere2->position);
	float collisionDistance = sphere1->radius + sphere2->radius;
	if (squareDistance <= collisionDistance*collisionDistance)
	{
		if (nullptr != a_collision)
		{
			a_collision->shape1(a_shape1);
			a_collision->shape2(a_shape2);
			a_collision->normal = glm::normalize(sphere2->position - sphere1->position);
			a_collision->interpenetration = collisionDistance - sqrt(squareDistance);
			float d = sphere1->radius - a_collision->interpenetration / 2;
			a_collision->point = sphere1->position + a_collision->normal * d;
		}
		return true;
	}
	return false;
}

bool SphereBox(const Geometry& a_shape1, const Geometry& a_shape2,
			   Geometry::Collision* a_collision)
{
	// type check
	const Geometry::Sphere* sphere = dynamic_cast<const Geometry::Sphere*>(&a_shape1);
	const Geometry::Box* box = dynamic_cast<const Geometry::Box*>(&a_shape2);
	if (nullptr == sphere || nullptr == box)
		return false;

	// find closest point on box surface
	bool inside = box->Contains(sphere->position);
	glm::vec3 closestPoint = box->ClosestSurfacePointTo(sphere->position);

	// if sphere center is inside box or no more than a radius away from the nearest surface point,
	// then there's a collision.
	if (inside || sphere->Contains(closestPoint))
	{
		if (nullptr != a_collision)
		{
			a_collision->shape1(a_shape1);
			a_collision->shape2(a_shape2);
			a_collision->normal =
				glm::normalize(closestPoint - sphere->position) * (inside ? -1.0f : 1.0f);
			a_collision->interpenetration = sphere->radius +
				(glm::distance(closestPoint, sphere->position) * (inside ? 1 : -1));
			float d = sphere->radius - a_collision->interpenetration / 2;
			a_collision->point = sphere->position + a_collision->normal * d;
		}
		return true;
	}
	return false;
}

// negative value = no overlap
static float ProjectionOverlap(const glm::vec3& a_axis,
							   const std::vector<glm::vec3>& a_group1,
							   const std::vector<glm::vec3>& a_group2,
							   glm::vec3& a_midPoint)
{
	float min1, min2, max1, max2;
	glm::vec3 minPoint1, minPoint2, maxPoint1, maxPoint2;
	auto project = [&](const glm::vec3& a_point)
	{
		return glm::dot(a_axis, a_point);
	};
	GetMinAndMax(a_group1, project, min1, max1, minPoint1, maxPoint1);
	GetMinAndMax(a_group2, project, min2, max2, minPoint2, maxPoint2);
	bool group1First = fabs(max1 - min2) < fabs(max2 - min1);
	a_midPoint = (group1First ? maxPoint1 + minPoint2 : maxPoint2 + minPoint1) * 0.5f;
	return (group1First ? max1 - min2 : max2 - min1);
}

bool BoxBox(const Geometry& a_shape1, const Geometry& a_shape2,
			Geometry::Collision* a_collision)
{
	// type check
	const Geometry::Box* box1 = dynamic_cast<const Geometry::Box*>(&a_shape1);
	const Geometry::Box* box2 = dynamic_cast<const Geometry::Box*>(&a_shape2);
	if (nullptr == box1 || nullptr == box2)
		return false;

	// first check - generalize to sphere to avoid unneccessary calculations
	float d = glm::distance(box1->extents, glm::vec3(0)) +
		glm::distance(box2->extents, glm::vec3(0));
	if (d*d < glm::distance2(box1->position, box2->position))
		return false;

	// get all the axes to test for separation
	glm::vec3 centerToCenterAxis = glm::normalize(box2->position - box1->position);
	std::vector<glm::vec3> axes;
	axes.push_back(centerToCenterAxis);
	for (unsigned int i = 0; i < 3; ++i)
	{
		axes.push_back(box1->axis(i));
		axes.push_back(box2->axis(i));
		for (unsigned int j = 0; j < 3; ++j)
			axes.push_back(glm::cross(box1->axis(i), box2->axis(j)));
	}

	// test for separation
	auto vertices1 = box1->vertices();
	auto vertices2 = box2->vertices();
	float interpenetration = 0;
	glm::vec3 midPoint;
	glm::vec3 normal;
	bool start = true;
	for (auto axis : axes)
	{
		// test for projection overlap along each axis
		glm::vec3 p;
		float overlap = ProjectionOverlap(axis, vertices1, vertices2, p);

		// if there is no overlap, then there is no collision
		if (0 > overlap)
			return false;

		// store the collision normal that provides the smallest interpenetration
		if (start || overlap < interpenetration)
		{
			interpenetration = overlap;
			normal = (0 > glm::dot(centerToCenterAxis, axis) ? -axis : axis);
			midPoint = p;	// not even close to accurate, but I don't know how to get the right one
		}
		start = false;
	}

	// make sure the collision normal points from the first box to the second
	if (0 > glm::dot(normal, axes[0]))
		normal *= -1;

	// if the projections of each box onto each possible axis always overlap, then the boxes intersect
	if (nullptr != a_collision)
	{
		a_collision->shape1(a_shape1);
		a_collision->shape2(a_shape2);
		a_collision->normal = normal;
		a_collision->interpenetration = interpenetration;
		a_collision->point = midPoint;
	}
	return true;
}
