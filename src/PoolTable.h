#ifndef _POOL_TABLE_H_
#define _POOL_TABLE_H_

#include "Scene.h"
#include "Mesh.h"
#include <glm/glm.hpp>

// derived application class that wraps up all globals neatly
class PoolTable : public Scene
{
public:

	PoolTable();
	virtual ~PoolTable();

	void Start();
	virtual void Update();
	virtual void Draw();
	void Stop();

protected:

	struct DataPoint
	{
		glm::vec3 position;
		glm::vec4 color;

		DataPoint(const glm::vec3& a_position, const glm::vec4& a_color)
			: position(a_position), color(a_color) {}
	};

	void ClearBalls();
	void Setup();

	glm::mat4	m_cameraMatrix;

	static const unsigned int BALL_COUNT = 15;

	Mesh m_ballMesh;
	Mesh m_boxMesh;
	Actor* m_cueBall;
	Texture m_cueBallTexture;
	Actor* m_balls[BALL_COUNT];
	Texture m_ballTextures[BALL_COUNT];
	bool m_aiming;
	bool m_cued;
	float m_threshold;
};

#endif	// _POOL_TABLE_H_
