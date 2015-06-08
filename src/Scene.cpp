#include "Scene.h"
#include "Engine.h"

void Scene::AddActor(Actor* a_actor)
{
	if (nullptr != a_actor)
		m_actors.insert(a_actor);
}
void Scene::ClearActors()
{
	while (!m_actors.empty())
	{
		Actor* actor = *m_actors.begin();
		m_actors.erase(actor);
		if (nullptr != actor)
			delete actor;
	}
}
bool Scene::DestroyActor(Actor* a_actor)	// returns false if actor not in scene
{
	if (nullptr == a_actor || 0 == m_actors.count(a_actor))
		return false;
	m_actors.erase(a_actor);
	delete a_actor;
	return true;
}

void Scene::Update()
{
	double time = Engine::GetElapsedTime();
	while (time - m_lastUpdate >= m_timeStep)
	{
		// standard physics update
		m_lastUpdate += m_timeStep;
		for (auto actor : m_actors)
			actor->Update(m_timeStep, m_gravity);

		// collision resolution
		std::set<Actor*> unchecked(m_actors);
		while (1 < unchecked.size())
		{
			Actor* actor1 = *unchecked.begin();
			unchecked.erase(actor1);
			for (auto actor2 : unchecked)
				Actor::ResolveCollision(actor1, actor2);
		}
	}
}

void Scene::QueueMeshes() const
{
	for (auto actor : m_actors)
		actor->QueueMesh();
}