#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Application/ApplicationSizes.h"
#include "Editor/GUI/guiMain.h"


using namespace Plaza::Editor;
using namespace Plaza;

void ApplicationClass::Callbacks::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	//if (Application->hoveredMenu != "File Explorer" && Application->hoveredMenu != "Inspector")
	//	Editor::selectedFiles.clear();
	if (Application->hoveredMenu == "Editor" && Application->focusedMenu != "Scene") {
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
		if (Application->focusedMenu == "Editor") {
			float xposGame = lastX - appSizes.hierarchySize.x;
			float yposGame = lastY - appSizes.sceneImageStart.y;
			//yposGame = appSizes.sceneSize.y - yposGame;
			yposGame = appSizes.sceneSize.y - (lastY - appSizes.sceneImageStart.y - 35);
			uint64_t clickUuid = 0;

			if (Editor::selectedGameObject && Editor::selectedGameObject->GetComponent<Transform>() != nullptr && Editor::selectedGameObject->parentUuid != 0)
				ImGuizmoHelper::IsDrawing = true;
			else
				ImGuizmoHelper::IsDrawing = false;

			bool pressingLeftClick = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
			bool drawingButMouseNotOverGizmo = !ImGuizmo::IsOver() && ImGuizmoHelper::IsDrawing;
			if (pressingLeftClick && (!ImGuizmoHelper::IsDrawing || drawingButMouseNotOverGizmo)) {
				//	Application->pickingTexture->GenerateTexture();
				//    	clickUuid = Application->pickingTexture->readPixel(xposGame, yposGame);
				clickUuid = Application->mRenderer->mPicking->DrawAndRead(glm::vec2(xposGame, yposGame));

				auto it = Application->activeScene->entities.find(clickUuid);
				if (it != Application->activeScene->entities.end()) {
					// Object with the specified name found
					Plaza::Editor::Gui::changeSelectedGameObject(&it->second);
				}
				else
					Plaza::Editor::Gui::changeSelectedGameObject(nullptr);
			}

		}
#pragma endregion Picking
	}
	else if (rightClickPressed == true) {
		rightClickPressed = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}