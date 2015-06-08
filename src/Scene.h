#ifndef _SCENE_H_
#define _SCENE_H_

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Actor.h"
#include "Engine.h"
#include <set>

class Scene
{
public:

	Scene(const glm::vec3& a_gravity = glm::vec3(0.0f, -9.81f, 0.0f),
		  double a_timeStep = 0.01)
		: m_gravity(a_gravity), m_timeStep(a_timeStep),
		m_lastUpdate(Engine::GetElapsedTime()) {}
	~Scene() { ClearActors(); }

	void AddActor(Actor* a_actor);
	void ClearActors();
	bool DestroyActor(Actor* a_actor);	// returns false if actor not in scene
	const std::set<Actor*>& GetActors() const { return m_actors; }
	bool HasActor(Actor* a_actor) const { return nullptr != a_actor && 0 != m_actors.count(a_actor); }
	void QueueMeshes() const;

	virtual void Update();


protected:

	glm::vec3 m_gravity;
	double m_timeStep;
	double m_lastUpdate;

	std::set<Actor*> m_actors;

};

#endif	// _SCENE_H_
