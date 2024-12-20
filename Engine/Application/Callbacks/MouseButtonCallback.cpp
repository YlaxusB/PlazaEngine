#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Application/ApplicationSizes.h"
#include "Editor/GUI/guiMain.h"
#include "Engine/Components/Drawing/UI/GuiButton.h"

using namespace Plaza::Editor;
using namespace Plaza;

void Callbacks::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	for (CallbackFunction callbackFunction : sOnMouseFunctions) {
		bool isLayerFocused = Editor::Gui::sFocusedLayer == callbackFunction.layerToExecute;
		if (isLayerFocused)
			callbackFunction.function;
	}

	for (auto& tool : Editor::Gui::sEditorTools) {
		tool.second->OnMouseClick(button, action, mods);
	}

	if (Application::Get()->runningScene && Application::Get()->focusedMenu == "Scene") {
		for (auto& [key, value] : Scene::GetActiveScene()->guiComponents) {
			for (auto& [itemKey, itemValue] : value.mGuiItems) {
				if (itemValue->mGuiType == GuiType::PL_GUI_BUTTON) {
					double x;
					double y;
					glfwGetCursorPos(window, &x, &y);
					if (static_cast<GuiButton*>(itemValue.get())->MouseIsInsideButton(glm::vec2(x, y)))
						static_cast<GuiButton*>(itemValue.get())->CallScriptsCallback();
				}
			}
		}
	}

	//if (Application::Get()->hoveredMenu != "File Explorer" && Application::Get()->hoveredMenu != "Inspector")
	//	Editor::selectedFiles.clear();
	if (Application::Get()->hoveredMenu == "Editor" && Application::Get()->focusedMenu != "Scene") {
		ApplicationSizes& appSizes = *Application::Get()->appSizes;
		ApplicationSizes& lastAppSizes = *Application::Get()->lastAppSizes;

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
		if (Application::Get()->focusedMenu == "Editor") {
			float xposGame = lastX - appSizes.hierarchySize.x;
			float yposGame = lastY - appSizes.sceneImageStart.y;
			//yposGame = appSizes.sceneSize.y - yposGame;
			yposGame = appSizes.sceneSize.y - (lastY - appSizes.sceneImageStart.y - 35);
			uint64_t clickUuid = 0;

			if (Editor::selectedGameObject && Editor::selectedGameObject->GetComponent<TransformComponent>() != nullptr && Editor::selectedGameObject->parentUuid != 0)
				ImGuizmoHelper::IsDrawing = true;
			else
				ImGuizmoHelper::IsDrawing = false;

			bool pressingLeftClick = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
			bool drawingButMouseNotOverGizmo = !ImGuizmo::IsOver() && ImGuizmoHelper::IsDrawing;
			if (pressingLeftClick && (!ImGuizmoHelper::IsDrawing || drawingButMouseNotOverGizmo)) {
				//	Application::Get()->pickingTexture->GenerateTexture();
				//    	clickUuid = Application::Get()->pickingTexture->readPixel(xposGame, yposGame);
				clickUuid = Application::Get()->mRenderer->mPicking->DrawAndRead(glm::vec2(xposGame, yposGame));

				auto it = Scene::GetActiveScene()->entities.find(clickUuid);
				if (it != Scene::GetActiveScene()->entities.end()) {
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