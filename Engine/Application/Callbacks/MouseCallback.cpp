#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"

using namespace Plaza;
using namespace Plaza::Editor;
bool Callbacks::rightClickPressed;
bool Callbacks::mouseFirstCallback;
bool Callbacks::firstMouse = true;

float Callbacks::lastX = 0;//Application->appSizes->appSize.x / 2.0f;
float Callbacks::lastY = 0;//Application->appSizes->appSize.y / 2.0f;



void Callbacks::mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
	if (Application->focusedMenu == "Editor") {
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
}

