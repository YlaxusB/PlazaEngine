#include "Engine/Core/PreCompiledHeaders.h"
#include "Project.h"

#include "Engine/Application/FileDialog/FileDialog.h"
#include "Engine/Application/Serializer/SceneSerializer.h"
#include "Engine/Application/Serializer/ProjectSerializer.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
#include "Engine/Application/Serializer/ScriptManagerSerializer.h"
#include "Engine/Core/Scripting/Mono.h"
#include "Editor/Filewatcher.h"
#include "Editor/ScriptManager/ScriptManager.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"
#include "Engine/Application/Serializer/FileSerializer/FileSerializer.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"
#include "Engine/Core/AssetsManager/AssetsReader.h"

namespace Plaza::Editor {
	void Project::Load(const std::string filePath) {
		filesystem::path projectFile(filePath);

		std::string fileName = projectFile.filename().string();
		std::string extension = projectFile.extension().string();
		// Check if its extension is the project extension
		if (extension != Standards::projectExtName || !std::filesystem::exists(projectFile)) {
			Application::Get()->runEngine = false;
			std::cout << "Project has not been found!" << "\n";
			return;
		}

		Application::Get()->projectPath = projectFile.parent_path().string();

		Application::Get()->activeProject = std::make_unique<Project>(*AssetsSerializer::DeSerializeFile<Project>(filePath));
		Application::Get()->activeProject->mAssetPath = filePath;

#ifdef EDITOR_MODE
		Filewatcher::Start(Application::Get()->activeProject->mAssetPath.parent_path().string());
#endif

		Application::Get()->runEngine = true;
		Application::Get()->runProjectManagerGui = false;
		std::cout << "Mid \n";

#ifdef EDITOR_MODE
		Gui::FileExplorer::currentDirectory = Application::Get()->activeProject->mAssetPath.parent_path().string();
		Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
#endif // EDITOR_MODE

		std::cout << "Mono \n";
		Mono::Init();

		std::cout << "Read Game Folder \n";
		Application::Get()->editorScene = new Scene();
		Application::Get()->editorScene->mainSceneEntity = new Entity();
		Application::Get()->activeScene = Application::Get()->editorScene;
		std::map<std::string, Script> scripts = std::map<std::string, Script>();
		/* Iterate over all files and subfolders of the project folder to load assets */
		std::string path = Application::Get()->activeProject->mAssetPath.parent_path().string();
		for (auto entry = filesystem::recursive_directory_iterator(Application::Get()->activeProject->mAssetPath.parent_path(), filesystem::directory_options::skip_permission_denied); entry != filesystem::end(entry); ++entry) {
			if (entry->is_directory() && entry->path().filename().string().ends_with(".ignore")) {
				entry.disable_recursion_pending();
			}

			AssetsReader::ReadAssetAtPath(entry->path());
		}

		for (auto& [key, value] : AssetsManager::mMaterials) {
			//Application::Get()->mRenderer->LoadTexture(AssetsManager::GetAssetOrImport(FileDialog::OpenFileDialog(".jpeg"))->mPath.string())
			if (key == 0)
				continue;
			value->GetDeserializedTextures();
			if (value->diffuse->mAssetUuid)
				value->diffuse = AssetsManager::GetTexture(value->diffuse->mAssetUuid);
			if (value->normal->mAssetUuid)
				value->normal = AssetsManager::GetTexture(value->normal->mAssetUuid);
			if (value->roughness->mAssetUuid)
				value->roughness = AssetsManager::GetTexture(value->roughness->mAssetUuid);
			if (value->metalness->mAssetUuid)
				value->metalness = AssetsManager::GetTexture(value->metalness->mAssetUuid);
		}

		std::cout << "Deserializing \n";
		//ProjectSerializer::DeSerialize(filePath);

		PL_CORE_INFO(Application::Get()->activeProject->mLastSceneUuid);
		if (Application::Get()->activeProject->mLastSceneUuid != 0 && AssetsManager::GetAsset(Application::Get()->activeProject->mLastSceneUuid)) {
			const std::string sceneFilePath = AssetsManager::GetAsset(Application::Get()->activeProject->mLastSceneUuid)->mAssetPath.string();//Application::Get()->projectPath + "\\" + AssetsManager::lastActiveScenePath;
			bool sceneFileExists = std::filesystem::exists(sceneFilePath);
			if (sceneFileExists) {
				std::shared_ptr<Scene>* sc = new std::shared_ptr<Scene>(AssetsSerializer::DeSerializeFile<Scene>(sceneFilePath));
				Application::Get()->editorScene = sc->get();
				Application::Get()->activeScene = Application::Get()->editorScene;
				Application::Get()->activeScene->RecalculateAddedComponents();
			}
			//if (sceneFileExists)
			//	Serializer::DeSerialize(sceneFilePath, true);
		}
		else {
			Application::Get()->editorScene = new Scene();
			Application::Get()->activeScene = Application::Get()->editorScene;
			Application::Get()->activeScene->mainSceneEntity = new Entity("Scene");
			Application::Get()->activeScene->mainSceneEntity->parentUuid = Application::Get()->activeScene->mainSceneEntity->uuid;
			Application::Get()->activeScene->GetEntity(Application::Get()->activeScene->mainSceneEntity->uuid)->parentUuid = Application::Get()->activeScene->mainSceneEntity->uuid;
			Editor::DefaultModels::Init();
		}
		std::cout << "Finished Deserializing \n";
	}
}