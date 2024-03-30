#include "Engine/Core/PreCompiledHeaders.h"

#include "guiMain.h"
#include "Editor/GUI/Utils/DataVisualizer.h"


#include "Engine/Components/Core/Entity.h"
#include "Editor/GUI/Hierarchy/Hierarchy.h"
#include "Editor/GUI/Inspector.h"
#include "Editor/GUI/gizmo.h"
#include "Editor/GUI/TransformOverlay.h"
#include "Engine/Components/Core/Entity.h"
#include "Engine/Application/ApplicationSizes.h"
#include "Engine/Editor/Editor.h"
#include "Engine/Application/EntryPoint.h"
#include "Editor/GUI/Inspector.h"
#include "Editor/GUI/Inspector/TransformInspector.h"
#include "Editor/GUI/FpsCounter.h"
#include "Editor/GUI/MenuBar.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
#include "Engine/Core/ModelLoader/ModelLoader.h"
#include "Engine/Application/Serializer/ModelSerializer.h"

#include "Editor/GUI/Hierarchy/HierarchyPopup.h"
#include "Editor/GUI/Utils/ImageLoader.h"
#include "Editor/ScriptManager/ScriptManager.h"
#include "Engine/Core/Input/Input.h"
#include "Engine/Core/Input/Cursor.h"

#include <ThirdParty/imgui/imgui_impl_opengl3.h>
#include "Engine/Core/AssetsManager/AssetsManager.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"

////   #include "ThirdParty/imgui/imgui_impl_vulkan.h"
//#include "Engine/Application/Application.h" //

//
//bool ImGuizmo::IsDrawing = false;


//using namespace Editor;

glm::vec2 curHierarchySize;
glm::vec2 curSceneSize;
glm::vec2 curInspectorSize;

bool Plaza::Editor::Gui::sceneViewUsingEditorCamera = false;
bool Plaza::Editor::Gui::isHierarchyOpen = true;
bool Plaza::Editor::Gui::isSceneOpen = true;
bool Plaza::Editor::Gui::isInspectorOpen = true;
bool Plaza::Editor::Gui::isFileExplorerOpen = true;
bool Plaza::Editor::Gui::canUpdateContent = false;

bool windowVisible = true;
// Update ImGui Windows
FpsCounter* fpsCounter;
namespace Plaza {
	namespace Editor {
		class Hierarchy;
		string Gui::scenePayloadName = "scenePayloadName";

		ImTextureID Gui::playPauseButtonImageId;
		void Gui::Update() {
			PLAZA_PROFILE_SECTION("ImGui Update");
			ImGuiIO& io = ImGui::GetIO();
			io.DeltaTime = Time::deltaTime;
			Gui::setupDockspace(Application->Window->glfwWindow, Application->textureColorbuffer, Application->activeCamera);
			if (Application->mRenderer->api == RendererAPI::OpenGL)
			{
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
			//else if (Application->mRenderer->api == RendererAPI::Vulkan)
			//	Application->mRenderer->UpdateGUI();
			//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), (VulkanRenderer)(Application->mRenderer)->);
		}

		void Gui::NewFrame() {
			PLAZA_PROFILE_SECTION("ImGui New Frame");
			if (Application->mRenderer->api == RendererAPI::OpenGL)
			{
				ImGui_ImplOpenGL3_NewFrame();
			}
			else if (Application->mRenderer->api == RendererAPI::Vulkan)
			{
				Application->mRenderer->NewFrameGUI();
			}
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		void Gui::Init(GLFWwindow* window) {
			EditorStyle* editorStyle = new EditorStyle();
			ImGui::CreateContext();
			ImGui::StyleColorsDark();
			ImGuiIO& io = ImGui::GetIO();
			(void)io;

			const char* iniFilePath = new char[(Application->enginePath + "\\imgui.ini").size() + 1];
			strcpy(const_cast<char*>(iniFilePath), (Application->enginePath + "\\imgui.ini").c_str());
			ImGui::GetIO().IniFilename = iniFilePath;
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			if (Application->mRenderer->api == RendererAPI::OpenGL)
			{
				ImGui_ImplGlfw_InitForOpenGL(window, true);
				ImGui_ImplOpenGL3_Init("#version 460");
			}
			else if (Application->mRenderer->api == RendererAPI::Vulkan)
			{
				ImGui_ImplGlfw_InitForVulkan(window, true);
				Application->mRenderer->InitGUI();
			}
			//ImGui_ImplGlfw_InitForVulkan(Application->Window->glfwWindow, true);
		//C:/Users/Giovane/Desktop/Workspace 2023/OpenGL/OpenGLEngine/Engine/Font/Poppins-Regular.ttf
			io.Fonts->AddFontFromFileTTF((Application->enginePath + "/Font/Poppins-Regular.ttf").c_str(), 18);
			io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts | ImGuiConfigFlags_DpiEnableScaleViewports;

#pragma region ImGui Style
			auto& colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_WindowBg] = editorStyle->backgroundColor;
			colors[ImGuiCol_Header] = editorStyle->CollapsingHeaderBackgroundColor;
			colors[ImGuiCol_HeaderHovered] = editorStyle->treeNodeHoverBackgroundColor;
			colors[ImGuiCol_HeaderActive] = editorStyle->treeNodeActiveBackgroundColor;
			colors[ImGuiCol_TabActive] = editorStyle->tabActiveColor;
			colors[ImGuiCol_Tab] = ImVec4(0.295f, 0.295f, 0.295f, 1.0f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.255f, 0.255f, 0.255f, 1.0f);
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
#pragma endregion

			ImGui::GetStyle().ScrollbarSize = 9.0f;
			Icon::Init();

			FpsCounter* fpsCounter = new FpsCounter();

			// Load Icons
			playPauseButtonImageId = Utils::LoadImageToImGuiTexture(std::string(Application->editorPath + "\\Images\\Other\\playPauseButton.png").c_str());
		}

		void Gui::Delete() {
			// Clean up ImGui
			if (Application->mRendererAPI == RendererAPI::OpenGL)
			{
				ImGui_ImplOpenGL3_Shutdown();
				ImGui_ImplGlfw_Shutdown();
				ImGui::DestroyContext();
			}
		}

		void Gui::changeSelectedGameObject(Entity* newSelectedGameObject) {
			selectedGameObject = newSelectedGameObject;
		}
		void Gui::setupDockspace(GLFWwindow* window, int gameFrameBuffer, Camera* camera) {
			PLAZA_PROFILE_SECTION("Setup Dockspace");
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;

			//	ImGuiWindowFlags  windowFlags = ImGuiWindowFlags_MenuBar;

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.appSize));
			//			windowFlags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
				//		windowFlags |= ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiConfigFlags_DockingEnable | ImGuiWindowFlags_NoScrollbar;

			bool showDockspace = true;
			ImGui::Begin("Main DockSpace", &showDockspace, windowFlags);

			//ImGui::SetWindowPos(ImVec2(0, 0));

			// Submit the DockSpace
			ImGuiIO& io = ImGui::GetIO();
			ImGuiID dockspace_id = ImGui::GetID("Main DockSpace");
			ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
			dockspace_flags |= ImGuiDockNodeFlags_NoDockingInCentralNode;

			// Submit the DockSpace
			ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);
			Gui::MainMenuBar::Begin();

			Gui::beginHierarchyView(gameFrameBuffer);


			Gui::beginScene(gameFrameBuffer, *Application->activeCamera);

			Gui::beginEditor(gameFrameBuffer, *Application->activeCamera);

			Gui::beginAssetsViewer(gameFrameBuffer, *Application->activeCamera);

			Gui::beginInspector(gameFrameBuffer, *camera);

			//ImGui::ShowDemoWindow();

			FileExplorer::UpdateGui();
			ImGui::End();


			Overlay::beginTransformOverlay(*Application->activeCamera);
			fpsCounter->Update();
			// Update the sizes after resizing

		   //Gui::UpdateSizes();
			if (canUpdateContent) {
				//    Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
				canUpdateContent = false;
			}
		}


		// Create the scene view
		inline void Gui::beginScene(int gameFrameBuffer, Camera& camera) {
			PLAZA_PROFILE_SECTION("Begin Scene");
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			Entity* selectedGameObject = Editor::selectedGameObject;

			// Set the window to be the content size + header size
			ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove;

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus;
			windowFlags |= ImGuiWindowFlags_NoScrollbar;

			/*			windowFlags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
			windowFlags |= ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiConfigFlags_DockingEnable | ImGuiWindowFlags_NoScrollbar;*/

			ImGui::SetNextWindowSize(ImVec2(appSizes.sceneSize.x, appSizes.sceneSize.y));
			if (ImGui::Begin("Scene", &Gui::isSceneOpen, windowFlags)) {
				if (sceneViewUsingEditorCamera)
					Application->activeCamera = Application->editorCamera;
				else {
					if (Application->activeCamera->isEditorCamera && Application->activeScene->cameraComponents.size() > 0)
						Application->activeCamera = &Application->activeScene->cameraComponents.begin()->second;
				}
			}


			if (ImGui::IsWindowFocused())
			{
				if (Application->focusedMenu != "Scene") {
					if (Input::Cursor::show)
						glfwSetInputMode(Application->Window->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					else
						glfwSetInputMode(Application->Window->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}

				Application->focusedMenu = "Scene";
			}
			if (ImGui::IsWindowHovered())
				Application->hoveredMenu = "Scene";

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2, 25));

			ImGuiStyle& style = ImGui::GetStyle();

			// Change the background color of the button
			if (Application->runningScene) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			}
			if (isSceneOpen) {
				if (ImGui::ImageButton("PlayPauseButton", ImTextureID(playPauseButtonImageId), ImVec2(25, 25))) {
					if (Application->runningScene)
						Scene::Stop();
					else
						Scene::Play();
				}
				ImGui::SameLine();
				if (ImGui::Button("Reload C# Script Assembly")) {
					ScriptManager::ReloadScriptsAssembly();
				}
				ImGui::SameLine();
				if (ImGui::Checkbox("Editor Camera", &sceneViewUsingEditorCamera)) {
					if (sceneViewUsingEditorCamera)
						Application->activeCamera = Application->editorCamera;
					else {
						if (Application->activeCamera->isEditorCamera && Application->activeScene->cameraComponents.size() > 0)
							Application->activeCamera = &Application->activeScene->cameraComponents.begin()->second;
					}
				}
			}
			ImGui::PopStyleColor();
			if (isSceneOpen) {

				ImVec2 uv0(0, 0); // bottom-left corner
				ImVec2 uv1(1, 1); // top-right corner
				appSizes.sceneImageStart = ImGui::glmVec2(ImGui::GetCursorScreenPos());
				ImGui::Image(ImTextureID(Application->mRenderer->GetFrameImage()), ImGui::imVec2(appSizes.sceneSize), uv0, uv1);
				//ImGui::Image(ImTextureID(Application->textureColorbuffer), ImGui::imVec2(appSizes.sceneSize), uv0, uv1);

				//	appSizes.sceneStart = ImGui::glmVec2(ImGui::GetWindowPos());
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(scenePayloadName.c_str())) {
						if (payload->DataSize == sizeof(Editor::File)) {
							File* file = *static_cast<File**>(payload->Data);
							//if (file->extension == Standards::modelExtName) {
							if (file->extension == Standards::modelExtName) {
								//file->directory, file->name

								ModelLoader::LoadImportedModelToScene(ModelSerializer::ReadUUID(file->directory), file->directory);
							}
							delete(file);
						}
					}
					ImGui::EndDragDropTarget();
				}
			}
			ImGui::End();
		}

		// Create the Editor view
		inline void Gui::beginEditor(int gameFrameBuffer, Camera& camera) {
			PLAZA_PROFILE_SECTION("Begin Editor");
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			Entity* selectedGameObject = Editor::selectedGameObject;

			// Set the window to be the content size + header size
			ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove;

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus;
			windowFlags |= ImGuiWindowFlags_NoScrollbar;

			/*			windowFlags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoTitleBar;
			windowFlags |= ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiConfigFlags_DockingEnable | ImGuiWindowFlags_NoScrollbar;*/

			ImGui::SetNextWindowSize(ImVec2(appSizes.sceneSize.x, appSizes.sceneSize.y));
			// Get a reference to the current ImGui style
			ImGuiStyle& style = ImGui::GetStyle();

			// Adjust padding and margin sizes
			style.WindowPadding = ImVec2(0.0f, 0.0f);  // Change window padding
			if (ImGui::Begin("Editor", &Gui::isSceneOpen, windowFlags)) {
				Application->activeCamera = Application->editorCamera;
			};
			if (ImGui::IsWindowFocused())
			{
				if (Application->focusedMenu != "Editor") {
					glfwSetInputMode(Application->Window->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
				Application->focusedMenu = "Editor";
				Application->activeCamera = Application->editorCamera;
			}
			if (ImGui::IsWindowHovered())
				Application->hoveredMenu = "Editor";



			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2, 25));

			//ImGuiStyle& style = ImGui::GetStyle();

			// Change the background color of the button
			if (Application->runningScene) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			}

			if (ImGui::ImageButton("PlayPauseButton", ImTextureID(playPauseButtonImageId), ImVec2(25, 25))) {
				if (Application->runningScene)
					Scene::Stop();
				else
					Scene::Play();
			}
			ImGui::SameLine();
			if (ImGui::Button("Reload C# Script Assembly")) {
				ScriptManager::ReloadScriptsAssembly();
			}
			ImGui::PopStyleColor();

			ImVec2 uv0(0, 0); // bottom-left corner
			ImVec2 uv1(1, 1); // top-right corner
			appSizes.sceneImageStart = ImGui::glmVec2(ImGui::GetCursorScreenPos());

			ImGui::Image(ImTextureID(Application->mRenderer->GetFrameImage()), ImGui::imVec2(appSizes.sceneSize), uv0, uv1);

			// Show the gizmo if there's a selected entity
			std::map<std::string, File*> files = Editor::selectedFiles;
			selectedGameObject = Editor::selectedGameObject;
			if (selectedGameObject && selectedGameObject->GetComponent<Transform>() != nullptr && selectedGameObject->parentUuid != 0) {
				ImGuizmoHelper::IsDrawing = true;
				Editor::Gizmo::Draw(selectedGameObject, camera);
			}
			else {
				ImGuizmoHelper::IsDrawing = false;
			}

			//	appSizes.sceneStart = ImGui::glmVec2(ImGui::GetWindowPos());
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(scenePayloadName.c_str())) {
					if (payload->DataSize == sizeof(Editor::File)) {
						File* file = *static_cast<File**>(payload->Data);
						//if (file->extension == Standards::modelExtName) {
						if (file->extension == Standards::modelExtName) {
							//file->directory, file->name

							AssetsLoader::LoadAsset(AssetsManager::GetAsset(file->directory));
							//ModelLoader::LoadImportedModelToScene(ModelSerializer::ReadUUID(file->directory), file->directory);
						}
						delete(file);
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::End();
		}

		void Gui::beginHierarchyView(int gameFrameBuffer) {
			PLAZA_PROFILE_SECTION("Begin Hierarchy");
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			Entity* selectedGameObject = Editor::selectedGameObject;
			//ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiConfigFlags_DockingEnable | ImGuiWindowFlags_HorizontalScrollbar | ImGuiScrollFlags_NoScrollParent;

			//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Set window background to red//
			ImGui::SetNextWindowSize(ImVec2(appSizes.hierarchySize.x, appSizes.hierarchySize.y));
			ImGui::Begin("Hierarchy", &Gui::isHierarchyOpen, sceneWindowFlags);
			if (ImGui::IsWindowFocused())
				Application->focusedMenu = "Hierarchy";
			if (ImGui::IsWindowHovered()) {
				Application->hoveredMenu = "Hierarchy";
			}

			appSizes.hierarchySize.x = ImGui::GetWindowSize().x;
			HierarchyPopup::Update();

			if (Editor::selectedGameObject)
				Editor::selectedFiles.clear();

			// Create the main collapser
			ImGui::SetCursorPosX(0);

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
			Editor::Gui::Hierarchy::Item(Application->activeScene->entities[Application->activeScene->mainSceneEntity->uuid], selectedGameObject);
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();


			curHierarchySize = ImGui::glmVec2(ImGui::GetWindowSize());
			ImGui::End();
		}

		void Gui::beginInspector(int gameFrameBuffer, Camera camera) {
			PLAZA_PROFILE_SECTION("Begin Inspector");
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			Entity* selectedGameObject = Editor::selectedGameObject;

			//ImGui::SetCursorPosY(appSizes.appHeaderSize);
			ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.inspectorSize), ImGuiCond_Always);
			//ImGui::SetNextWindowPos(ImVec2(appSizes.appSize.x - appSizes.inspectorSize.x, 0));

			ImGuiWindowFlags sceneWindowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiConfigFlags_DockingEnable;
			ImGuiWindowFlags  inspectorWindowFlags = ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f)); // Remove the padding of the window
			//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Set window background to red//
			ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.inspectorSize));
			ImGui::Begin("Inspector", &Gui::isInspectorOpen, sceneWindowFlags);
			if (ImGui::IsWindowFocused())
				Application->focusedMenu = "Inspector";
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
				Application->hoveredMenu = "Inspector";


			//// Handle size changes
			//const ImVec2& CurSize = ImGui::GetWindowSize();
			//if (!ImGui::Compare(CurSize, ImGui::imVec2(lastAppSizes.inspectorSize))) {
			//	//appSizes.inspectorSize = ImGui::glmVec2(CurSize);
			//	lastAppSizes.inspectorSize = appSizes.inspectorSize;
			//	ImGui::SetWindowSize(ImGui::imVec2(appSizes.inspectorSize), ImGuiCond_Always);
			//	//ImGui::SetWindowPos(ImVec2(appSizes.appSize.x - appSizes.inspectorSize.x, 0));
			//}

			if (Editor::selectedFiles.size() > 0) {
				Editor::Inspector::FileInspector::CreateInspector();
			}
			else if (selectedGameObject) {
				if (selectedGameObject->parentUuid)
					Inspector::ComponentInspector::UpdateComponents();
				Editor::Inspector::ComponentInspector::CreateInspector();
				ImGui::Text(std::to_string(selectedGameObject->uuid).c_str());
			}


			curInspectorSize = ImGui::glmVec2(ImGui::GetWindowSize());
			ImGui::End();
			ImGui::PopStyleVar();
		}

		void Gui::beginImageViewer(int gameFrameBuffer, Camera camera) {
			PLAZA_PROFILE_SECTION("Begin Image Viewer");
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			Entity* selectedGameObject = Editor::selectedGameObject;

			ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.inspectorSize), ImGuiCond_Always);

			ImGuiWindowFlags sceneWindowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiConfigFlags_DockingEnable;
			ImGuiWindowFlags  inspectorWindowFlags = ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f)); // Remove the padding of the window
			//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Set window background to red//
			ImGui::SetNextWindowSize(ImGui::imVec2(appSizes.inspectorSize));
			ImGui::Begin("Inspector", &Gui::isInspectorOpen, sceneWindowFlags);
			if (ImGui::IsWindowFocused())
				Application->focusedMenu = "Inspector";
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
				Application->hoveredMenu = "Inspector";


			//// Handle size changes
			//const ImVec2& CurSize = ImGui::GetWindowSize();
			//if (!ImGui::Compare(CurSize, ImGui::imVec2(lastAppSizes.inspectorSize))) {
			//	//appSizes.inspectorSize = ImGui::glmVec2(CurSize);
			//	lastAppSizes.inspectorSize = appSizes.inspectorSize;
			//	ImGui::SetWindowSize(ImGui::imVec2(appSizes.inspectorSize), ImGuiCond_Always);
			//	//ImGui::SetWindowPos(ImVec2(appSizes.appSize.x - appSizes.inspectorSize.x, 0));
			//}

			if (Editor::selectedFiles.size() > 0) {
				Editor::Inspector::FileInspector::CreateInspector();
			}
			else if (selectedGameObject) {
				if (selectedGameObject->parentUuid)
					Inspector::ComponentInspector::UpdateComponents();
				Editor::Inspector::ComponentInspector::CreateInspector();
				ImGui::Text(std::to_string(selectedGameObject->uuid).c_str());
			}


			curInspectorSize = ImGui::glmVec2(ImGui::GetWindowSize());
			ImGui::End();
			ImGui::PopStyleVar();
		}

		void Gui::beginAssetsViewer(int gameFrameBuffer, Camera camera) {
			PLAZA_PROFILE_SECTION("Begin Assets Viewer");
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			Entity* selectedGameObject = Editor::selectedGameObject;

			// Set the window to be the content size + header size
			ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove;

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus;
			windowFlags |= ImGuiWindowFlags_NoScrollbar;

			ImGui::SetNextWindowSize(ImVec2(appSizes.sceneSize.x, appSizes.sceneSize.y));

			ImGuiStyle& style = ImGui::GetStyle();

			// Adjust padding and margin sizes
			style.WindowPadding = ImVec2(0.0f, 0.0f);  // Change window padding
			if (ImGui::Begin("Assets Viewer", &Gui::isSceneOpen, windowFlags)) {

			};
			if (ImGui::IsWindowFocused())
			{
				if (Application->focusedMenu != "AssetsViewer") {
					glfwSetInputMode(Application->Window->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
				Application->focusedMenu = "AssetsViewer";
			}
			if (ImGui::IsWindowHovered())
				Application->hoveredMenu = "AssetsViewer";

			for (auto& [key, value] : AssetsManager::mAssets) {
				if (ImGui::TreeNodeEx(value->mPath.filename().string().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {


					ImGui::Text("Uuid: ");
					ImGui::SameLine();
					ImGui::Text(std::to_string(value->mAssetUuid).c_str());

					ImGui::Text("Path: ");
					ImGui::SameLine();
					ImGui::Text(value->mPath.string().c_str());

					ImGui::Text("Extension: ");
					ImGui::SameLine();
					ImGui::Text(value->mAssetExtension.c_str());

					ImGui::TreePop();
				}
			}

			ImGui::End();
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