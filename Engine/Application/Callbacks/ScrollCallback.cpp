#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Core/Time.h"
void Engine::ApplicationClass::Callbacks::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	double mousePosX = 0;
	double mousePosY = 0;
	glfwGetCursorPos(window, &mousePosX, &mousePosY);
	glm::vec2 mousePos = glm::vec2(mousePosX, mousePosY);
	if (Application->focusedMenu == "Scene" && glm::isInside(mousePos, Application->appSizes->sceneStart, Application->appSizes->sceneStart + Application->appSizes->sceneSize)) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			Application->activeCamera->MovementSpeed += Application->activeCamera->MovementSpeed * (yoffset * 3) * Time::deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
			Application->activeCamera->ProcessMouseScroll(static_cast<float>(yoffset));
	}
}