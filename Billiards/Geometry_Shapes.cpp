#include "Geometry_Shapes.h"

//
// Plane
//

Geometry::Plane::Plane(unsigned int a_increments, float a_size,
					   const glm::vec3& a_origin,
					   const glm::vec3& a_normal,
					   const glm::vec3& a_verticalAxis)
	: Geometry(a_origin, glm::orientation(a_normal, a_verticalAxis), PLANE),
	  increments(a_increments), size(a_size) {}
Geometry::Plane::Plane(unsigned int a_increments, float a_size,
					   const glm::vec3& a_origin,
					   const glm::quat& a_orientation)
	: Geometry(a_origin, a_orientation, PLANE),
	  increments(a_increments), size(a_size) {}

Geometry* Geometry::Plane::Clone() const
{
	return new Plane(increments, size, position, orientation());
}
glm::vec3 Geometry::Plane::ClosestSurfacePointTo(const glm::vec3& a_point,
												 glm::vec3* a_normal) const
{
	float distance = glm::dot(normal(), a_point - position);
	if (nullptr != a_normal)
		*a_normal = normal();
	return a_point - normal()*distance;
}
bool Geometry::Plane::Contains(const glm::vec3& a_point) const
{
	return (0 == glm::dot(a_point - position, normal()));
}

//
// Sphere
//

Geometry::Sphere::Sphere(float a_radius, const glm::vec3& a_center)
	: Geometry(a_center, SPHERE), radius(a_radius) {}
Geometry::Sphere::Sphere(float a_radius, const glm::vec3& a_center,
						 const glm::quat& a_orientation)
	: Geometry(a_center, a_orientation, SPHERE), radius(a_radius) {}
Geometry::Sphere::Sphere(float a_radius, const glm::vec3& a_center,
						 const glm::vec3& a_forward, const glm::vec3& a_up)
	: Geometry(a_center, a_forward, a_up, SPHERE), radius(a_radius) {}
Geometry::Sphere::Sphere(float a_radius, const glm::vec3& a_center,
						 const glm::vec3& a_axis, float a_angle)
	: Geometry(a_center, a_axis, a_angle, SPHERE), radius(a_radius) {}
Geometry::Sphere::Sphere(float a_radius, const glm::vec3& a_center,
						 float a_yaw, float a_pitch, float a_roll)
	: Geometry(a_center, a_yaw, a_pitch, a_roll, SPHERE), radius(a_radius) {}
Geometry::Sphere::Sphere(float a_radius, const glm::vec3& a_center,
						 float a_yaw, float a_pitch, float a_roll,
						 const glm::vec3& a_yawAxis, const glm::vec3& a_rollAxis)
	: Geometry(a_center, a_yaw, a_pitch, a_roll, a_yawAxis, a_rollAxis, SPHERE), radius(a_radius) {}

glm::vec3 Geometry::Sphere::AxisAlignedExtents() const
{
	return glm::vec3(radius);
}
Geometry* Geometry::Sphere::Clone() const
{
	return new Sphere(radius, position, orientation());
}
glm::vec3 Geometry::Sphere::ClosestSurfacePointTo(const glm::vec3& a_point,
												  glm::vec3* a_normal) const
{
	if (nullptr != a_normal)
		*a_normal = glm::normalize(a_point - position);
	return position + glm::normalize(a_point - position)*radius;
}
bool Geometry::Sphere::Contains(const glm::vec3& a_point) const
{
	return (glm::distance2(position, a_point) <= radius*radius);
}
float Geometry::Sphere::volume() const
{
	return radius * radius * radius * glm::pi<float>() * 4 / 3;
}
float Geometry::Sphere::area() const
{
	return radius * radius * glm::pi<float>() * 4;
}
glm::mat3 Geometry::Sphere::interiaTensorDividedByMass() const
{
	return glm::mat3(radius * radius * 2 / 3, 0, 0,
		0, radius * radius * 2 / 3, 0,
		0, 0, radius * radius * 2 / 3);
}

//
// Box
//

Geometry::Box::Box(const glm::vec3& a_extents, const glm::vec3& a_center)
	: Geometry(a_center, BOX), extents(a_extents) {}
Geometry::Box::Box(const glm::vec3& a_extents, const glm::vec3& a_center,
				   const glm::quat& a_orientation)
	: Geometry(a_center, a_orientation, BOX), extents(a_extents) {}
Geometry::Box::Box(const glm::vec3& a_extents, const glm::vec3& a_center,
				   const glm::vec3& a_forward, const glm::vec3& a_up)
	: Geometry(a_center, a_forward, a_up, BOX), extents(a_extents) {}
Geometry::Box::Box(const glm::vec3& a_extents, const glm::vec3& a_center,
				   const glm::vec3& a_axis, float a_angle)
	: Geometry(a_center, a_axis, a_angle, BOX), extents(a_extents) {}
Geometry::Box::Box(const glm::vec3& a_extents, const glm::vec3& a_center,
				   float a_yaw, float a_pitch, float a_roll)
	: Geometry(a_center, a_yaw, a_pitch, a_roll, BOX), extents(a_extents) {}
Geometry::Box::Box(const glm::vec3& a_extents, const glm::vec3& a_center,
				   float a_yaw, float a_pitch, float a_roll,
				   const glm::vec3& a_yawAxis, const glm::vec3& a_rollAxis)
	: Geometry(a_center, a_yaw, a_pitch, a_roll, a_yawAxis, a_rollAxis, BOX), extents(a_extents) {}

glm::vec3 Geometry::Box::AxisAlignedExtents() const
{
	glm::vec3 corners[4] = { ToWorld(extents, true),
		ToWorld(glm::vec3(-extents.x, extents.y, extents.z), true),
		ToWorld(glm::vec3(extents.x, -extents.y, extents.z), true),
		ToWorld(glm::vec3(extents.x, extents.y, -extents.z), true) };
	glm::vec3 result(0);
	for (auto corner : corners)
	{
		if (fabs(corner.x) > result.x)
			result.x = fabs(corner.x);
		if (fabs(corner.y) > result.y)
			result.y = fabs(corner.y);
		if (fabs(corner.z) > result.z)
			result.z = fabs(corner.z);
	}
	return result;
}
Geometry* Geometry::Box::Clone() const
{
	return new Box(extents, position, orientation());
}
glm::vec3 Geometry::Box::ClosestSurfacePointTo(const glm::vec3& a_point,
											   glm::vec3* a_normal) const
{
	// rotate into box's coordinate system and calculate distances between point
	// and nearest face in each direction
	glm::vec3 local = ToLocal(a_point);
	glm::vec3 distances = extents - glm::vec3(fabs(local.x), fabs(local.y), fabs(local.z));
	if (0 < distances.x && 0 < distances.y && 0 < distances.z)
	{
		if (distances.x < distances.y && distances.x < distances.z)
			distances.x *= -1;
		else if (distances.y < distances.z)
			distances.y *= -1;
		else
			distances.z *= -1;
	}

	// no normal at corners and edges
	if (nullptr != a_normal)
	{
		if (0 == distances.x && 0 < distances.y && 0 < distances.z)
			*a_normal = ToWorld((0 > local.x ? -1.0f : 1.0f), 0, 0, true);
		else if (0 < distances.x && 0 == distances.y && 0 < distances.z)
			*a_normal = ToWorld(0, (0 > local.y ? -1.0f : 1.0f), 0, true);
		else if (0 < distances.x && 0 < distances.y && 0 == distances.z)
			*a_normal = ToWorld(0, 0, (0 > local.z ? -1.0f : 1.0f), true);
		else
			*a_normal = glm::vec3(0);
	}

	// find closest point on box surface
	return ToWorld((0 > local.x ? -1 : 1) * (extents.x - (0 <= distances.x ? distances.x : 0)),
		(0 > local.y ? -1 : 1) * (extents.y - (0 <= distances.y ? distances.y : 0)),
		(0 > local.z ? -1 : 1) * (extents.z - (0 <= distances.z ? distances.z : 0)));
}
float Geometry::Box::volume() const
{
	return extents.x * extents.y * extents.z * 8;
}
float Geometry::Box::area() const
{
	return (extents.x*extents.y + extents.y*extents.z + extents.z*extents.x) * 2;
}
bool Geometry::Box::Contains(const glm::vec3& a_point) const
{
	glm::vec3 local = ToLocal(a_point);
	return (-extents.x <= local.x && local.x <= extents.x &&
			-extents.y <= local.y && local.y <= extents.y &&
			-extents.z <= local.z && local.z <= extents.z);
}
glm::mat3 Geometry::Box::interiaTensorDividedByMass() const
{
	return glm::mat3((extents.y*extents.y + extents.z*extents.z) / 3, 0, 0,
					 0, (extents.x*extents.x + extents.z*extents.z) / 3, 0,
					 0, 0, (extents.x*extents.x + extents.y*extents.y) / 3);
}

std::vector<glm::vec3> Geometry::Box::vertices() const
{
	std::vector<glm::vec3> points;
	points.push_back(ToWorld(extents.x, extents.y, extents.z));
	points.push_back(ToWorld(extents.x, extents.y, -extents.z));
	points.push_back(ToWorld(extents.x, -extents.y, extents.z));
	points.push_back(ToWorld(extents.x, -extents.y, -extents.z));
	points.push_back(ToWorld(-extents.x, extents.y, extents.z));
	points.push_back(ToWorld(-extents.x, extents.y, -extents.z));
	points.push_back(ToWorld(-extents.x, -extents.y, extents.z));
	points.push_back(ToWorld(-extents.x, -extents.y, -extents.z));
	return points;
}
