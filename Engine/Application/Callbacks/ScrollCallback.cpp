#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Core/Time.h"
#include "Engine/Utils/glmUtils.h"
void Plaza::ApplicationClass::Callbacks::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	double mousePosX = 0;
	double mousePosY = 0;
	glfwGetCursorPos(window, &mousePosX, &mousePosY);
	glm::vec2 mousePos = glm::vec2(mousePosX, mousePosY);
	if (Application->focusedMenu == "Editor" && glm::isInside(mousePos, Application->appSizes->sceneStart, Application->appSizes->sceneStart + Application->appSizes->sceneSize)) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			Application->activeCamera->MovementSpeed = glm::max(Application->activeCamera->MovementSpeed + (Application->activeCamera->MovementSpeed * yoffset / 10), 0.0001);
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
			Application->activeCamera->ProcessMouseScroll(static_cast<float>(yoffset));
	}
}