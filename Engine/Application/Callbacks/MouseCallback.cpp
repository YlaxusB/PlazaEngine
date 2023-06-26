#include "CallbacksHeader.h"
using namespace Engine;
bool Application::Callbacks::rightClickPressed;
bool Application::Callbacks::mouseFirstCallback;
bool Application::Callbacks::firstMouse = true;

float Application::Callbacks::lastX = Engine::Application::appSizes.appSize.x / 2.0f;
float Application::Callbacks::lastY = Engine::Application::appSizes.appSize.y / 2.0f;



void Engine::Application::Callbacks::mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	// Prevent the camera from roughly moving to mouse position when pressed right button
	if (rightClickPressed && mouseFirstCallback) {
		mouseFirstCallback = false;
	}
	else if (rightClickPressed && !mouseFirstCallback) {
		Application::activeCamera.ProcessMouseMovement(xoffset, yoffset);
		mouseFirstCallback = false;
	}
	else if (!rightClickPressed) {
		mouseFirstCallback = true;
	}
	lastX = xpos;
	lastY = ypos;
}

