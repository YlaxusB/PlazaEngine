#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Application/ApplicationSizes.h"
#include "Engine/GUI/guiMain.h"


using namespace Engine::Editor;
using namespace Engine;
int id(int x, int y) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, Application->frameBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT4);
	float pixels[3];
	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, pixels);
	std::cout << (int)pixels[0] << std::endl;
	return (int)pixels[0];
}


void ApplicationClass::Callbacks::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	ApplicationSizes& appSizes = *Application->appSizes;
	ApplicationSizes& lastAppSizes = *Application->lastAppSizes;

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		// Pressing right button
		rightClickPressed = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		// No more pressing right button
		rightClickPressed = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

#pragma region Picking
	float xposGame = lastX - appSizes.hierarchySize.x;
	float yposGame = lastY - appSizes.appHeaderSize;
	yposGame = appSizes.sceneSize.y - yposGame;
	int clickId = -1;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		//clickId = Application->pickingTexture->readPixel(xposGame, yposGame);
		clickId = id(xposGame, yposGame);
	}


	// Select nothing if didnt clicked on a GameObject
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver() && clickId == 0) {
		Engine::Editor::Gui::changeSelectedGameObject(nullptr);
	}


	// Select the GameObject
	// Mouse is over imguizmo but imguizmo is not drawing || Mouse inst over imguizmo and its being drawn
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !ImGuizmo::IsUsing() && ((ImGuizmo::IsOver() && !ImGuizmoHelper::IsDrawing) || !(ImGuizmo::IsOver() && ImGuizmoHelper::IsDrawing)) && clickId != 0 && (!ImGuizmo::IsOver() && !ImGuizmoHelper::IsDrawing)) {
		//int targetName = Application->pickingTexture->readPixel(xposGame, yposGame);


		auto it = std::find_if(gameObjects.begin(), gameObjects.end(), [&](const GameObject* obj) {
			return obj->id == clickId;
			});
		if (it != gameObjects.end()) {
			// Object with the specified name found
			//Engine::Editor::Gui::changeSelectedGameObject(*it);
		}
	}
#pragma endregion Picking
}