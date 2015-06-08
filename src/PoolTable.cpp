#include "PoolTable.h"
#include "Engine.h"
#include "Mesh.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

PoolTable::PoolTable() {}
PoolTable::~PoolTable() {}

void PoolTable::Start()
{
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	glm::vec2 windowSize = Engine::GetWindowSize();
	Engine::PushProjectionMatrix(glm::perspective(glm::pi<float>() * 0.25f, windowSize.x / windowSize.y, 0.1f, 1000.0f));

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(20,20,20),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	Renderer::SetCameraPosition(m_cameraMatrix[3].xyz());

	// load meshes
	m_boxMesh = Mesh::GenerateCubeMesh();
	m_ballMesh = Mesh::GenerateSphereMesh();

	// add a light
	Renderer::Light light;
	light.color = glm::vec3(1);
	light.direction = glm::vec3(0, -1, 0);
	light.position = glm::vec3(0, 15, 0);
	light.power = 1;
	light.attenuation = 0.001f;
	light.angle = 60;
	light.blur = 0.1f;
	Renderer::AddLight(light);
	Renderer::SetAmbientLight(glm::vec3(0.1f));

	// set up pool table
	Actor::Material felt(1.0f, 0.5f, 2.0f, 2.0f);
	//Actor::Material wood(1.0f, 0.9f, 0.9f, 0.9f);
	Texture green(glm::vec4(0, 0.625f, 0.125f, 1), glm::vec4(0));
	/*AddActor(new Actor(Geometry::Plane(40, 1.0f, glm::vec3(0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1)),
					   false, felt, Texture(0, 1, 0, 0, 0)));/**/
	AddActor(new Actor(Geometry::Box(glm::vec3(10, 1, 19.5), glm::vec3(0, -1, 0)), m_boxMesh, false, felt, green));
	AddActor(new Actor(Geometry::Box(glm::vec3(1, 1, 8), glm::vec3(11, 1, 9.5)), m_boxMesh, false, felt, green));
	AddActor(new Actor(Geometry::Box(glm::vec3(1, 1, 8), glm::vec3(11, 1, -9.5)), m_boxMesh, false, felt, green));
	AddActor(new Actor(Geometry::Box(glm::vec3(1, 1, 8), glm::vec3(-11, 1, 9.5)), m_boxMesh, false, felt, green));
	AddActor(new Actor(Geometry::Box(glm::vec3(1, 1, 8), glm::vec3(-11, 1, -9.5)), m_boxMesh, false, felt, green));
	AddActor(new Actor(Geometry::Box(glm::vec3(8, 1, 1), glm::vec3(0, 1, 20.5)), m_boxMesh, false, felt, green));
	AddActor(new Actor(Geometry::Box(glm::vec3(8, 1, 1), glm::vec3(0, 1, -20.5)), m_boxMesh, false, felt, green));

	// load ball textures
	m_cueBallTexture = Texture("images/BallCue.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[0] = Texture("images/Ball1.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[1] = Texture("images/Ball2.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[2] = Texture("images/Ball3.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[3] = Texture("images/Ball4.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[4] = Texture("images/Ball5.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[5] = Texture("images/Ball6.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[6] = Texture("images/Ball7.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[7] = Texture("images/Ball8.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[8] = Texture("images/Ball9.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[9] = Texture("images/Ball10.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[10] = Texture("images/Ball11.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[11] = Texture("images/Ball12.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[12] = Texture("images/Ball13.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[13] = Texture("images/Ball14.jpg", glm::vec4(1), glm::vec4(1));
	m_ballTextures[14] = Texture("images/Ball15.jpg", glm::vec4(1), glm::vec4(1));

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
						  m_ballMesh, ivory, m_cueBallTexture);
	m_balls[0] = new Actor(Geometry::Sphere(1, glm::vec3(0, 1, -10)),
						   m_ballMesh, ivory, m_ballTextures[0]);
	m_balls[1] = new Actor(Geometry::Sphere(1, glm::vec3(1.01, 1, -11.75)),
						   m_ballMesh, ivory, m_ballTextures[1]);
	m_balls[2] = new Actor(Geometry::Sphere(1, glm::vec3(-1.01, 1, -11.75)),
						   m_ballMesh, ivory, m_ballTextures[2]);
	m_balls[3] = new Actor(Geometry::Sphere(1, glm::vec3(2.02, 1, -13.5)),
						   m_ballMesh, ivory, m_ballTextures[3]);
	m_balls[4] = new Actor(Geometry::Sphere(1, glm::vec3(0, 1, -13.5)),
						   m_ballMesh, ivory, m_ballTextures[4]);
	m_balls[5] = new Actor(Geometry::Sphere(1, glm::vec3(-2.02, 1, -13.5)),
						   m_ballMesh, ivory, m_ballTextures[5]);
	m_balls[6] = new Actor(Geometry::Sphere(1, glm::vec3(3.03, 1, -15.25)),
						   m_ballMesh, ivory, m_ballTextures[6]);
	m_balls[7] = new Actor(Geometry::Sphere(1, glm::vec3(1.01, 1, -15.25)),
						   m_ballMesh, ivory, m_ballTextures[7]);
	m_balls[8] = new Actor(Geometry::Sphere(1, glm::vec3(-1.01, 1, -15.25)),
						   m_ballMesh, ivory, m_ballTextures[8]);
	m_balls[9] = new Actor(Geometry::Sphere(1, glm::vec3(-3.03, 1, -15.25)),
						   m_ballMesh, ivory, m_ballTextures[9]);
	m_balls[10] = new Actor(Geometry::Sphere(1, glm::vec3(4.04, 1, -17)),
							m_ballMesh, ivory, m_ballTextures[10]);
	m_balls[11] = new Actor(Geometry::Sphere(1, glm::vec3(2.02, 1, -17)),
							m_ballMesh, ivory, m_ballTextures[11]);
	m_balls[12] = new Actor(Geometry::Sphere(1, glm::vec3(0, 1, -17)),
							m_ballMesh, ivory, m_ballTextures[12]);
	m_balls[13] = new Actor(Geometry::Sphere(1, glm::vec3(-2.02, 1, -17)),
							m_ballMesh, ivory, m_ballTextures[13]);
	m_balls[14] = new Actor(Geometry::Sphere(1, glm::vec3(-4.04, 1, -17)),
							m_ballMesh, ivory, m_ballTextures[14]);

	// add balls to scene
	AddActor(m_cueBall);
	for (auto ball : m_balls)
		AddActor(ball);
	m_aiming = m_cued = false;
}

// Copied from Utilities.cpp in AIEFramework
static void freeMovement(glm::mat4& a_transform, double a_deltaTime, double a_speed, const glm::vec3& a_up = glm::vec3(0,1,0))
{
	GLFWwindow* window = Engine::GetWindow();

	double frameSpeed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? a_deltaTime * a_speed * 2 : a_deltaTime * a_speed;

	// translate
	if (glfwGetKey(window, 'W') == GLFW_PRESS)
		a_transform[3] -= a_transform[2] * frameSpeed;
	if (glfwGetKey(window, 'S') == GLFW_PRESS)
		a_transform[3] += a_transform[2] * frameSpeed;
	if (glfwGetKey(window, 'D') == GLFW_PRESS)
		a_transform[3] += a_transform[0] * frameSpeed;
	if (glfwGetKey(window, 'A') == GLFW_PRESS)
		a_transform[3] -= a_transform[0] * frameSpeed;
	if (glfwGetKey(window, 'Q') == GLFW_PRESS)
		a_transform[3] += a_transform[1] * frameSpeed;
	if (glfwGetKey(window, 'E') == GLFW_PRESS)
		a_transform[3] -= a_transform[1] * frameSpeed;

	// check for rotation
	static bool sbMouseButtonDown = false;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		static double siPrevMouseX = 0;
		static double siPrevMouseY = 0;

		if (sbMouseButtonDown == false)
		{
			sbMouseButtonDown = true;
			glfwGetCursorPos(window, &siPrevMouseX, &siPrevMouseY);
		}

		double mouseX = 0, mouseY = 0;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		double iDeltaX = mouseX - siPrevMouseX;
		double iDeltaY = mouseY - siPrevMouseY;

		siPrevMouseX = mouseX;
		siPrevMouseY = mouseY;

		glm::mat4 mMat;

		// pitch
		if (iDeltaY != 0)
		{
			mMat = glm::axisAngleMatrix(a_transform[0].xyz(), (float)-iDeltaY / 150.0f);
			a_transform[0] = mMat * a_transform[0];
			a_transform[1] = mMat * a_transform[1];
			a_transform[2] = mMat * a_transform[2];
		}

		// yaw
		if (iDeltaX != 0)
		{
			mMat = glm::axisAngleMatrix(a_up, (float)-iDeltaX / 150.0f);
			a_transform[0] = mMat * a_transform[0];
			a_transform[1] = mMat * a_transform[1];
			a_transform[2] = mMat * a_transform[2];
		}
	}
	else
	{
		sbMouseButtonDown = false;
	}
}

void PoolTable::Update()
{
	// update our camera matrix using the keyboard/mouse
	freeMovement( m_cameraMatrix, Engine::GetDeltaTime(), 0.001 );
	Renderer::SetCameraPosition(m_cameraMatrix[3].xyz());
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);
	Engine::SwapViewMatrix(viewMatrix);

	Scene::Update();
	if (!m_cued)
	{
		// get window dimensions to calculate aspect ratio
		int width = 0, height = 0;
		glfwGetWindowSize(Engine::GetWindow(), &width, &height);
		double mouseX = 0, mouseY = 0;
		glfwGetCursorPos(Engine::GetWindow(), &mouseX, &mouseY);
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
			if (glfwGetMouseButton(Engine::GetWindow(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
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

void PoolTable::Draw()
{
	Renderer::ClearMeshQueue();
	QueueMeshes();
	Renderer::DrawQueuedMeshes();
}

void PoolTable::Stop()
{
	ClearBalls();
	m_boxMesh.Destroy();
	m_ballMesh.Destroy();
	m_cueBallTexture.Destroy();
	for (Texture texture : m_ballTextures)
		texture.Destroy();
}