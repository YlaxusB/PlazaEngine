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

namespace Plaza::Editor {
	void Project::Load(const std::string filePath) {
		filesystem::path projectFile(filePath);

		std::string fileName = projectFile.filename().string();
		std::string extension = projectFile.extension().string();
		// Check if its extension is the project extension
		if (extension != Standards::projectExtName) {
			Application::Get()->runEngine = false;
			std::cout << "Project has not been found!" << "\n";
			return;
		}


		std::cout << filePath << std::endl;
		Application::Get()->projectPath = projectFile.parent_path().string();


		Application::Get()->activeProject = new Project();
		Application::Get()->activeProject->name = fileName;
		Application::Get()->activeProject->directory = projectFile.parent_path().string();
		Application::Get()->activeProject->scriptsConfigFilePath = Application::Get()->activeProject->directory + "\\Scripts" + Standards::scriptConfigExtName;
		//Application::Get()->activeProject->scripts = ScriptManagerSerializer::DeSerialize(Application::Get()->activeProject->scriptsConfigFilePath);
		#ifdef EDITOR_MODE
		Filewatcher::Start(Application::Get()->activeProject->directory);
		#endif

		Application::Get()->runEngine = true;
		Application::Get()->runProjectManagerGui = false;
		std::cout << "Mid \n";
		//this->currentContent = new NewProjectContent();

#ifdef EDITOR_MODE
		Gui::FileExplorer::currentDirectory = Application::Get()->activeProject->directory;
		Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
#endif // !GAME_REL

		Application::Get()->activeProject->directory = projectFile.parent_path().string();


		//free(Application::Get()->editorScene);
		//free(Application::Get()->runtimeScene);

		Application::Get()->editorScene = new Scene();
		Application::Get()->runtimeScene = new Scene();
		Application::Get()->editorScene->mainSceneEntity = new Entity("Scene");
		Application::Get()->activeScene = Application::Get()->editorScene;

		std::cout << "Mono \n";
		Mono::Init();

		std::cout << "Read Game Folder \n";
		std::map<std::string, Script> scripts = std::map<std::string, Script>();
		/* Iterate over all files and subfolders of the project folder to load assets */
		for (auto entry = filesystem::recursive_directory_iterator(Application::Get()->activeProject->directory, filesystem::directory_options::skip_permission_denied); entry != filesystem::end(entry); ++entry) {
			if (entry->is_directory() && entry->path().filename().string().ends_with(".ignore")) {
				entry.disable_recursion_pending();
			}

			const std::string extension = entry->path().extension().string();
			if (entry->is_regular_file() && extension != "")
			{
				if (extension == Standards::metadataExtName) {
					Asset* asset = AssetsManager::LoadMetadataAsAsset(entry->path());
					if (AssetsLoader::mSupportedTextureLoadFormats.find(asset->mAssetExtension) != AssetsLoader::mSupportedTextureLoadFormats.end()) {
						AssetsManager::mTextures.emplace(asset->mAssetUuid, Application::Get()->mRenderer->LoadTexture(asset->mAssetPath.string(), asset->mAssetUuid));
					}
				}
				else if (AssetsLoader::mSupportedLoadFormats.find(extension) != AssetsLoader::mSupportedLoadFormats.end()) {
					Asset* asset = AssetsManager::LoadBinaryFileAsAsset(entry->path());
					if (extension == ".plzmat" || extension == Standards::animationExtName)
					{
						AssetsLoader::LoadAsset(asset);
						//AssetsManager::AddAsset(asset);
					}
					else if (extension == ".plzmod") {
						std::ifstream binaryFile(entry->path(), std::ios::binary);
						uint64_t uuid = 0;
						binaryFile.read(reinterpret_cast<char*>(&uuid), sizeof(uint64_t));
						binaryFile.close();
						AssetsManager::NewAsset(uuid, AssetType::MODEL, entry->path().string());
						//AssetsManager::LoadMetadataAsAsset(entry.path());
						//AssetsLoader::LoadPrefabToMemory(asset);
					}
					else if (extension == Standards::animationExtName) {
						std::ifstream binaryFile(entry->path(), std::ios::binary);
						uint64_t uuid = 0;
						binaryFile.read(reinterpret_cast<char*>(&uuid), sizeof(uint64_t));
						binaryFile.close();
						Asset* asset = AssetsManager::NewAsset(uuid, AssetType::ANIMATION, entry->path().string());
						AssetsLoader::LoadAnimation(asset);
					}
				}

				if (entry->is_regular_file() && entry->path().extension() == ".cs") {
					scripts.emplace(entry->path().string(), Script());
				}
			}
		}

		for (auto& [key, value] : Application::Get()->activeScene->materials) {
			//Application::Get()->mRenderer->LoadTexture(AssetsManager::GetAssetOrImport(FileDialog::OpenFileDialog(".jpeg"))->mPath.string())
			if (key == 0)
				continue;
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
		ProjectSerializer::DeSerialize(filePath);
		std::cout << "Finished Deserializing \n";
		Application::Get()->activeProject->scripts = scripts;

	}
}