#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Core/Time.h"

#include "Engine/Application/Serializer/Components/MeshSerializer.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"
#include "Engine/Core/Physics.h"

void Callbacks::processInput(GLFWwindow* window) {
	if (Application->focusedMenu == "Editor") {
		ApplicationSizes& appSizes = *Application->appSizes;
		ApplicationSizes& lastAppSizes = *Application->lastAppSizes;

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			Application->activeCamera->MovementSpeedTemporaryBoost = 5.0f;

		if (Application->activeCamera->isEditorCamera) {
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::FORWARD, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::BACKWARD, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::LEFT, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::RIGHT, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::UP, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::DOWN, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::UP, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::DOWN, Time::deltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
			Application->activeCamera->MovementSpeedTemporaryBoost = 1.0f;
	}
}
