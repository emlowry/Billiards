#include "Engine.h"
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <stack>
#include <set>

namespace Engine
{

	//
	// TIME
	//

	static float sg_elapsedTime = 0;
	static float sg_deltaTime = 0;

	static void ResetTime()
	{
		sg_elapsedTime = 0;
		sg_deltaTime = 0;
	}

	static void Tick()
	{
		float time = glfwGetTime();
		sg_deltaTime = time - sg_elapsedTime;
		sg_deltaTime = time;
	}

	float Engine::GetElapsedTime() { return sg_elapsedTime; }
	float Engine::GetDeltaTime() { return sg_deltaTime; }


	//
	// WINDOW
	//

	static GLFWwindow* sg_window = nullptr;
	GLFWwindow* GetWindow() { return sg_window; }

	static glm::vec2 sg_windowSize = glm::vec2(1280, 760);
	static std::string sg_windowTitle = "My Game";

	static void CreateWindow()
	{
		if (nullptr == sg_window)
		{
			sg_window = glfwCreateWindow((int)sg_windowSize.x, (int)sg_windowSize.y,
										 sg_windowTitle.c_str(), nullptr, nullptr);
			if (nullptr != sg_window)
			{
				int width = 0, height = 0;
				glfwGetWindowSize(sg_window, &width, &height);
				sg_windowSize = glm::vec2(width, height);
			}
		}
	}

	glm::vec2 GetWindowSize() { return sg_windowSize; }
	void SetWindowSize(const glm::vec2& a_size)
	{
		sg_windowSize = a_size;
		if (nullptr != sg_window)
		{
			glfwSetWindowSize(sg_window, (int)sg_windowSize.x, (int)sg_windowSize.y);
			int width = 0, height = 0;
			glfwGetWindowSize(sg_window, &width, &height);
			sg_windowSize = glm::vec2(width, height);
		}
	}

	std::string GetWindowTitle() { return sg_windowTitle; }
	void SetWindowTitle(const std::string& a_title)
	{
		sg_windowTitle = a_title;
		if (nullptr != sg_window)
			glfwSetWindowTitle(sg_window, sg_windowTitle.c_str());
	}


	//
	// TRANSFORMS
	//

	const glm::mat4 IDENTITY_MATRIX = glm::mat4(1, 0, 0, 0,
												0, 1, 0, 0,
												0, 0, 1, 0,
												0, 0, 0, 1);

	typedef std::stack<glm::mat4> MatrixStack;
	static MatrixStack sg_projectionMatrix;
	static MatrixStack sg_viewMatrix;

	static void ResetMatrices()
	{
		while (!sg_projectionMatrix.empty())
			sg_projectionMatrix.pop();
		while (!sg_viewMatrix.empty())
			sg_viewMatrix.pop();
	}

	static const glm::mat4& Get(MatrixStack& a_stack)
	{
		return (a_stack.empty() ? IDENTITY_MATRIX : a_stack.top());
	}
	static void Push(MatrixStack& a_stack, const glm::mat4 a_data = IDENTITY_MATRIX)
	{
		a_stack.push(a_data);
	}
	static glm::mat4 Pop(MatrixStack& a_stack)
	{
		glm::mat4 data = Get(a_stack);
		a_stack.pop();
		return data;
	}
	static glm::mat4 Swap(MatrixStack& a_stack, const glm::mat4 a_data = IDENTITY_MATRIX)
	{
		glm::mat4 data = Pop(a_stack);
		Push(a_stack, a_data);
		return data;
	}

	static MatrixStack& GetStack(Matrix a_matrix)
	{
		return (VIEW_MATRIX == a_matrix ? sg_viewMatrix : sg_projectionMatrix);
	}
	const glm::mat4& GetMatrix(Matrix a_matrix)						{ return Get(GetStack(a_matrix)); }
	void PushMatrix(Matrix a_matrix, const glm::mat4& a_data)		{ Push(GetStack(a_matrix), a_data); }
	glm::mat4 PopMatrix(Matrix a_matrix)							{ return Pop(GetStack(a_matrix)); }
	glm::mat4 SwapMatrix(Matrix a_matrix, const glm::mat4& a_data)	{ return Swap(GetStack(a_matrix), a_data); }

	const glm::mat4& GetProjectionMatrix()					{ return Get(sg_projectionMatrix); }
	void PushProjectionMatrix(const glm::mat4 a_data)		{ Push(sg_projectionMatrix, a_data); }
	glm::mat4 PopProjectionMatrix()							{ return Pop(sg_projectionMatrix); }
	glm::mat4 SwapProjectionMatrix(const glm::mat4 a_data)	{ return Swap(sg_projectionMatrix, a_data); }

	const glm::mat4& GetViewMatrix()					{ return Get(sg_viewMatrix); }
	void PushViewMatrix(const glm::mat4 a_data)			{ Push(sg_viewMatrix, a_data); }
	glm::mat4 PopViewMatrix()							{ return Pop(sg_viewMatrix); }
	glm::mat4 SwapViewMatrix(const glm::mat4 a_data)	{ return Swap(sg_viewMatrix, a_data); }


	//
	// ACTIONS
	//

	typedef std::set<Action*> ActionSet;
	static ActionSet sg_startActions;
	static ActionSet sg_updateActions;
	static ActionSet sg_drawActions;
	static ActionSet sg_stopActions;

	static bool HasUpdateActions() { return !sg_updateActions.empty(); }
	static void ResetActions()
	{
		sg_startActions.clear();
		sg_updateActions.clear();
		sg_drawActions.clear();
		sg_stopActions.clear();
	}

	static void Act(ActionSet& a_set)
	{
		for each (Action* action in a_set)
			action();
	}
	static void StartActions()		{ Act(sg_startActions); }
	static void UpdateActions()		{ Act(sg_updateActions); }
	static void DrawActions()		{ Act(sg_drawActions); }
	static void StopActions()		{ Act(sg_stopActions); }

	static void Register(ActionSet& a_set, Action a_action)
	{
		if (0 == a_set.count(a_action))
			a_set.insert(a_action);
	}
	static void Deregister(ActionSet& a_set, Action a_action)
	{
		if (0 < a_set.count(a_action))
			a_set.erase(a_action);
	}

	static ActionSet& GetSet(Phase a_phase)
	{
		return (START_PHASE == a_phase ? sg_startActions :
				DRAW_PHASE == a_phase ? sg_drawActions :
				STOP_PHASE == a_phase ? sg_stopActions :
				sg_updateActions);
	}
	void RegisterAction(Phase a_phase, Action a_action)		{ Register(GetSet(a_phase), a_action); }
	void DeregisterAction(Phase a_phase, Action a_action)	{ Deregister(GetSet(a_phase), a_action); }

	void RegisterStartAction(Action a_action)		{ Register(sg_startActions, a_action); }
	void DeregisterStartAction(Action a_action)		{ Deregister(sg_startActions, a_action); }
	void RegisterUpdateAction(Action a_action)		{ Register(sg_updateActions, a_action); }
	void DeregisterUpdateAction(Action a_action)	{ Deregister(sg_updateActions, a_action); }
	void RegisterDrawAction(Action a_action)		{ Register(sg_drawActions, a_action); }
	void DeregisterDrawAction(Action a_action)		{ Deregister(sg_drawActions, a_action); }
	void RegisterStopAction(Action a_action)		{ Register(sg_stopActions, a_action); }
	void DeregisterStopAction(Action a_action)		{ Deregister(sg_stopActions, a_action); }

	//
	// RUN
	//

	static bool sg_running = false;

	bool IsRunning() { return sg_running; }
	void Quit() { sg_running = false; }

	static bool Start()
	{
		// start glfw
		if (glfwInit() != GL_TRUE)
			return false;

		// create window
		CreateWindow();
		if (nullptr == GetWindow())
		{
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(GetWindow());

		// initialise glew
		glewExperimental = GL_TRUE;
		if (GLEW_OK != glewInit())
		{
			glfwTerminate();
			return false;
		}

		// set window resize callback
		glfwSetWindowSizeCallback(GetWindow(), [](GLFWwindow*, int w, int h){ glViewport(0, 0, w, h); });

		// reset timer
		ResetTime();

		// do start actions
		try
		{
			StartActions();
		}
		catch (...)
		{
			glfwTerminate();
			return false;
		}

		// engine is now running!
		sg_running = true;
		return true;
	}	// static void Start()

	static void Update()
	{
		try
		{
			// update
			Tick();
			UpdateActions();
		}
		catch (...)
		{
			Quit();
		}

		// check for GLFW events
		glfwPollEvents();

		// quit if window is closed or there are no actions to take
		if (0 == glfwWindowShouldClose(GetWindow()) || !HasUpdateActions())
			Quit();
	}

	static void Draw()
	{
		// clear the backbuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw
		DrawActions();

		// swap buffers
		glfwSwapBuffers(GetWindow());
	}

	static void Stop()
	{
		// do stop actions
		StopActions();

		// terminate
		glfwTerminate();
	}

	void Reset()
	{
		Quit();
		ResetActions();
		ResetMatrices();
		ResetTime();
	}

	void Run()
	{
		if (IsRunning())
			return;
		if (Start())
		{
			while (IsRunning())
			{
				Update();
				Draw();
			}
		}
		Stop();
	}

}	// namespace Engine
