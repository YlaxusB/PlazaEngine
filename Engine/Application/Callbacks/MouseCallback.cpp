#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"

using namespace Plaza;
using namespace Plaza::Editor;
bool Callbacks::rightClickPressed;
bool Callbacks::mouseFirstCallback;
bool Callbacks::firstMouse = true;

float Callbacks::lastX = 0;//Application::Get()->appSizes->appSize.x / 2.0f;
float Callbacks::lastY = 0;//Application::Get()->appSizes->appSize.y / 2.0f;



void Callbacks::mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	if (Application::Get()->focusedMenu == "Editor") {

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		// Prevent the camera from roughly moving to mouse position when pressed right button
		if (rightClickPressed && mouseFirstCallback) {
			mouseFirstCallback = false;
		}
		else if (rightClickPressed && !mouseFirstCallback) {
			Application::Get()->activeCamera->ProcessMouseMovement(xoffset, yoffset);
			mouseFirstCallback = false;
		}
		else if (!rightClickPressed) {
			mouseFirstCallback = true;
		}

		lastX = xpos;
		lastY = ypos;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
		if (xpos >= Application::Get()->appSizes->appSize.x - 1.0f) {
			glfwSetCursorPos(window, 0.0, yposIn);
			lastX = 0.0;
		}
		if (xpos <= 1.0f) {
			glfwSetCursorPos(window, Application::Get()->appSizes->appSize.x, yposIn);
			lastX = Application::Get()->appSizes->appSize.x;
		}
		if (ypos >= Application::Get()->appSizes->appSize.y - 1.0f) {
			glfwSetCursorPos(window, xposIn, 0.0);
			lastY = 0.0;
		}
		if (ypos <= 1.0f) {
			glfwSetCursorPos(window, xposIn, Application::Get()->appSizes->appSize.y);
			lastY = Application::Get()->appSizes->appSize.y;
		}

	}
}

