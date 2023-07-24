#include "Engine/Core/PreCompiledHeaders.h"

#include "guiMain.h"


#include "Engine/Components/Core/GameObject.h"
#include "Editor/GUI/Hierarchy/Hierarchy.h"
#include "Editor/GUI/Inspector.h"
#include "Editor/GUI/gizmo.h"
#include "Editor/GUI/TransformOverlay.h"
#include "Engine/Components/Core/GameObject.h"
#include "Engine/Application/ApplicationSizes.h"
#include "Engine/Editor/Editor.h"
#include "Engine/Application/EntryPoint.h"
#include "Editor/GUI/Inspector.h"
#include "Editor/GUI/Inspector/TransformInspector.h"
#include "Editor/GUI/FpsCounter.h"
#include "Editor/GUI/MenuBar.h"
//#include "Engine/Application/Application.h" //

//
//bool ImGuizmo::IsDrawing = false;

using namespace Engine;
using namespace Engine::Editor;
//using namespace Editor;

glm::vec2 curHierarchySize;
glm::vec2 curSceneSize;
glm::vec2 curInspectorSize;

bool Engine::Editor::Gui::isHierarchyOpen = true;
bool Engine::Editor::Gui::isSceneOpen = true;
bool Engine::Editor::Gui::isInspectorOpen = true;
bool windowVisible = true;
// Update ImGui Windows
FpsCounter* fpsCounter;
namespace Engine {
	namespace Editor {
		class Hierarchy;

		void Gui::Update() {
			Gui::setupDockspace(Application->Window->glfwWindow, Application->textureColorbuffer, Application->activeCamera);
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
			io.IniFilename = "Engine/imgui.ini";
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			ImGui_ImplOpenGL3_Init("#version 330");
			io.Fonts->AddFontFromFileTTF("C:/Users/Giovane/Desktop/Workspace 2023/OpenGL/OpenGLEngine/Engine/Font/Poppins-Regular.ttf", 18);
			io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts | ImGuiConfigFlags_DpiEnableScaleViewports;

			FpsCounter* fpsCounter = new FpsCounter();
		}

		void Gui::Delete() {
			// Clean up ImGui
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}

		void Gui::changeSelectedGameObject(GameObject* newSelectedGameObject) {
			Editor::selectedGameObject = newSelectedGameObject;
		}


		void Gui::setupDockspace(GLFWwindow* window, int gameFrameBuffer, Camera* camera) {

			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;

			ImGui::PushStyleColor(ImGuiCol_WindowBg, editorStyle.hierarchyBackgroundColor);

			//	ImGuiWindowFlags  windowFlags = ImGuiWindowFlags_MenuBar;

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.appSize));
			//			windowFlags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
				//		windowFlags |= ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiConfigFlags_DockingEnable | ImGuiWindowFlags_NoScrollbar;

			if (ImGui::Begin("Main DockSpace", new bool(true), windowFlags)) {
				ImGui::SetWindowPos(ImVec2(0, 0));


				// Submit the DockSpace
				ImGuiIO& io = ImGui::GetIO();
				ImGuiID dockspace_id = ImGui::GetID("Main DockSpace");
				ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
				dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
				dockspace_flags |= ImGuiDockNodeFlags_NoDockingInCentralNode;

				// Submit the DockSpace
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

				Gui::MainMenuBar::Begin();

				//ImGui::SetWindowPos(ImVec2(0, 0));

				//ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_Once);
				Gui::beginHierarchyView(gameFrameBuffer);

				//ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_Once);
				Gui::beginScene(gameFrameBuffer, *camera);
				//ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_Once);
				Gui::beginInspector(gameFrameBuffer, *camera);

				ImGui::End();
			}

			ImGui::PopStyleColor();


			Overlay::beginTransformOverlay(*Application->activeCamera);
			fpsCounter->Update();
			// Update the sizes after resizing
			Gui::UpdateSizes();
		}


		// Create the scene view
		inline void Gui::beginScene(int gameFrameBuffer, Camera& camera) {
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			GameObject* selectedGameObject = Editor::selectedGameObject;

			// Set the window to be the content size + header size
			ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove;
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus;
			windowFlags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;

			/*			windowFlags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
			windowFlags |= ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiConfigFlags_DockingEnable | ImGuiWindowFlags_NoScrollbar;*/

			ImGui::SetNextWindowSize(ImVec2(appSizes.sceneSize.x, appSizes.sceneSize.y));
			if (ImGui::Begin("Scene", &Gui::isSceneOpen, windowFlags)) {

				ImVec2 uv0(0, 1); // bottom-left corner
				ImVec2 uv1(1, 0); // top-right corner
				appSizes.sceneImageStart = ImGui::glmVec2(ImGui::GetCursorScreenPos());
				ImGui::Image(ImTextureID(Application->textureColorbuffer), ImGui::imVec2(appSizes.sceneSize), uv0, uv1);








				ImVec2 imageDisplayedSize;
				// Calculate the size of the image
				imageDisplayedSize = ImGui::GetItemRectSize();

				// Get the available width and height in the current window/viewport
				ImVec2 availableSpace = ImGui::GetContentRegionAvail();

				// Calculate the displayed size considering the available space
				imageDisplayedSize.x = std::min(imageDisplayedSize.x, availableSpace.x);
				imageDisplayedSize.y = std::min(imageDisplayedSize.y, availableSpace.y);

				//appSizes.sceneSize = ImGui::glmVec2(imageDisplayedSize);
















				curSceneSize = glm::abs(ImGui::glmVec2(ImGui::GetWindowSize()));
				// Show the gizmo if there's a selected gameObject
				if (selectedGameObject && selectedGameObject->GetComponent<Transform>() != nullptr && selectedGameObject->parent != nullptr) {
					ImGuizmoHelper::IsDrawing = true;
					Editor::Gizmo::Draw(selectedGameObject, camera);
				}
				else {
					ImGuizmoHelper::IsDrawing = false;
				}

				//	appSizes.sceneStart = ImGui::glmVec2(ImGui::GetWindowPos());

			}




			ImGui::End();
			ImGui::PopStyleColor();
		}

		void Gui::beginHierarchyView(int gameFrameBuffer) {
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			GameObject* selectedGameObject = Editor::selectedGameObject;
			//ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiConfigFlags_DockingEnable;
			ImGuiID dockspace_id = ImGui::GetID("Main DockSpace");

			//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Set window background to red//
			ImGui::PushStyleColor(ImGuiCol_WindowBg, editorStyle.treeNodeBackgroundColor);
			ImGui::SetNextWindowSize(ImVec2(appSizes.hierarchySize.x, appSizes.hierarchySize.y));
			if (ImGui::Begin("Hierarchy", &Gui::isHierarchyOpen, sceneWindowFlags)) {
				ImGui::PushStyleColor(ImGuiCol_WindowBg, editorStyle.treeNodeBackgroundColor);


				// Create the main collapser
				Editor::Gui::Hierarchy::Item(gameObjects.front(), selectedGameObject);
				ImGui::PopStyleColor(); // Background Color
			}


			curHierarchySize = ImGui::glmVec2(ImGui::GetWindowSize());
			ImGui::End();
			ImGui::PopStyleColor(); // Background Color
		}

		void Gui::beginInspector(int gameFrameBuffer, Camera camera) {
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			GameObject* selectedGameObject = Editor::selectedGameObject;

			//ImGui::SetCursorPosY(appSizes.appHeaderSize);
			ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.inspectorSize), ImGuiCond_Always);
			//ImGui::SetNextWindowPos(ImVec2(appSizes.appSize.x - appSizes.inspectorSize.x, 0));

			ImGuiWindowFlags sceneWindowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiConfigFlags_DockingEnable;
			ImGuiWindowFlags  inspectorWindowFlags = ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f)); // Remove the padding of the window
			//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Set window background to red//
			ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.inspectorSize));
			if (ImGui::Begin("Inspector", &Gui::isInspectorOpen, sceneWindowFlags)) {

				// Handle size changes
				const ImVec2& CurSize = ImGui::GetWindowSize();
				if (!ImGui::Compare(CurSize, ImGui::imVec2(lastAppSizes.inspectorSize))) {
					//appSizes.inspectorSize = ImGui::glmVec2(CurSize);
					lastAppSizes.inspectorSize = appSizes.inspectorSize;
					ImGui::SetWindowSize(ImGui::imVec2(appSizes.inspectorSize), ImGuiCond_Always);
					//ImGui::SetWindowPos(ImVec2(appSizes.appSize.x - appSizes.inspectorSize.x, 0));
				}

				if (selectedGameObject && selectedGameObject->parent) {
					Editor::Inspector::ComponentInspector inspector(selectedGameObject);
					Editor::Inspector* asd = new Inspector();
					asd->addComponentButton();
					//Gui::Inspector::addComponentButton(appSizes);
				}


			}

			curInspectorSize = ImGui::glmVec2(ImGui::GetWindowSize());
			ImGui::End();
			ImGui::PopStyleVar();
		}



		// Update appSizes
		void Gui::UpdateSizes() {
			ApplicationSizes& appSizes = *Application->appSizes;
			appSizes.hierarchySize = glm::abs(curHierarchySize);
			appSizes.inspectorSize = glm::abs(curInspectorSize);
			//appSizes.sceneSize = curSceneSize;
		}
	};
}

//glm::vec2 ImGui::vec2() {
//	return glm::vec2;
//}