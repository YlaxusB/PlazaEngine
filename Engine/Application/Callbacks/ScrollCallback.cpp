#include "CallbacksHeader.h"
#include "Engine/Core/Time.h"
#include "Engine/Application/Application.h"
void Engine::ApplicationClass::Callbacks::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		Application->activeCamera.MovementSpeed += Application->activeCamera.MovementSpeed * (yoffset * 3) * Time::deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
		Application->activeCamera.ProcessMouseScroll(static_cast<float>(yoffset));
}