#include "Geometry.h"

const glm::mat4 Geometry::NO_ROTATION = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
const glm::quat Geometry::UNROTATED_ORIENTATION = glm::quat(1, 0, 0, 0);

//
// Collision
//

Geometry::Collision::~Collision()
{
	if (nullptr != m_shape1)
	{
		delete m_shape1;
		m_shape1 = nullptr;
	}
	if (nullptr != m_shape2)
	{
		delete m_shape2;
		m_shape2 = nullptr;
	}
}
void Geometry::Collision::shape1(const Geometry& a_shape)
{
	if (nullptr != m_shape1)
		delete m_shape1;
	m_shape1 = a_shape.Clone();
}
void Geometry::Collision::shape2(const Geometry& a_shape)
{
	if (nullptr != m_shape2)
		delete m_shape2;
	m_shape2 = a_shape.Clone();
}

//
// Constructors
//

Geometry::Geometry(const glm::vec3& a_position, Shape a_shape)
	: position(a_position), m_shape(a_shape), m_rotationMatrix(NO_ROTATION),
	  m_orientation(UNROTATED_ORIENTATION) {}
Geometry::Geometry(const glm::vec3& a_position,
				   const glm::mat4& a_rotation,
				   Shape a_shape)
	: position(a_position), m_rotationMatrix(a_rotation), m_shape(a_shape),
	  m_orientation(glm::quat_cast(a_rotation)) {}
Geometry::Geometry(const glm::vec3& a_position,
				   const glm::quat& a_orientation,
				   Shape a_shape)
	: position(a_position), m_rotationMatrix(glm::mat4_cast(a_orientation)),
	  m_shape(a_shape), m_orientation(a_orientation) {}
Geometry::Geometry(const glm::vec3& a_position,
				   const glm::vec3& a_forward, const glm::vec3& a_up,
				   Shape a_shape)
	: position(a_position), m_shape(a_shape),
	  m_rotationMatrix(glm::orientation(a_forward, a_up))
{
	m_orientation = glm::quat_cast(m_rotationMatrix);
}
Geometry::Geometry(const glm::vec3& a_position,
				   const glm::vec3& a_axis, float a_angle,
				   Shape a_shape)
	: position(a_position), m_shape(a_shape)
{
	AxisAngle(m_orientation, a_angle, a_axis);
	m_rotationMatrix = glm::mat4_cast(m_orientation);
}
Geometry::Geometry(const glm::vec3& a_position,
				   float a_yaw, float a_pitch, float a_roll,
				   const glm::vec3& a_yawAxis,
				   const glm::vec3& a_rollAxis,
				   Shape a_shape)
	: position(a_position), m_shape(a_shape)
{
	YawPitchRoll(m_orientation, a_yaw, a_pitch, a_roll, a_yawAxis, a_rollAxis);
	m_rotationMatrix = glm::mat4_cast(m_orientation);
}
Geometry::Geometry(const glm::vec3& a_position,
				   float a_yaw, float a_pitch, float a_roll,
				   Shape a_shape)
	: position(a_position), m_shape(a_shape)
{
	YawPitchRoll(m_orientation, a_yaw, a_pitch, a_roll);
	m_rotationMatrix = glm::mat4_cast(m_orientation);
}

//
// static functions for creating quaternions
//

void Geometry::Rotation(glm::quat& a_orientation, const glm::vec3& a_rotation)
{
	float angle = glm::length(a_rotation);
	AxisAngle(a_orientation, angle, a_rotation);
}
glm::quat Geometry::Rotation(const glm::vec3& a_rotation)
{
	glm::quat result;
	Rotation(result, a_rotation);
	return result;
}
void Geometry::AxisAngle(glm::quat& a_orientation,
						 float a_angle, const glm::vec3& a_axis)
{
	if (glm::vec3(0) == a_axis)
	{
		a_orientation = UNROTATED_ORIENTATION;
		return;
	}
	float angle = a_angle;
	while (angle > glm::pi<float>())
		angle -= glm::pi<float>() * 2;
	while (angle <= -glm::pi<float>())
		angle += glm::pi<float>() * 2;
	a_orientation = glm::quat(glm::cos(angle / 2), glm::normalize(a_axis) * glm::sin(angle / 2));
}
glm::quat Geometry::AxisAngle(float a_angle, const glm::vec3& a_axis)
{
	glm::quat result;
	AxisAngle(result, a_angle, a_axis);
	return result;
}
void Geometry::YawPitchRoll(glm::quat& a_orientation,
							float a_yaw, float a_pitch, float a_roll,
							const glm::vec3& a_yawAxis,
							const glm::vec3& a_rollAxis)
{
	if (0 == a_yaw && 0 == a_pitch && 0 == a_roll)
	{
		a_orientation = UNROTATED_ORIENTATION;
		return;
	}
	glm::vec3 yawAxis = a_yawAxis;
	glm::vec3 rollAxis = a_rollAxis;
	glm::vec3 pitchAxis;
	bool correctRollAxis = (0 != glm::dot(yawAxis, rollAxis));
	if (glm::vec3(0) == yawAxis && glm::vec3(0) == rollAxis)
	{
		yawAxis.z = 1.0f;
		rollAxis.x = 1.0f;
		correctRollAxis = false;
	}
	else if (glm::vec3(0) == rollAxis || rollAxis == yawAxis || rollAxis == -yawAxis)
	{
		glm::vec3 projections(glm::dot(yawAxis, glm::vec3(1, 0, 0)),
							  glm::dot(yawAxis, glm::vec3(0, 1, 0)),
							  glm::dot(yawAxis, glm::vec3(0, 0, 1)));
		if (fabs(projections.x) > fabs(projections.z) &&
			fabs(projections.x) > fabs(projections.y))
			rollAxis = glm::cross(glm::vec3(0, 0 > projections.x ? 1 : -1, 0), yawAxis);
		else if (fabs(projections.y) > fabs(projections.z))
			rollAxis = glm::cross(glm::vec3(0 > projections.y ? -1 : 1, 0, 0), yawAxis);
		else
			rollAxis = glm::cross(glm::vec3(0, 0 > projections.z ? -1 : 1, 0), yawAxis);
		correctRollAxis = false;
	}
	else if (glm::vec3(0) == yawAxis)
	{
		glm::vec3 projections(glm::dot(rollAxis, glm::vec3(1, 0, 0)),
							  glm::dot(rollAxis, glm::vec3(0, 1, 0)),
							  glm::dot(rollAxis, glm::vec3(0, 0, 1)));
		if (fabs(projections.y) > fabs(projections.x) &&
			fabs(projections.y) > fabs(projections.z))
			yawAxis = glm::cross(rollAxis, glm::vec3(0 > projections.y ? 1 : -1, 0, 0));
		else if (fabs(projections.z) > fabs(projections.x))
			yawAxis = glm::cross(rollAxis, glm::vec3(0, 0 > projections.z ? -1 : 1, 0));
		else
			yawAxis = glm::cross(rollAxis, glm::vec3(0, 0 > projections.x ? -1 : 1, 0));
		correctRollAxis = false;
	}
	pitchAxis = glm::cross(yawAxis, rollAxis);
	if (correctRollAxis)
		rollAxis = glm::cross(pitchAxis, yawAxis);
	yawAxis = glm::normalize(yawAxis);
	pitchAxis = glm::normalize(pitchAxis);
	rollAxis = glm::normalize(rollAxis);
	a_orientation = AxisAngle(a_yaw, yawAxis) * AxisAngle(a_pitch, pitchAxis) * AxisAngle(a_roll, rollAxis);
}
glm::quat Geometry::YawPitchRoll(float a_yaw, float a_pitch, float a_roll,
								 const glm::vec3& a_yawAxis, const glm::vec3& a_rollAxis)
{
	glm::quat result;
	YawPitchRoll(result, a_yaw, a_pitch, a_roll, a_yawAxis, a_rollAxis);
	return result;
}

//
// Switching between world and local coordinates
//

glm::vec3 Geometry::ToWorld(const glm::vec3& a_localCoordinate, bool a_isDirection) const
{
	return (glm::translate(position) * m_rotationMatrix * glm::vec4(a_localCoordinate, a_isDirection ? 0 : 1)).xyz();
}
glm::vec3 Geometry::ToWorld(float a_localX, float a_localY, float a_localZ, bool a_isDirection) const
{
	return ToWorld(glm::vec3(a_localX, a_localY, a_localZ), a_isDirection);
}
glm::vec3 Geometry::ToLocal(const glm::vec3& a_worldCoordinate, bool a_isDirection) const
{
	return (glm::inverse(m_rotationMatrix) * glm::translate(-position) * glm::vec4(a_worldCoordinate, a_isDirection ? 0 : 1)).xyz();
}
glm::vec3 Geometry::ToLocal(float a_worldX, float a_worldY, float a_worldZ, bool a_isDirection) const
{
	return ToLocal(glm::vec3(a_worldX, a_worldY, a_worldZ), a_isDirection);
}

//
// orientation manipulation
//

void Geometry::orientation(const glm::quat& a_orientation)
{
	m_orientation = a_orientation;
	m_rotationMatrix = glm::mat4_cast(m_orientation);
}
void Geometry::orientation(const glm::vec3& a_rotation)
{
	orientation(Rotation(a_rotation));
}
void Geometry::orientation(float a_angle, glm::vec3& a_axis)
{
	orientation(AxisAngle(a_angle, a_axis));
}
void Geometry::spin(const glm::quat& a_rotation)
{
	m_orientation = a_rotation * m_orientation;
	m_rotationMatrix = glm::mat4_cast(m_orientation);
}
void Geometry::spin(const glm::vec3& a_rotation)
{
	spin(Rotation(a_rotation));
}
void Geometry::spin(float a_angle, glm::vec3& a_axis)
{
	spin(AxisAngle(a_angle, a_axis));
}
