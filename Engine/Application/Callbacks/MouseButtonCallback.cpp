#include "CallbacksHeader.h"
#include "Engine/Vendor/imgui/ImGuizmo.h"
#include "Engine/Application/Application.h"
#include "Engine/Application/ApplicationSizes.h"
#include "Engine/GUI/guiMain.h"
using namespace Engine::Editor;
using namespace Engine;
void ApplicationClass::Callbacks::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	ApplicationSizes& appSizes = Application->appSizes;
	ApplicationSizes& lastAppSizes = Application->lastAppSizes;

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

	// Select nothing if didnt clicked on a GameObject
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver() && pickingTexture->readPixel(xposGame, yposGame) == 0) {
		Gui::changeSelectedGameObject(nullptr);
	}


	// Select the GameObject
	// Mouse is over imguizmo but imguizmo is not drawing || Mouse inst over imguizmo and its being drawn
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !ImGuizmo::IsUsing() && ((ImGuizmo::IsOver() && !ImGuizmo::IsDrawing) || !(ImGuizmo::IsOver() && ImGuizmo::IsDrawing)) && pickingTexture->readPixel(xposGame, yposGame) != 0 && (!ImGuizmo::IsOver() && !ImGuizmo::IsDrawing)) {
		int targetName = pickingTexture->readPixel(xposGame, yposGame);
		auto it = std::find_if(gameObjects.begin(), gameObjects.end(), [&](const GameObject* obj) {
			return obj->id == targetName;
			});
		if (it != gameObjects.end()) {
			// Object with the specified name found
			Gui::changeSelectedGameObject(*it);
		}
	}
#pragma endregion Picking
}