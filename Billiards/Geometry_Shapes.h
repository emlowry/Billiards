#pragma once
#include "Geometry.h"

struct Geometry::Plane : public Geometry
{
	Plane(unsigned int a_increments = 20, float a_size = 1.0f,
		  const glm::vec3& a_origin = glm::vec3(0),
		  const glm::vec3& a_normal = glm::vec3(0, 0, 1),
		  const glm::vec3& a_verticalAxis = glm::vec3(0, 1, 0));
	Plane(unsigned int a_increments, float a_size,
		  const glm::vec3& a_origin,
		  const glm::quat& a_orientation);

	virtual glm::vec3 AxisAlignedExtents() const { return glm::vec3(0); }	// actually infinite
	virtual glm::vec3 ClosestSurfacePointTo(const glm::vec3& a_point,
											glm::vec3* a_normal = nullptr) const;
	virtual bool Contains(const glm::vec3& a_point) const;
	virtual Geometry* Clone() const;
	virtual float volume() const { return 0; }
	virtual float area() const { return 0; }	// actually infinite
	virtual glm::mat3 interiaTensorDividedByMass() const { return glm::mat3(0); }

	glm::vec3 normal() const { return localZAxis(); }
	glm::vec3 up() const { return localYAxis(); }
	glm::vec3 right() const { return localXAxis(); }

	unsigned int increments;
	float size;
};

struct Geometry::Sphere : public Geometry
{
	Sphere(float a_radius = 0.5f,
		   const glm::vec3& a_center = glm::vec3(0));
	Sphere(float a_radius, const glm::vec3& a_center,
		   const glm::quat& a_orientation);
	Sphere(float a_radius, const glm::vec3& a_center,
		   const glm::vec3& a_forward, const glm::vec3& a_up = glm::vec3(0, 0, 1));
	Sphere(float a_radius, const glm::vec3& a_center,
		   const glm::vec3& a_axis, float a_angle);
	Sphere(float a_radius, const glm::vec3& a_center,
		   float a_yaw, float a_pitch, float a_roll);
	Sphere(float a_radius, const glm::vec3& a_center,
		   float a_yaw, float a_pitch, float a_roll,
		   const glm::vec3& a_yawAxis, const glm::vec3& a_rollAxis = glm::vec3(1, 0, 0));

	virtual glm::vec3 AxisAlignedExtents() const;
	virtual Geometry* Clone() const;
	virtual glm::vec3 ClosestSurfacePointTo(const glm::vec3& a_point,
											glm::vec3* a_normal = nullptr) const;
	virtual bool Contains(const glm::vec3& a_point) const;
	virtual float volume() const;
	virtual float area() const;
	virtual glm::mat3 interiaTensorDividedByMass() const;

	float radius;
};

struct Geometry::Box : public Geometry
{
	Box(const glm::vec3& a_extents = glm::vec3(1),
		const glm::vec3& a_center = glm::vec3(0));
	Box(const glm::vec3& a_extents, const glm::vec3& a_center,
		const glm::quat& a_orientation);
	Box(const glm::vec3& a_extents, const glm::vec3& a_center,
		const glm::vec3& a_forward, const glm::vec3& a_up = glm::vec3(0, 0, 1));
	Box(const glm::vec3& a_extents, const glm::vec3& a_center,
		const glm::vec3& a_axis, float a_angle);
	Box(const glm::vec3& a_extents, const glm::vec3& a_center,
		float a_yaw, float a_pitch, float a_roll);
	Box(const glm::vec3& a_extents, const glm::vec3& a_center,
		float a_yaw, float a_pitch, float a_roll,
		const glm::vec3& a_yawAxis, const glm::vec3& a_rollAxis = glm::vec3(1, 0, 0));

	virtual glm::vec3 AxisAlignedExtents() const;
	virtual Geometry* Clone() const;
	virtual glm::vec3 ClosestSurfacePointTo(const glm::vec3& a_point,
		glm::vec3* a_normal = nullptr) const;
	virtual bool Contains(const glm::vec3& a_point) const;
	virtual float volume() const;
	virtual float area() const;
	virtual glm::mat3 interiaTensorDividedByMass() const;

	glm::vec3 size() const { return extents * 2.0f; }
	std::vector<glm::vec3> vertices() const;

	glm::vec3 extents;
};
