#include "Engine/Core/PreCompiledHeaders.h"
#include "MenuBar.h"
#include "Engine/Application/FileDialog/FileDialog.h"
#include "Engine/Application/Serializer/SceneSerializer.h"
#include "Engine/Application/Serializer/ProjectSerializer.h"
#include "Engine/Core/Standards.h"
#include "Editor/SessionCache/Cache.h"
namespace Plaza {
	namespace Editor {
		void Gui::MainMenuBar::Begin() {
			// MenuBar / TitleBar
			ImGui::BeginMainMenuBar();

			if (ImGui::BeginMenu("File")) {
				if (ImGui::Button("Return to Project Menu")) {
					Editor::selectedGameObject = nullptr;
					Application->runEngine = false;
					Application->runProjectManagerGui = true;
					Application->activeProject = new Project();
					Cache::Serialize(Application->enginePathAppData + "\\cache.yaml");
					//free(Application->editorScene);
					//free(Application->runtimeScene);
				}

				if (ImGui::Button("New Scene")) {
					std::string newPath = FileDialog::SaveFileDialog(("Engine (*.%s)", Standards::sceneExtName).c_str());
					if (!newPath.empty()) {
						Editor::selectedGameObject = nullptr;
						//free(Application->editorScene);
						//free(Application->runtimeScene);
						Application->editorScene = new Scene();
						Application->editorScene->mainSceneEntity = new Entity("Scene");
						Application->activeScene = Application->editorScene;
						Serializer::Serialize(newPath);
						if (newPath.starts_with(Application->projectPath))
							Application->editorScene->filePath = newPath.substr(Application->projectPath.length() + 1, newPath.length() - Application->projectPath.length());
						ProjectSerializer::Serialize(Application->projectPath + "\\" + Application->activeProject->name + Standards::projectExtName);
						Serializer::DeSerialize(newPath);
					}
				}
				if (ImGui::Button("Save Scene")) {
					Serializer::Serialize(Application->projectPath + "\\" + Application->activeScene->filePath);
					ProjectSerializer::Serialize(Application->projectPath + "\\" + Application->activeProject->name + Standards::projectExtName);
				}
				if (ImGui::Button("Save Scene As...")) {
					std::string path = FileDialog::SaveFileDialog(("Engine (*.%s)", Standards::sceneExtName).c_str());
					if (!path.empty()) {
						Serializer::Serialize(path);
						if (path.starts_with(Application->projectPath))
							Application->editorScene->filePath = path.substr(Application->projectPath.length() + 1, path.length() - Application->projectPath.length());
						ProjectSerializer::Serialize(Application->projectPath + "\\" + Application->activeProject->name + Standards::projectExtName);
					}
				}
				if (ImGui::Button("Open Scene")) {
					std::string path = FileDialog::OpenFileDialog(("Engine (*.%s)", Standards::sceneExtName).c_str());
					if (std::filesystem::exists(path)) {
						Editor::selectedGameObject = nullptr;
						Serializer::DeSerialize(path);
						if (path.starts_with(Application->projectPath))
							Application->editorScene->filePath = path.substr(Application->projectPath.length() + 1, path.length() - Application->projectPath.length());
						ProjectSerializer::Serialize(Application->projectPath + "\\" + Application->activeProject->name + Standards::projectExtName);
					}
				}
				ImGui::EndMenu();
			}




			// Create an invisible margin to align the buttons to the right
			float spacingX = Application->appSizes->appSize.x - 150;
			ImGui::Dummy(ImVec2(spacingX, 0));

			// Create the right corner buttons
			ImGui::SameLine();
			ImVec2 buttonSize = ImVec2(25, 25);
			if (ImGui::Button("-", buttonSize)) {
				glfwIconifyWindow(Application->Window->glfwWindow);
			}
			ImGui::SameLine();
			if (ImGui::Button("□", buttonSize)) {
				glfwMaximizeWindow(Application->Window->glfwWindow);
			}
			ImGui::SameLine();
			if (ImGui::Button("X", buttonSize)) {
				glfwSetWindowShouldClose(Application->Window->glfwWindow, true);
			}

			ImGui::EndMainMenuBar();


		}
	}
}