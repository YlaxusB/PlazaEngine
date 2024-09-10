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
		if (extension != Standards::projectExtName || !std::filesystem::exists(projectFile)) {
			Application::Get()->runEngine = false;
			std::cout << "Project has not been found!" << "\n";
			return;
		}

		Application::Get()->projectPath = projectFile.parent_path().string();

 		Application::Get()->activeProject = new Project(AssetsSerializer::DeSerializeFile<Project>(filePath));
		Project* proj = Application::Get()->activeProject;

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
		std::map<std::string, Script> scripts = std::map<std::string, Script>();
		/* Iterate over all files and subfolders of the project folder to load assets */
		std::string path = Application::Get()->activeProject->mAssetPath.parent_path().string();
		for (auto entry = filesystem::recursive_directory_iterator(Application::Get()->activeProject->mAssetPath.parent_path(), filesystem::directory_options::skip_permission_denied); entry != filesystem::end(entry); ++entry) {
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
					}
					else if (extension == ".plzmod") {
						std::ifstream binaryFile(entry->path(), std::ios::binary);
						uint64_t uuid = 0;
						binaryFile.read(reinterpret_cast<char*>(&uuid), sizeof(uint64_t));
						binaryFile.close();
						AssetsManager::NewAsset(uuid, AssetType::MODEL, entry->path().string());
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
		//ProjectSerializer::DeSerialize(filePath);
		std::cout << "Finished Deserializing \n";
	}
}