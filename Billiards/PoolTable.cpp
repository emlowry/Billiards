#include "PoolTable.h"
#include "Engine.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

PoolTable::PoolTable() {}
PoolTable::~PoolTable() {}

void PoolTable::Start()
{
	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	glm::vec2 windowSize = Engine::GetWindowSize();
	Engine::PushProjectionMatrix(glm::perspective(glm::pi<float>() * 0.25f, windowSize.x / windowSize.y, 0.1f, 1000.0f));

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(20,20,20),glm::vec3(0,0,0), glm::vec3(0,1,0)) );

	// set up pool table
	Actor::Material felt(1.0f, 0.5f, 2.0f, 2.0f);
	Actor::Material wood(1.0f, 0.9f, 0.9f, 0.9f);
	Actor::Texture green(glm::vec4(0, 0.625f, 0.125f, 1), 0.0f);
	/*AddActor(new Actor(Geometry::Plane(40, 1.0f, glm::vec3(0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1)),
					   false, felt, Texture(0, 1, 0, 0, 0)));/**/
	AddActor(new Actor(Geometry::Box(glm::vec3(10, 1, 19.5), glm::vec3(0, -1, 0)), false, felt, green));
	AddActor(new Actor(Geometry::Box(glm::vec3(1, 1, 8), glm::vec3(11, 1, 9.5)), false, felt, green));
	AddActor(new Actor(Geometry::Box(glm::vec3(1, 1, 8), glm::vec3(11, 1, -9.5)), false, felt, green));
	AddActor(new Actor(Geometry::Box(glm::vec3(1, 1, 8), glm::vec3(-11, 1, 9.5)), false, felt, green));
	AddActor(new Actor(Geometry::Box(glm::vec3(1, 1, 8), glm::vec3(-11, 1, -9.5)), false, felt, green));
	AddActor(new Actor(Geometry::Box(glm::vec3(8, 1, 1), glm::vec3(0, 1, 20.5)), false, felt, green));
	AddActor(new Actor(Geometry::Box(glm::vec3(8, 1, 1), glm::vec3(0, 1, -20.5)), false, felt, green));

	// add balls
	m_cueBall = nullptr;
	for (unsigned int i = 0; i < BALL_COUNT; ++i)
		m_balls[i] = nullptr;
	Setup();
	m_threshold = -1.5f;
}

void PoolTable::ClearBalls()
{
	if (nullptr != m_cueBall)
	{
		DestroyActor(m_cueBall);
		m_cueBall = nullptr;
	}
	for (unsigned int i = 0; i < BALL_COUNT; ++i)
	{
		if (nullptr != m_balls[i])
		{
			DestroyActor(m_balls[i]);
			m_balls[i] = nullptr;
		}
	}
}
void PoolTable::Setup()
{
	// remove existing balls
	ClearBalls();

	// create balls
	Actor::Material ivory;
	m_cueBall = new Actor(Geometry::Sphere(1, glm::vec3(0, 1, 10)),
						  ivory, Actor::Texture(glm::vec4(1), 1));
	m_balls[0] = new Actor(Geometry::Sphere(1, glm::vec3(0, 1, -10)),
						   ivory, Actor::Texture(glm::vec4(1, 1, 0, 1), 1));
	m_balls[1] = new Actor(Geometry::Sphere(1, glm::vec3(1.01, 1, -11.75)),
						   ivory, Actor::Texture(glm::vec4(1, 0, 0, 0.5), 1));
	m_balls[2] = new Actor(Geometry::Sphere(1, glm::vec3(-1.01, 1, -11.75)),
						   ivory, Actor::Texture(glm::vec4(0, 0, 1, 0.5), 1));
	m_balls[3] = new Actor(Geometry::Sphere(1, glm::vec3(2.02, 1, -13.5)),
						   ivory, Actor::Texture(glm::vec4(1, 1, 0, 0.5), 1));
	m_balls[4] = new Actor(Geometry::Sphere(1, glm::vec3(0, 1, -13.5)),
						   ivory, Actor::Texture(glm::vec4(0, 0, 0, 1), 1));
	m_balls[5] = new Actor(Geometry::Sphere(1, glm::vec3(-2.02, 1, -13.5)),
						   ivory, Actor::Texture(glm::vec4(0, 1, 0, 1), 1));
	m_balls[6] = new Actor(Geometry::Sphere(1, glm::vec3(3.03, 1, -15.25)),
						   ivory, Actor::Texture(glm::vec4(1, 0.5, 0, 1), 1));
	m_balls[7] = new Actor(Geometry::Sphere(1, glm::vec3(1.01, 1, -15.25)),
						   ivory, Actor::Texture(glm::vec4(0.5, 0, 1, 0.5), 1));
	m_balls[8] = new Actor(Geometry::Sphere(1, glm::vec3(-1.01, 1, -15.25)),
						   ivory, Actor::Texture(glm::vec4(1, 0, 0.5, 1), 1));
	m_balls[9] = new Actor(Geometry::Sphere(1, glm::vec3(-3.03, 1, -15.25)),
						   ivory, Actor::Texture(glm::vec4(1, 0.5, 0, 0.5), 1));
	m_balls[10] = new Actor(Geometry::Sphere(1, glm::vec3(4.04, 1, -17)),
							ivory, Actor::Texture(glm::vec4(1, 0, 0.5, 0.5), 1));
	m_balls[11] = new Actor(Geometry::Sphere(1, glm::vec3(2.02, 1, -17)),
							ivory, Actor::Texture(glm::vec4(0, 0, 1, 1), 1));
	m_balls[12] = new Actor(Geometry::Sphere(1, glm::vec3(0, 1, -17)),
							ivory, Actor::Texture(glm::vec4(0, 1, 0, 0.5), 1));
	m_balls[13] = new Actor(Geometry::Sphere(1, glm::vec3(-2.02, 1, -17)),
							ivory, Actor::Texture(glm::vec4(1, 0, 0, 1), 1));
	m_balls[14] = new Actor(Geometry::Sphere(1, glm::vec3(-4.04, 1, -17)),
							ivory, Actor::Texture(glm::vec4(0.5, 0, 1, 1), 1));

	// add balls to scene
	AddActor(m_cueBall);
	for (auto ball : m_balls)
		AddActor(ball);
	m_aiming = m_cued = false;
}

void PoolTable::Update()
{
	// update our camera matrix using the keyboard/mouse
	//Utility::freeMovement( m_cameraMatrix, Engine::GetDeltaTime(), 10 );
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);
	Engine::SwapViewMatrix(viewMatrix);

	Scene::Update();
	if (!m_cued)
	{
		GLFWwindow* window = glfwGetCurrentContext();
		// get window dimensions to calculate aspect ratio
		int width = 0, height = 0;
		glfwGetWindowSize(Engine::GetWindow(), &width, &height);
		double mouseX = 0, mouseY = 0;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		glm::vec3 screenCoord(mouseX, (float)height - mouseY, 0);
		glm::vec4 viewPort = glm::vec4(0.f, 0.f, width, height);
		glm::vec3 worldPos = glm::unProject(screenCoord, viewMatrix, Engine::GetProjectionMatrix(), viewPort);
		glm::vec3 rayOrigin = m_cameraMatrix[3].xyz();
		glm::vec3 rayDirection = glm::normalize(worldPos - m_cameraMatrix[3].xyz());
		glm::vec3 up(0, 1, 0);
		float d = glm::dot(rayDirection, up);
		if (0 != d)
		{
			glm::vec3 cue = rayOrigin + rayDirection * glm::dot(up - rayOrigin, up) / d;
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
			{
				m_aiming = true;
				//Gizmos::addLine(cue, m_cueBall->GetPosition(), glm::vec4(1));
			}
			else if (m_aiming)
			{
				m_aiming = false;
				m_cued = true;
				glm::vec3 aim = m_cueBall->GetPosition() - cue;
				m_cueBall->ApplyImpulse(aim * 2.0f * m_cueBall->GetMass(),
										m_cueBall->GetGeometry().ClosestSurfacePointTo(cue));
			}
		}
	}
	else
	{
		if (m_cueBall->GetPosition().y < m_threshold)
			Setup();
		bool still = glm::vec3(0) == m_cueBall->GetVelocity();
		unsigned int remainingBalls = 0;
		for (unsigned int i = 0; i < BALL_COUNT; ++i)
		{
			if (nullptr == m_balls[i])
				continue;
			++remainingBalls;
			if (m_balls[i]->GetPosition().y < m_threshold)
			{
				DestroyActor(m_balls[i]);
				m_balls[i] = nullptr;
				--remainingBalls;
			}
			else if (still)
			{
				still = glm::vec3(0) == m_balls[i]->GetVelocity();
			}
		}
		if (0 == remainingBalls)
		{
			Setup();
		}
		else if (still)
		{
			m_aiming = m_cued = false;
		}
	}

	// quit our application when escape is pressed
	if (glfwGetKey(Engine::GetWindow(),GLFW_KEY_ESCAPE) == GLFW_PRESS)
		Engine::Quit();
}

void PoolTable::Stop()
{
	ClearBalls();
}