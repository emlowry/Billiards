#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include <functional>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

namespace Engine
{
	const glm::mat4 IDENTITY_MATRIX = glm::mat4(1, 0, 0, 0,
												0, 1, 0, 0,
												0, 0, 1, 0,
												0, 0, 0, 1);

	void Reset();
	void Run();

	bool IsRunning();
	void Quit();

	double GetElapsedTime();
	double GetDeltaTime();

	GLFWwindow* GetWindow();
	glm::vec2 GetWindowSize();
	void SetWindowSize(const glm::vec2& a_size);
	std::string GetWindowTitle();
	void SetWindowTitle(const std::string& a_title);

	enum Matrix { PROJECTION_MATRIX, VIEW_MATRIX };
	const glm::mat4& GetMatrix(Matrix a_matrix);
	void PushMatrix(Matrix a_matrix, const glm::mat4& a_data = IDENTITY_MATRIX);
	glm::mat4 PopMatrix(Matrix a_matrix);
	glm::mat4 SwapMatrix(Matrix a_matrix, const glm::mat4& a_data = IDENTITY_MATRIX);

	const glm::mat4& GetProjectionMatrix();
	void PushProjectionMatrix(const glm::mat4& a_data = IDENTITY_MATRIX);
	glm::mat4 PopProjectionMatrix();
	glm::mat4 SwapProjectionMatrix(const glm::mat4& a_data = IDENTITY_MATRIX);

	const glm::mat4& GetViewMatrix();
	void PushViewMatrix(const glm::mat4& a_data = IDENTITY_MATRIX);
	glm::mat4 PopViewMatrix();
	glm::mat4 SwapViewMatrix(const glm::mat4& a_data = IDENTITY_MATRIX);

	const glm::mat4& GetProjectionViewMatrix();

	typedef std::function<void(void)> Action;
	enum Phase { START_PHASE, UPDATE_PHASE, DRAW_PHASE, STOP_PHASE };
	void RegisterAction(Phase a_phase, Action& a_action);
	void DeregisterAction(Phase a_phase, Action& a_action);

	void RegisterStartAction(Action& a_action);
	void DeregisterStartAction(Action& a_action);
	void RegisterUpdateAction(Action& a_action);
	void DeregisterUpdateAction(Action& a_action);
	void RegisterDrawAction(Action& a_action);
	void DeregisterDrawAction(Action& a_action);
	void RegisterStopAction(Action& a_action);
	void DeregisterStopAction(Action& a_action);
}

#endif	// _ENGINE_H_
