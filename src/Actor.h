#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "Geometry.h"
#include "Mesh.h"
#include "Texture.h"
#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Actor
{
public:

	struct Material
	{
		float density;
		float elasticity;
		float linearDrag;
		float rotationalDrag;
		float staticFriction;
		float dynamicFriction;
		Material(float a_density = 1.0f, float a_elasticity = 0.9f,
				 float a_staticFriction = 1.0f, float a_dynamicFriction = 1.0f,
				 float a_linearDrag = 0.5f, float a_rotationalDrag = 0.5f)
			: density(a_density), elasticity(a_elasticity),
			  linearDrag(a_linearDrag), rotationalDrag(a_rotationalDrag),
			  staticFriction(a_staticFriction), dynamicFriction(a_dynamicFriction) {}
	};

	Actor(const Geometry& a_geometry,
		  const Mesh& a_mesh,
		  bool a_dynamic = false,
		  const Material& a_material = Material(),
		  const Texture& a_texture = Texture(),
		  const glm::vec3& a_velocity = glm::vec3(0),
		  const glm::vec3& a_angularVelocity = glm::vec3(0),
		  float a_mass = 0.0f,
		  const glm::mat3& a_inertiaTensor = glm::mat3(0),
		  float a_minSpeed = 0.1f,
		  float a_minAngularSpeed = 0.05f)
		: m_mesh(a_mesh), m_texture(a_texture), m_geometry(a_geometry.Clone()), m_dynamic(a_dynamic),
		  m_material(a_material), m_velocity(a_velocity), m_angularVelocity(a_angularVelocity),
		  m_mass(a_mass), m_inertiaTensor(a_inertiaTensor), m_force(0), m_torque(0),
		  m_minSpeed2(a_minSpeed * a_minSpeed), m_minAngularSpeed2(a_minAngularSpeed * a_minAngularSpeed) {}
	Actor(const Geometry& a_geometry,
		  const Mesh& a_mesh,
		  const Material& a_material,
		  const Texture& a_texture = Texture(),
		  const glm::vec3& a_velocity = glm::vec3(0),
		  const glm::vec3& a_angularVelocity = glm::vec3(0),
		  float a_mass = 0.0f,
		  const glm::mat3& a_inertiaTensor = glm::mat3(0),
		  float a_minSpeed = 0.1f,
		  float a_minAngularSpeed = 0.05f)
		: m_mesh(a_mesh), m_texture(a_texture), m_geometry(a_geometry.Clone()), m_dynamic(true),
		  m_material(a_material), m_velocity(a_velocity), m_angularVelocity(a_angularVelocity),
		  m_mass(a_mass), m_inertiaTensor(a_inertiaTensor), m_force(0), m_torque(0),
		  m_minSpeed2(a_minSpeed * a_minSpeed), m_minAngularSpeed2(a_minAngularSpeed * a_minAngularSpeed) {}
	~Actor() { delete m_geometry; m_geometry = nullptr; }

	virtual void Update(double a_deltaTime, const glm::vec3& a_gravity = glm::vec3(0));
	void QueueMesh() const { Renderer::QueueMesh(m_mesh, m_texture, m_geometry->modelMatrix()); }

	const glm::vec3& GetPosition() const { return m_geometry->position; }
	const glm::quat& GetOrientation() const { return m_geometry->orientation(); }
	const Geometry& GetGeometry() const { return *m_geometry; }
	Geometry& GetGeometry() { return *m_geometry; }
	const glm::vec3& GetVelocity() const { return m_velocity; }
	const glm::vec3& GetAngularVelocity() const { return m_angularVelocity; }
	glm::vec3 GetPointVelocity(const glm::vec3& a_point, bool a_ignoreOutside = true) const;
	float GetMass() const
	{
		return (0 != m_mass ? m_mass : m_material.density * m_geometry->volume());
	}
	glm::mat3 GetInertiaTensor() const
	{
		return glm::mat3(0) != m_inertiaTensor ? m_inertiaTensor :
				m_geometry->interiaTensorDividedByMass() * GetMass();
	}
	float GetRotationalInertia(const glm::vec3& a_axis) const
	{
		if (glm::vec3(0) == a_axis)
			return 0;
		glm::vec3 axis = glm::normalize(a_axis);
		return glm::dot(axis, GetInertiaTensor() * axis);
	}
	bool IsDynamic() const { return m_dynamic; }

	void SetMass(float a_mass = 0.0f) { m_mass = a_mass; }
	void SetPosition(const glm::vec3& a_position = glm::vec3(0))
	{
		m_geometry->position = a_position;
	}
	void SetOrientation(const glm::quat& a_orientation = glm::quat(0, glm::vec3(0)))
	{
		m_geometry->orientation(a_orientation);
	}
	void SetVelocity(const glm::vec3& a_velocity = glm::vec3(0))
	{
		m_velocity = a_velocity;
	}
	void SetAngularVelocity(const glm::vec3& a_angularVelocity = glm::vec3(0))
	{
		m_angularVelocity = a_angularVelocity;
	}
	void Move(const glm::vec3& a_displacement = glm::vec3(0))
	{
		m_geometry->position += a_displacement;
	}
	void Spin(const glm::vec3& a_rotation = glm::vec3(0))
	{
		m_geometry->spin(a_rotation);
	}
	void Accelerate(const glm::vec3& a_deltaV = glm::vec3(0))
	{
		m_velocity += a_deltaV;
	}
	void AccelerateRotation(const glm::vec3& a_deltaAV = glm::vec3(0))
	{
		m_angularVelocity += a_deltaAV;
	}

	void ApplyImpulse(const glm::vec3& a_impulse, const glm::vec3& contactPoint);
	void ApplyLinearImpulse(const glm::vec3& a_impulse);
	void ApplyAngularImpulse(const glm::vec3& a_angularImpulse);

	void EnforceMinSpeed();

	static void ResolveCollision(Actor* a_actor1, Actor* a_actor2);

protected:

	glm::vec4 m_color;
	Geometry* m_geometry;
	Mesh m_mesh;
	bool m_dynamic;
	float m_mass;
	glm::mat3 m_inertiaTensor;
	Material m_material;
	Texture m_texture;
	glm::vec3 m_velocity;
	glm::vec3 m_angularVelocity;
	glm::vec3 m_force;
	glm::vec3 m_torque;
	float m_minSpeed2;
	float m_minAngularSpeed2;
};

#endif	// _ACTOR_H_
