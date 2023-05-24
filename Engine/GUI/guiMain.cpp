#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include <stb/stb_image.h>

#include <iostream>
#include <random>
#include <filesystem>
#include <fileSystem/fileSystem.h>
#include "guiMain.h"
#include "Engine/Application/Application.h"
#include "Engine/Components/GameObject.h"
#include "Engine/GUI/Hierarchy.h"
#include "Engine/GUI/Inspector.h"
#include "Engine/GUI/gizmo.h"
//

GameObject* selectedGameObject;

void beginScene(int gameFrameBuffer, AppSizes& appSizes, AppSizes& lastAppSizes, Camera camera);
void beginHierarchyView(int gameFrameBuffer, AppSizes& appSizes, AppSizes& lastAppSizes);
void beginInspector(int gameFrameBuffer, AppSizes& appSizes, AppSizes& lastAppSizes, Camera camera);

void updateSizes(AppSizes appSizes);

glm::vec2 curHierarchySize;
glm::vec2 curSceneSize;
glm::vec2 curInspectorSize;



void Gui::setupDockspace(GLFWwindow* window, int gameFrameBuffer, AppSizes& appSizes, AppSizes& lastAppSizes, Camera camera) {
	ImGuiWindowFlags  windowFlags = ImGuiWindowFlags_MenuBar;
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.appSize));
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));
	if (ImGui::Begin("mainRect", new bool(true), windowFlags)) {
		ImGui::PopStyleVar();
		// Create UI elements
		beginScene(gameFrameBuffer, appSizes, lastAppSizes, camera);
		beginHierarchyView(gameFrameBuffer, appSizes, lastAppSizes);
		beginInspector(gameFrameBuffer, appSizes, lastAppSizes, camera);
	}
	appSizes.sceneSize = glm::vec2(appSizes.appSize.x - appSizes.hierarchySize.x - appSizes.inspectorSize.x, appSizes.sceneSize.y);
	curSceneSize = ImGui::glmVec2(ImGui::GetWindowSize());
	ImGui::End();

	// Update the sizes after resizing
	updateSizes(appSizes);
}

// Create the scene view
inline void beginScene(int gameFrameBuffer, AppSizes& appSizes, AppSizes& lastAppSizes, Camera camera) {
	// Get the header size
	ImGuiStyle style = ImGui::GetStyle();
	ImVec2 headerSize = ImVec2(0, ImGui::GetFontSize() + style.FramePadding.y * 2);
	// Set the window to be the content size + header size
	ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.sceneSize - glm::vec2(0, appSizes.sceneSize.y + headerSize.y + 100))); // REMOVE THE + glm::vec2(0.0f,50.0f) -----------------------------------
	ImGui::SetNextWindowPos(ImVec2(appSizes.hierarchySize.x, 0));
	ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize | ImGuiViewportFlags_NoFocusOnClick;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f)); // Remove the padding of the window
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); 
	if (ImGui::Begin("Scene", new bool(true), sceneWindowFlags)) {
		if (ImGui::BeginChild("Image Container", ImVec2(appSizes.sceneSize.x , appSizes.sceneSize.y), false, sceneWindowFlags | ImGuiWindowFlags_NoTitleBar)) {
			ImVec2 uv0(0, 1); // bottom-left corner
			ImVec2 uv1(1, 0); // top-right corner

			ImGui::SetWindowSize(ImGui::imVec2(appSizes.sceneSize));
			ImGui::SetWindowPos(ImVec2(appSizes.hierarchySize.x, headerSize.y)); // Position it to on the center and below the header
			ImGui::Image(ImTextureID(gameFrameBuffer), ImGui::imVec2(appSizes.sceneSize), uv0, uv1);
			// Show the gizmo if there's a selected gameObject
			if (selectedGameObject) {
				Gui::Gizmo gizmo(selectedGameObject, camera, appSizes);
			}
		}
		ImGui::EndChild();

	}
	curSceneSize = ImGui::glmVec2(ImGui::GetWindowSize());
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

void beginHierarchyView(int gameFrameBuffer, AppSizes& appSizes, AppSizes& lastAppSizes) {
	ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.hierarchySize), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f)); // Remove the padding of the window
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Set window background to red//

	if (ImGui::Begin("Hierarchy", new bool(true), sceneWindowFlags)) {
		ImGui::SetWindowSize(ImVec2(appSizes.hierarchySize.x, appSizes.hierarchySize.y), ImGuiCond_Always);
		// Handle size changes
		const ImVec2& CurSize = ImGui::GetWindowSize();
		if (CurSize.x != lastAppSizes.hierarchySize.x || CurSize.y != lastAppSizes.hierarchySize.y) {
			appSizes.hierarchySize = ImGui::glmVec2(CurSize);
			lastAppSizes.hierarchySize = appSizes.hierarchySize;
			ImGui::SetWindowSize(ImVec2(appSizes.hierarchySize.x, appSizes.hierarchySize.y), ImGuiCond_Always);
			ImGui::SetWindowPos(ImVec2(0, 0));
		}

		// Create the main collapser
		if (ImGui::TreeNode("Scene Objects")) {

			// For each GameObject, create a new collapser
			for (GameObject* gameObject : gameObjects)
			{
				Gui::Hierarchy::Item(gameObject, selectedGameObject);
			}

			ImGui::TreePop(); // End TreeNode Scene Objects
		}
	}
	curHierarchySize = ImGui::glmVec2(ImGui::GetWindowSize());
	ImGui::End(); // End hierarchy
	ImGui::PopStyleVar();// End hierarchy style
}

void beginInspector(int gameFrameBuffer, AppSizes& appSizes, AppSizes& lastAppSizes, Camera camera) {
	ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.inspectorSize), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(appSizes.appSize.x - appSizes.inspectorSize.x, 0));

	ImGuiWindowFlags  inspectorWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollbar;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f)); // Remove the padding of the window
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Set window background to red//
	if (ImGui::Begin("Inspector", new bool(true), inspectorWindowFlags)) {

		// Handle size changes
		const ImVec2& CurSize = ImGui::GetWindowSize();
		if (!ImGui::Compare(CurSize, ImGui::imVec2(lastAppSizes.inspectorSize))) {
			appSizes.inspectorSize = ImGui::glmVec2(CurSize);
			lastAppSizes.inspectorSize = appSizes.inspectorSize;
			ImGui::SetWindowSize(ImGui::imVec2(appSizes.inspectorSize), ImGuiCond_Always);
			ImGui::SetWindowPos(ImVec2(appSizes.appSize.x - appSizes.inspectorSize.x, 0));
		}

		if (selectedGameObject) {
			Gui::Inspector::ComponentInspector inspector(selectedGameObject);
		}
	}
	curInspectorSize = ImGui::glmVec2(ImGui::GetWindowSize());
	ImGui::End();
	ImGui::PopStyleVar();
}

// Update appSizes
void updateSizes(AppSizes appSizes) {
	appSizes.sceneSize = curSceneSize;
	appSizes.hierarchySize = curHierarchySize;
	appSizes.inspectorSize = curInspectorSize;
}



//glm::vec2 ImGui::vec2() {
//	return glm::vec2;
//}