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
			//
			if (ImGui::BeginMenu("File")) {
				if (ImGui::Button("Build")) {
					//std::string compileCommand = "mcs -target:library -out:\"" + dllPath.parent_path().string() + "\\" + dllPath.stem().string() + ".dll\" " + "\"" + std::string(scriptPath) + "\"";
					//compileCommand += " -reference:\"" + Application::Get()->dllPath + "\\PlazaScriptCore.dll\"";
					/*
					msbuild.exe "C:\Users\Giovane\Desktop\Workspace\Plaza\Engine\..\OpenGLEngine.sln" /p:Configuration=GameRel /t:Build /p:OutDir="C:\Users\Giovane\Desktop\Workspace\PlazaGames\daldal\build\\"
					*/
					std::string devEnv = " \"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\msbuild.exe\" ";
					std::string outPath = "\"" + Application::Get()->projectPath + "\\..\\" + Application::Get()->activeProject->name + "build\\\\\"";
					std::string command = "\"" + devEnv + "\"" + Application::Get()->enginePath + "\\..\\OpenGLEngine.sln\" /p:Configuration=GameRel /t:Build /p:PROJECT_NAME=YourMacroValue /p:OutDir=" + outPath + "\"";
					std::cout << command << std::endl;
					system(command.c_str());

					/* Copy assets and scripts into Content Folder inside build folder */
					try {
						filesystem::copy(Application::Get()->projectPath, Application::Get()->projectPath + "\\..\\" + Application::Get()->activeProject->name + "build", filesystem::copy_options::recursive);
						std::cout << "Project Folder copied successfully.\n";
					}
					catch (const filesystem::filesystem_error& e) {
						std::cerr << "Error: " << e.what() << std::endl;
					}
				}

				if (ImGui::Button("Return to Project Menu")) {
					Editor::selectedGameObject = nullptr;
					Application::Get()->runEngine = false;
					Application::Get()->runProjectManagerGui = true;
					Application::Get()->activeProject = new Project();
					Cache::Serialize(Application::Get()->enginePathAppData + "\\cache.yaml");
					//free(Application::Get()->editorScene);
					//free(Application::Get()->runtimeScene);
				}

				if (ImGui::Button("New Scene")) {
					std::string newPath = FileDialog::SaveFileDialog(("Engine (*.%s)", Standards::sceneExtName).c_str());
					if (!newPath.empty()) {
						Editor::selectedGameObject = nullptr;
						//free(Application::Get()->editorScene);
						//free(Application::Get()->runtimeScene);
						Application::Get()->editorScene = new Scene();
						Application::Get()->activeScene = Application::Get()->editorScene;
						Application::Get()->editorScene->mainSceneEntity = new Entity("Scene");
						Application::Get()->editorScene->entities.at(Application::Get()->editorScene->mainSceneEntity->uuid).parentUuid = Application::Get()->editorScene->mainSceneEntity->uuid;
						Application::Get()->editorScene->mainSceneEntity->parentUuid = Application::Get()->editorScene->mainSceneEntity->uuid;
						Serializer::Serialize(AssetsManager::NewAsset(AssetType::SCENE, newPath));
						if (newPath.starts_with(Application::Get()->projectPath))
							Application::Get()->editorScene->filePath = newPath.substr(Application::Get()->projectPath.length() + 1, newPath.length() - Application::Get()->projectPath.length());
						ProjectSerializer::Serialize(Application::Get()->projectPath + "\\" + Application::Get()->activeProject->name + Standards::projectExtName);
						Serializer::DeSerialize(newPath);
					}
				}
				if (ImGui::Button("Save Scene")) {
					//std::string path = Application::Get()->projectPath + "\\" + Application::Get()->activeScene->filePath;
					/* TODO: IMPLEMENT PROPER SCENE ASSET LOADER */
					Asset* temporarySceneAsset = new Asset();
					temporarySceneAsset->mAssetUuid = Application::Get()->activeScene->mAssetUuid;
					temporarySceneAsset->mAssetPath = std::filesystem::path{ Application::Get()->activeProject->directory + "\\" + Application::Get()->activeScene->filePath };
					temporarySceneAsset->mAssetExtension = Standards::sceneExtName;
					Serializer::Serialize(temporarySceneAsset);
					ProjectSerializer::Serialize(Application::Get()->projectPath + "\\" + Application::Get()->activeProject->name + Standards::projectExtName);
				}
				if (ImGui::Button("Save Scene As...")) {
					std::string path = FileDialog::SaveFileDialog(("Engine (*.%s)", Standards::sceneExtName).c_str());
					if (!path.empty()) {
						Serializer::Serialize(AssetsManager::NewAsset(AssetType::SCENE, path));
						if (path.starts_with(Application::Get()->projectPath))
							Application::Get()->editorScene->filePath = path.substr(Application::Get()->projectPath.length() + 1, path.length() - Application::Get()->projectPath.length());
						ProjectSerializer::Serialize(Application::Get()->projectPath + "\\" + Application::Get()->activeProject->name + Standards::projectExtName);
					}
				}
				if (ImGui::Button("Open Scene")) {
					std::string path = FileDialog::OpenFileDialog(("Engine (*.%s)", Standards::sceneExtName).c_str());
					if (std::filesystem::exists(path)) {
						Editor::selectedGameObject = nullptr;
						Serializer::DeSerialize(path);
						if (path.starts_with(Application::Get()->projectPath))
							Application::Get()->editorScene->filePath = path.substr(Application::Get()->projectPath.length() + 1, path.length() - Application::Get()->projectPath.length());
						ProjectSerializer::Serialize(Application::Get()->projectPath + "\\" + Application::Get()->activeProject->name + Standards::projectExtName);
					}
				}
				ImGui::EndMenu();
			}




			// Create an invisible margin to align the buttons to the right
			float spacingX = Application::Get()->appSizes->appSize.x - 150;
			ImGui::Dummy(ImVec2(spacingX, 0));

			// Create the right corner buttons
			ImGui::SameLine();
			ImVec2 buttonSize = ImVec2(25, 25);
			if (ImGui::Button("-", buttonSize)) {
				glfwIconifyWindow(Application::Get()->mWindow->glfwWindow);
			}
			ImGui::SameLine();
			if (ImGui::Button("□", buttonSize)) {
				glfwMaximizeWindow(Application::Get()->mWindow->glfwWindow);
			}
			ImGui::SameLine();
			if (ImGui::Button("X", buttonSize)) {
				glfwSetWindowShouldClose(Application::Get()->mWindow->glfwWindow, true);
			}

			ImGui::EndMainMenuBar();


		}
	}
}