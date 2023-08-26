#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Application/ApplicationSizes.h"
#include "Editor/GUI/guiMain.h"


using namespace Plaza::Editor;
using namespace Plaza;

void ApplicationClass::Callbacks::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (Application->hoveredMenu == "Scene") {
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
		float yposGame = lastY - appSizes.appHeaderSize + 5;
		yposGame = appSizes.sceneSize.y - yposGame;
		yposGame = appSizes.sceneSize.y - (lastY - appSizes.sceneImageStart.y);
		int clickId = -1;
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && (!ImGuizmo::IsUsing() || (ImGuizmo::IsOver() && !ImGuizmo::IsUsing))) {
			Application->pickingTexture->GenerateTexture();
			clickId = Application->pickingTexture->readPixel(xposGame, yposGame);
		}


		// Select nothing if didnt clicked on a Entity
		if (button == GLFW_MOUSE_BUTTON_LEFT && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver() && clickId <= 0) {
			Plaza::Editor::Gui::changeSelectedGameObject(nullptr);
		}


		// Select the Entity
		// Mouse is over imguizmo but imguizmo is not drawing || Mouse inst over imguizmo and its being drawn
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !ImGuizmo::IsUsing() && ((ImGuizmo::IsOver() && !ImGuizmoHelper::IsDrawing) || !(ImGuizmo::IsOver() && ImGuizmoHelper::IsDrawing)) && clickId != 0 && (!ImGuizmo::IsOver() && !ImGuizmoHelper::IsDrawing)) {
			//
			//int targetName = Application->pickingTexture->readPixel(xposGame, yposGame);


			auto it = std::find_if(Application->activeScene->gameObjects.begin(), Application->activeScene->gameObjects.end(), [&](const std::unique_ptr<Entity>& obj) {
				return obj->uuid == clickId;
				});
			if (it != Application->activeScene->gameObjects.end()) {
				// Object with the specified name found
				Plaza::Editor::Gui::changeSelectedGameObject(it->get());
			}
		}
#pragma endregion Picking
	}
	else if (rightClickPressed == true) {
		rightClickPressed = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}