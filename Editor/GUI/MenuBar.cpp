#include "Engine/Core/PreCompiledHeaders.h"
#include "MenuBar.h"
#include "Engine/Application/FileDialog/FileDialog.h"
#include "Engine/Core/Standards.h"
#include "Editor/SessionCache/Cache.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"
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
					std::string outPath = "\"" + Application::Get()->projectPath + "\\..\\" + Application::Get()->activeProject->mAssetName + "build\\\\\"";
					std::string command = "\"" + devEnv + "\"" + Application::Get()->enginePath + "\\..\\OpenGLEngine.sln\" /p:Configuration=GameRel /t:Build /p:PROJECT_NAME=YourMacroValue /p:OutDir=" + outPath + "\"";
					std::cout << command << std::endl;
					system(command.c_str());

					/* Copy assets and scripts into Content Folder inside build folder */
					try {
						filesystem::copy(Application::Get()->projectPath, Application::Get()->projectPath + "\\..\\" + Application::Get()->activeProject->mAssetName + "build", filesystem::copy_options::recursive);
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
					Application::Get()->activeProject = std::make_unique<Project>();
					Cache::Serialize(Application::Get()->enginePathAppData + "\\cache.yaml");
					Application::Get()->focusedMenu = "ProjectManager";
					//free(Scene::GetEditorScene());
					//free(Application::Get()->runtimeScene);
				}

				if (ImGui::Button("New Scene")) {
					std::string newPath = FileDialog::SaveFileDialog(("Engine (*.%s)", Standards::sceneExtName).c_str());
					if (!newPath.empty()) {
						Editor::selectedGameObject = nullptr;
						Scene::SetEditorScene(*new std::shared_ptr<Scene>(AssetsManager::NewAsset<Scene>(newPath)));
						Scene::SetActiveScene(Scene::GetEditorScene());
						Scene::GetActiveScene()->mainSceneEntity = new Entity("Scene");
						Scene::GetActiveScene()->mainSceneEntity->parentUuid = Scene::GetActiveScene()->mainSceneEntity->uuid;

						AssetsSerializer::SerializeFile<Scene>(*Scene::GetEditorScene(), newPath);
						Application::Get()->activeProject->mLastSceneUuid = Scene::GetEditorScene()->mAssetUuid;
						AssetsSerializer::SerializeFile<Project>(*Application::Get()->activeProject, Application::Get()->activeProject->mAssetPath.string());
						AssetsLoader::LoadScene(AssetsSerializer::DeSerializeFile<Asset>(newPath).get());
					}
				}
				if (ImGui::Button("Save Scene")) {
					if (!Scene::GetActiveScene()->mAssetPath.empty() && std::filesystem::exists(Scene::GetActiveScene()->mAssetPath)) {
						AssetsSerializer::SerializeFile(*Scene::GetActiveScene(), AssetsManager::GetAsset(Scene::GetActiveScene()->mAssetUuid)->mAssetPath.string());
						Application::Get()->activeProject->mLastSceneUuid = Scene::GetActiveScene()->mAssetUuid;
						AssetsSerializer::SerializeFile<Project>(*Application::Get()->activeProject, Application::Get()->activeProject->mAssetPath.string());
					}
				}
				if (ImGui::Button("Save Scene As...")) {
					std::string path = FileDialog::SaveFileDialog(("Engine (*.%s)", Standards::sceneExtName).c_str());
					if (!path.empty()) {
						bool sceneFileIsEditorScene = path == AssetsManager::GetAsset(Scene::GetActiveScene()->mAssetUuid)->mAssetPath.string();

						Scene::GetActiveScene()->mAssetPath = path;
						Scene::GetActiveScene()->mAssetName = std::filesystem::path{ path }.filename().string();
						AssetsSerializer::SerializeFile(*Scene::GetActiveScene(), path);

						Application::Get()->activeProject->mLastSceneUuid = Scene::GetActiveScene()->mAssetUuid;
						AssetsSerializer::SerializeFile<Project>(*Application::Get()->activeProject, Application::Get()->activeProject->mAssetPath.string());

						AssetsLoader::LoadScene(AssetsSerializer::DeSerializeFile<Asset>(path).get());
					}

				}
				if (ImGui::Button("Open Scene")) {
					std::string path = FileDialog::OpenFileDialog(("Engine (*.%s)", Standards::sceneExtName).c_str());
					if (std::filesystem::exists(path)) {
						Scene::SetEditorScene(AssetsSerializer::DeSerializeFile<Scene>(path));
						Scene::SetActiveScene(Scene::GetEditorScene());
						Scene::GetActiveScene()->RecalculateAddedComponents();
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