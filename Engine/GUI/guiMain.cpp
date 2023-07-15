#include "Engine/Core/PreCompiledHeaders.h"

#include "guiMain.h"


#include "Engine/Components/Core/GameObject.h"
#include "Engine/GUI/Hierarchy/Hierarchy.h"
#include "Engine/GUI/Inspector.h"
#include "Engine/GUI/gizmo.h"
#include "Engine/GUI/TransformOverlay.h"
#include "Engine/Components/Core/GameObject.h"
#include "Engine/Application/ApplicationSizes.h"
#include "Engine/Editor/Editor.h"
#include "Engine/Application/EntryPoint.h"
#include "Engine/GUI/Inspector.h"
#include "Engine/GUI/Inspector/TransformInspector.h"
#include "Engine/GUI/FpsCounter.h"
//#include "Engine/Application/Application.h" //

//
//bool ImGuizmo::IsDrawing = false;

using namespace Engine;
using namespace Engine::Editor;
//using namespace Editor;

glm::vec2 curHierarchySize;
glm::vec2 curSceneSize;
glm::vec2 curInspectorSize;

void ShowExampleAppDockSpace(bool* p_open)
{
	// READ THIS !!!
	// TL;DR; this demo is more complicated than what most users you would normally use.
	// If we remove all options we are showcasing, this demo would become:
	//     void ShowExampleAppDockSpace()
	//     {
	//         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	//     }
	// In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
	// In this specific demo, we are not using DockSpaceOverViewport() because:
	// - (1) we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
	// - (2) we allow the host window to have padding (when opt_padding == true)
	// - (3) we expose many flags and need a way to have them visible.
	// - (4) we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport()
	//      in your code, but we don't here because we allow the window to be floating)

	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(ImVec2(2000, 2000));
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", p_open, window_flags);
	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
			ImGui::MenuItem("Padding", NULL, &opt_padding);
			ImGui::Separator();

			if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
			if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
			if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
			if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
			if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
			ImGui::Separator();

			if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
				*p_open = false;
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
}

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
			io.IniFilename = "Engine/imgui.ini"; // Set the path to your desired ini file
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			ImGui_ImplOpenGL3_Init("#version 330");

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
			/*
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;

			ImGui::PushStyleColor(ImGuiCol_WindowBg, editorStyle.hierarchyBackgroundColor);

			ImGuiWindowFlags  windowFlags = ImGuiWindowFlags_MenuBar;
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.appSize));
			windowFlags |=  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiConfigFlags_DockingEnable;
			//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));
			if (ImGui::Begin("mainRect", new bool(false), windowFlags)) {
				ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
				ImGui::PopStyleColor();
				/*
				//ImGui::DockSpace(ImGui::GetID("mainRect2"), ImVec2(100, 100), 0);
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
				Gui::beginScene(gameFrameBuffer, *camera);
				ImGui::Separator();
				ImGui::PopStyleColor();

				ImGui::NextColumn();

				// Set the background color for the third column
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				ImGui::Separator();
				Gui::beginInspector(gameFrameBuffer, *camera);
				ImGui::Separator();
				ImGui::PopStyleColor();

				ImGui::Columns();

			}



			if (ImGui::Begin("Viewp3")) {
				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGuiIO& io = ImGui::GetIO();
				ImGui::DockSpace(ImGui::GetID("Viewp3"), ImVec2(100, 100), ImGuiConfigFlags_DockingEnable);
				ImGui::End();
			}

			Overlay::beginTransformOverlay(*Application->activeCamera);
			//appSizes.sceneSize = glm::abs(glm::vec2(appSizes.appSize.x - appSizes.hierarchySize.x - appSizes.inspectorSize.x, appSizes.sceneSize.y));
			//curSceneSize = glm::abs(ImGui::glmVec2(ImGui::GetWindowSize()));

			fpsCounter->Update();

			ImGui::End();

			if (ImGui::Begin("Viewp2")) {
				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGuiIO& io = ImGui::GetIO();
				ImGui::DockSpace(ImGui::GetID("Viewp2"), ImVec2(100, 100), ImGuiConfigFlags_DockingEnable);
				ImGui::End();
			}
			*/
			//ShowExampleAppDockSpace(new bool(true));
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;

			ImGui::PushStyleColor(ImGuiCol_WindowBg, editorStyle.hierarchyBackgroundColor);

			ImGuiWindowFlags  windowFlags = ImGuiWindowFlags_MenuBar;
			ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.appSize));
			windowFlags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
			windowFlags |= ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiConfigFlags_DockingEnable | ImGuiWindowFlags_NoScrollbar;
			if (ImGui::Begin("DockSpace Demo", new bool(true), windowFlags)) {
				ImGui::SetWindowPos(ImVec2(0,0));
				ImGui::End();
			}
			Gui::beginHierarchyView(gameFrameBuffer);
			Gui::beginScene(gameFrameBuffer, *camera);
			Gui::beginInspector(gameFrameBuffer, *camera);
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
			ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_MenuBar;
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
			windowFlags |= ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiConfigFlags_DockingEnable | ImGuiWindowFlags_NoScrollbar;

			/*			windowFlags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
			windowFlags |= ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiConfigFlags_DockingEnable | ImGuiWindowFlags_NoScrollbar;*/

			ImGui::SetNextWindowSize(ImVec2(appSizes.sceneSize.x, appSizes.sceneSize.y));
			if (ImGui::Begin("Scene", new bool(true), sceneWindowFlags)) {
				
				ImVec2 uv0(0, 1); // bottom-left corner
				ImVec2 uv1(1, 0); // top-right corner
				ImGui::Image(ImTextureID(Application->textureColorbuffer), ImGui::imVec2(appSizes.sceneSize), uv0, uv1);

				// Show the gizmo if there's a selected gameObject
				if (selectedGameObject && selectedGameObject->GetComponent<Transform>() != nullptr && selectedGameObject->parent != nullptr) {
					ImGuizmoHelper::IsDrawing = true;
					Editor::Gizmo::Draw(selectedGameObject, camera);
				}
				else {
					ImGuizmoHelper::IsDrawing = false;
				}
				


				ImGui::End();
			}


			curSceneSize = glm::abs(ImGui::glmVec2(ImGui::GetWindowSize()));

			ImGui::PopStyleColor();
		}

		void Gui::beginHierarchyView(int gameFrameBuffer) {
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			GameObject* selectedGameObject = Editor::selectedGameObject;
			//ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiConfigFlags_DockingEnable;

			//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Set window background to red//
			ImGui::PushStyleColor(ImGuiCol_WindowBg, editorStyle.treeNodeBackgroundColor);
			ImGui::SetNextWindowSize(ImVec2(appSizes.hierarchySize.x, appSizes.hierarchySize.y));
			if (ImGui::Begin("Hierarchy", new bool(true), sceneWindowFlags)) {
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
			ImGuiWindowFlags  inspectorWindowFlags =  ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f)); // Remove the padding of the window
			//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Set window background to red//
			ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.inspectorSize));
			if (ImGui::Begin("Inspector", new bool(true), sceneWindowFlags)) {

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

				ImGui::End();
			}

			curInspectorSize = ImGui::glmVec2(ImGui::GetWindowSize());

			ImGui::PopStyleVar();
		}



		// Update appSizes
		void Gui::UpdateSizes() {
			ApplicationSizes& appSizes = *Application->appSizes;
			//appSizes.hierarchySize = glm::abs(curHierarchySize);
			//appSizes.inspectorSize = glm::abs(curInspectorSize);
			//appSizes.sceneSize.x = glm::abs(appSizes.appSize.x - appSizes.hierarchySize.x - appSizes.inspectorSize.x);
		}
	};
}

//glm::vec2 ImGui::vec2() {
//	return glm::vec2;
//}