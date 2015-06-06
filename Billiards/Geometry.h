#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

struct Geometry
{
public:

	// What shape is this?
	enum Shape
	{
		NONE = 0,
		PLANE = 1,
		SPHERE = 2,
		BOX = 3,

		SHAPE_COUNT = 4
	};

	struct Collision
	{
		glm::vec3 point;
		glm::vec3 normal; // points from shape1 to shape 2
		float interpenetration;

		~Collision();

		const Geometry& shape1() const { return *m_shape1; }
		Geometry& shape1() { return *m_shape1; }
		void shape1(const Geometry& a_shape);
		const Geometry& shape2() const { return *m_shape2; }
		Geometry& shape2() { return *m_shape2; }
		void shape2(const Geometry& a_shape);

	private:
		Geometry* m_shape1 = nullptr;
		Geometry* m_shape2 = nullptr;
	};

	// implemented base classes for each shape
	struct Plane;
	struct Box;
	struct Sphere;

	struct Texture
	{
		glm::vec4 color;
		float gloss;
		int imageID;
		Texture(const glm::vec4 a_color = glm::vec4(1), float a_gloss = 0.5f)
			: imageID(-1), color(a_color), gloss(a_gloss) {}
		Texture(int a_imageID, const glm::vec4 a_color = glm::vec4(1), float a_gloss = 0.5f)
			: imageID(a_imageID), color(a_color), gloss(a_gloss) {}
	};

	// static functions
	static void Rotation(glm::quat& a_orientation,
						 const glm::vec3& a_rotation = glm::vec3(0));
	static glm::quat Rotation(const glm::vec3& a_rotation = glm::vec3(0));
	static void AxisAngle(glm::quat& a_orientation,
						  float a_angle = 0,
						  const glm::vec3& a_axis = glm::vec3(0, 0, 1));
	static glm::quat AxisAngle(float a_angle = 0,
							   const glm::vec3& a_axis = glm::vec3(0, 0, 1));
	static void YawPitchRoll(glm::quat& a_orientation,
							 float a_yaw = 0, float a_pitch = 0, float a_roll = 0,
							 const glm::vec3& a_yawAxis = glm::vec3(0, 0, 1),
							 const glm::vec3& a_rollAxis = glm::vec3(0, 0, 1));
	static glm::quat YawPitchRoll(float a_yaw = 0, float a_pitch = 0, float a_roll = 0,
								  const glm::vec3& a_yawAxis = glm::vec3(0, 0, 1),
								  const glm::vec3& a_rollAxis = glm::vec3(0, 0, 1));
	static bool DetectCollision(const Geometry& a_shape1, const Geometry& a_shape2,
								Geometry::Collision* a_collision = nullptr);

	// abstract functions
	virtual glm::vec3 AxisAlignedExtents() const = 0;
	virtual glm::vec3 ClosestSurfacePointTo(const glm::vec3& a_point,
											glm::vec3* a_normal = nullptr) const = 0;
	virtual Geometry* Clone() const = 0;
	virtual bool Contains(const glm::vec3& a_point) const = 0;
	virtual float volume() const = 0;
	virtual float area() const = 0;
	virtual glm::mat3 interiaTensorDividedByMass() const = 0;

	// implemented member functions
	Shape GetShape() const { return m_shape; }
	glm::vec3 ToWorld(const glm::vec3& a_localCoordinate, bool a_isDirection = false) const;
	glm::vec3 ToWorld(float a_localX, float a_localY, float a_localZ, bool a_isDirection = false) const;
	glm::vec3 ToLocal(const glm::vec3& a_worldCoordinate, bool a_isDirection = false) const;
	glm::vec3 ToLocal(float a_worldX, float a_worldY, float a_worldZ, bool a_isDirection = false) const;

	glm::vec3 axis(unsigned int a_index = 2) const { return m_rotationMatrix[a_index % 3].xyz(); }
	glm::vec3 localXAxis() const { return m_rotationMatrix[0].xyz(); }
	glm::vec3 localYAxis() const { return m_rotationMatrix[1].xyz(); }
	glm::vec3 localZAxis() const { return m_rotationMatrix[2].xyz(); }
	const glm::mat4* rotationMatrix() const { return &m_rotationMatrix; }
	const glm::quat& orientation() const { return m_orientation; }
	void orientation(const glm::quat& a_orientation);
	void orientation(const glm::vec3& a_rotation);
	void orientation(float a_angle, glm::vec3& a_axis = glm::vec3(0, 0, 1));
	void spin(const glm::quat& a_rotation);
	void spin(const glm::vec3& a_rotation);
	void spin(float a_angle, glm::vec3& a_axis = glm::vec3(0, 0, 1));

	// constant values
	static const glm::mat4 NO_ROTATION;
	static const glm::quat UNROTATED_ORIENTATION;

	// member variables and what C# would call properties
	glm::vec3 position;

protected:

	// constructors are protected, since this is a base class
	Geometry(const glm::vec3& a_position = glm::vec3(0), Shape a_type = NONE);
	Geometry(const glm::vec3& a_position,
			 const glm::mat4& a_rotation,
			 Shape a_shape = NONE);
	Geometry(const glm::vec3& a_position,
			 const glm::quat& a_orientation,
			 Shape a_shape = NONE);
	Geometry(const glm::vec3& a_position,
			 const glm::vec3& a_forward, const glm::vec3& a_up = glm::vec3(0, 0, 1),
			 Shape a_shape = NONE);
	Geometry(const glm::vec3& a_position,
			 const glm::vec3& a_axis, float a_angle,
			 Shape a_shape = NONE);
	Geometry(const glm::vec3& a_position,
			 float a_yaw, float a_pitch, float a_roll,
			 const glm::vec3& a_yawAxis,
			 const glm::vec3& a_rollAxis,
			 Shape a_shape = NONE);
	Geometry(const glm::vec3& a_position,
			 float a_yaw, float a_pitch, float a_roll,
			 Shape a_shape);

private:

	glm::quat m_orientation;
	glm::mat4 m_rotationMatrix;
	Shape m_shape;
};

#include "Geometry_Shapes.h"
