#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <imgui/imgui.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <stb/stb_image.h>

#include <iostream>
#include <random>
#include <filesystem>
#include <fileSystem/fileSystem.h>

#include "guiMain.h"


#include "Engine/Components/Core/GameObject.h"
#include "Engine/GUI/Hierarchy/Hierarchy.h"
#include "Engine/GUI/Inspector.h"
#include "Engine/GUI/gizmo.h"
#include "Engine/GUI/TransformOverlay.h"
#include "Engine/Components/Core/GameObject.h"
#include "Engine/Application/ApplicationSizes.h"
#include "Engine/Editor/Editor.h"
//#include "Engine/Application/Application.h" //

//



//bool ImGuizmo::IsDrawing = false;

using namespace Engine;
//using namespace Editor;

glm::vec2 curHierarchySize;
glm::vec2 curSceneSize;
glm::vec2 curInspectorSize;

// Update ImGui Windows
void Gui::changeSelectedGameObject(GameObject* newSelectedGameObject) {
	Engine::Editor::selectedGameObject = newSelectedGameObject;
	
}


namespace Editor {
	void Gui::Update() {
		Gui::setupDockspace(Engine::Application::window, Engine::Application::textureColorbuffer, Engine::Application::activeCamera);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void Gui::NewFrame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Gui::Init(GLFWwindow* window) {
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		io.IniFilename = NULL;
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void Gui::Delete() {
		// Clean up ImGui
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}




	void Gui::setupDockspace(GLFWwindow* window, int gameFrameBuffer, Camera& camera) {
		ApplicationSizes& appSizes = Engine::Application::appSizes;
		ApplicationSizes& lastAppSizes = Engine::Application::lastAppSizes;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, editorStyle.hierarchyBackgroundColor);

		ImGuiWindowFlags  windowFlags = ImGuiWindowFlags_MenuBar;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.appSize));
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));
		if (ImGui::Begin("mainRect", new bool(false), windowFlags)) {
			//ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			// Create UI elements

			ImGui::Columns(3, "MainColumns", true);
			//float columnWidth1 = 320.0f;
			float columnWidth1 = appSizes.hierarchySize.x;
			float columnWidth2 = appSizes.sceneSize.x;
			float columnWidth3 = appSizes.inspectorSize.x;


			ImGui::SetColumnWidth(0, columnWidth1);
			ImGui::SetColumnWidth(1, columnWidth2);
			ImGui::SetColumnWidth(2, columnWidth3);


			// Set the background color for the first column
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGui::Separator();
			Gui::beginHierarchyView(gameFrameBuffer);
			ImGui::Separator();
			ImGui::PopStyleColor();

			ImGui::NextColumn();

			// Set the background color for the second column
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
			ImGui::Separator();
			Gui::beginScene(gameFrameBuffer, camera);
			//Gui::Overlay().beginTransformOverlay(appSizes, lastAppSizes, camera);
			ImGui::Separator();
			ImGui::PopStyleColor();

			ImGui::NextColumn();

			// Set the background color for the third column
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
			ImGui::Separator();
			Gui::beginInspector(gameFrameBuffer, camera);
			ImGui::Separator();
			ImGui::PopStyleColor();

			ImGui::Columns();
		}
		//appSizes.sceneSize = glm::abs(glm::vec2(appSizes.appSize.x - appSizes.hierarchySize.x - appSizes.inspectorSize.x, appSizes.sceneSize.y));
		curSceneSize = glm::abs(ImGui::glmVec2(ImGui::GetWindowSize()));


		ImGui::End();

		// Update the sizes after resizing
		Gui::UpdateSizes();
	}


	//using namespace Editor::Gui;
	// Create the scene view
	inline void Gui::beginScene(int gameFrameBuffer, Camera& camera) {
		ApplicationSizes& appSizes = Engine::Application::appSizes;
		ApplicationSizes& lastAppSizes = Engine::Application::lastAppSizes;
		GameObject* selectedGameObject = Engine::Editor::selectedGameObject;
		ImGui::SetCursorPosY(appSizes.appHeaderSize);
		ImGui::Text("Scene");
		//ImGui::NextColumn();
		// Get the header size
		ImGuiStyle style = ImGui::GetStyle();
		ImVec2 headerSize = ImVec2(0, ImGui::GetFontSize() + style.FramePadding.y * 2);
		appSizes.appHeaderSize = headerSize.y;
		// Set the window to be the content size + header size
		ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.sceneSize - glm::vec2(0, appSizes.sceneSize.y + headerSize.y + 100))); // REMOVE THE + glm::vec2(0.0f,50.0f) -----------------------------------
		//ImGui::SetNextWindowPos(ImVec2(appSizes.hierarchySize.x, 0));
		ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize | ImGuiViewportFlags_NoFocusOnClick;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f)); // Remove the padding of the window
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		ImGui::SetCursorPosY(appSizes.appHeaderSize + 20);
		if (ImGui::BeginChild("Scene", ImVec2(appSizes.sceneSize.x, appSizes.sceneSize.y + appSizes.appHeaderSize), new bool(true), sceneWindowFlags)) {
			ImGui::SetCursorPosY(appSizes.appHeaderSize);
			if (ImGui::BeginChild("Image Container", ImVec2(appSizes.sceneSize.x, appSizes.sceneSize.y), false, sceneWindowFlags | ImGuiWindowFlags_NoTitleBar)) {
				ImVec2 uv0(0, 1); // bottom-left corner
				ImVec2 uv1(1, 0); // top-right corner

				ImGui::SetWindowSize(ImGui::imVec2(appSizes.sceneSize));
				ImGui::SetWindowPos(ImVec2(appSizes.hierarchySize.x, headerSize.y)); // Position it to on the center and below the header
				//ImGui::Image(ImTextureID(Application::selectedColorBuffer), ImGui::imVec2(appSizes.sceneSize), uv0, uv1);
				//ImGui::Image(ImTextureID(Application::textureColorbuffer), ImGui::imVec2(appSizes.sceneSize), uv0, uv1);
				ImGui::Image(ImTextureID(Application::edgeDetectionColorBuffer), ImGui::imVec2(appSizes.sceneSize), uv0, uv1);
				//ImGui::Image(ImTextureID(Application::blurColorBuffer), ImGui::imVec2(appSizes.sceneSize), uv0, uv1);
				
				// Show the gizmo if there's a selected gameObject
				if (selectedGameObject && selectedGameObject->parent) {
					//std::cout << selectedGameObject->GetComponent<Transform>(). << std::endl;
				}

				if (selectedGameObject && selectedGameObject->GetComponent<Transform>() != nullptr && selectedGameObject->parent != nullptr) {
					ImGuizmo::IsDrawing = true;
					Gui::Gizmo::Draw(selectedGameObject, camera);
					//camera.Position = selectedGameObject->transform->worldPosition;
				}
				else {
					ImGuizmo::IsDrawing = false;
				}

				ImGui::EndChild();

			}
		}


		curSceneSize = glm::abs(ImGui::glmVec2(ImGui::GetWindowSize()));
		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}

	void Gui::beginHierarchyView(int gameFrameBuffer) {
		ApplicationSizes& appSizes = Engine::Application::appSizes;
		ApplicationSizes& lastAppSizes = Engine::Application::lastAppSizes;
		GameObject* selectedGameObject = Engine::Editor::selectedGameObject;

		ImGui::SetCursorPosY(appSizes.appHeaderSize);
		ImGui::Text("Hierarchy");
		ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.hierarchySize + glm::vec2(0.0f, appSizes.appHeaderSize)), ImGuiCond_Always);
		//ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f)); // Remove the padding of the window
		//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Set window background to red//
		ImGui::PushStyleColor(ImGuiCol_WindowBg, editorStyle.treeNodeBackgroundColor);

		if (ImGui::BeginChild("Hierarchy", ImVec2(appSizes.hierarchySize.x, appSizes.hierarchySize.y), new bool(false), sceneWindowFlags)) {
			ImGui::PushStyleColor(ImGuiCol_WindowBg, editorStyle.treeNodeBackgroundColor);

			ImGui::SetWindowSize(ImVec2(appSizes.hierarchySize.x, appSizes.hierarchySize.y), ImGuiCond_Always);
			// Handle size changes
			const ImVec2& CurSize = ImGui::GetWindowSize();
			if (CurSize.x != lastAppSizes.hierarchySize.x || CurSize.y != lastAppSizes.hierarchySize.y) {
				//appSizes.hierarchySize = ImGui::glmVec2(CurSize);
				lastAppSizes.hierarchySize = appSizes.hierarchySize;
				ImGui::SetWindowSize(ImVec2(appSizes.hierarchySize.x, appSizes.hierarchySize.y), ImGuiCond_Always);
				//ImGui::SetWindowPos(ImVec2(0, 0));


			}
			// Create the main collapser
			Gui::Hierarchy::Item(gameObjects.front(), selectedGameObject);
			ImGui::PopStyleColor(); // Background Color
		}


		curHierarchySize = ImGui::glmVec2(ImGui::GetWindowSize());
		ImGui::EndChild();
		ImGui::PopStyleColor(); // Background Color
		ImGui::PopStyleVar();// End hierarchy style
	}

	void Gui::beginInspector(int gameFrameBuffer, Camera camera) {
		ApplicationSizes& appSizes = Engine::Application::appSizes;
		ApplicationSizes& lastAppSizes = Engine::Application::lastAppSizes;
		GameObject* selectedGameObject = Engine::Editor::selectedGameObject;

		ImGui::SetCursorPosY(appSizes.appHeaderSize);
		ImGui::Text("Inspector");
		ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.inspectorSize), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(appSizes.appSize.x - appSizes.inspectorSize.x, 0));

		ImGuiWindowFlags  inspectorWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f)); // Remove the padding of the window
		//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Set window background to red//
		if (ImGui::BeginChild("Inspector", ImGui::imVec2(appSizes.inspectorSize), new bool(true), inspectorWindowFlags)) {

			// Handle size changes
			const ImVec2& CurSize = ImGui::GetWindowSize();
			if (!ImGui::Compare(CurSize, ImGui::imVec2(lastAppSizes.inspectorSize))) {
				//appSizes.inspectorSize = ImGui::glmVec2(CurSize);
				lastAppSizes.inspectorSize = appSizes.inspectorSize;
				ImGui::SetWindowSize(ImGui::imVec2(appSizes.inspectorSize), ImGuiCond_Always);
				//ImGui::SetWindowPos(ImVec2(appSizes.appSize.x - appSizes.inspectorSize.x, 0));
			}

			if (selectedGameObject && selectedGameObject->parent) {
				Gui::Inspector::ComponentInspector inspector(selectedGameObject);

				Gui::Inspector* asd = new Gui::Inspector();
				asd->addComponentButton();
				//Gui::Inspector::addComponentButton(appSizes);
			}
		}

		curInspectorSize = ImGui::glmVec2(ImGui::GetWindowSize());
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}



	// Update appSizes
	void Gui::UpdateSizes() {
		ApplicationSizes& appSizes = Engine::Application::appSizes;
		//appSizes.hierarchySize = glm::abs(curHierarchySize);
		//appSizes.inspectorSize = glm::abs(curInspectorSize);
		//appSizes.sceneSize.x = glm::abs(appSizes.appSize.x - appSizes.hierarchySize.x - appSizes.inspectorSize.x);
	}
}


//glm::vec2 ImGui::vec2() {
//	return glm::vec2;
//}