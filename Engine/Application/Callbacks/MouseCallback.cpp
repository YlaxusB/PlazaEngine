#include "CallbacksHeader.h"
#include "Engine/Application/EntryPoint.h"

using namespace Engine;
using namespace Engine::Editor;
bool ApplicationClass::Callbacks::rightClickPressed;
bool ApplicationClass::Callbacks::mouseFirstCallback;
bool ApplicationClass::Callbacks::firstMouse = true;

float ApplicationClass::Callbacks::lastX = 0;//Application->appSizes->appSize.x / 2.0f;
float ApplicationClass::Callbacks::lastY = 0;//Application->appSizes->appSize.y / 2.0f;



void ApplicationClass::Callbacks::mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	// Prevent the camera from roughly moving to mouse position when pressed right button
	if (rightClickPressed && mouseFirstCallback) {
		mouseFirstCallback = false;
	}
	else if (rightClickPressed && !mouseFirstCallback) {
		Application->activeCamera->ProcessMouseMovement(xoffset, yoffset);
		mouseFirstCallback = false;
	}
	else if (!rightClickPressed) {
		mouseFirstCallback = true;
	}
	lastX = xpos;
	lastY = ypos;
}

