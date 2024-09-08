#include "Engine/Core/PreCompiledHeaders.h"
#include "RenamedFile.h"
namespace Plaza::Editor {
	void RenamedFileManager::Run(std::string oldPath, std::string newPath) {
		std::string newExtension = std::filesystem::path{ newPath }.extension().string();

		/* Material */
		if (newExtension == Standards::materialExtName) {
			if (Application::Get()->activeScene->materialsNames.find(oldPath) != Application::Get()->activeScene->materialsNames.end()) {
				uint64_t materialUuid = Application::Get()->activeScene->materialsNames.at(oldPath);
				Application::Get()->activeScene->materials.at(materialUuid)->mAssetName = newPath;
				Application::Get()->activeScene->materialsNames.emplace(newPath, materialUuid);
				Application::Get()->activeScene->materialsNames.erase(oldPath);
				AssetsManager::ChangeAssetPath(Application::Get()->activeScene->materials.at(materialUuid)->mAssetUuid, newPath);
			}
		}
		/* C# Script */
		else if (newExtension == ".cs") {
			auto it = Application::Get()->activeProject->scripts.find(oldPath);
			if (it != Application::Get()->activeProject->scripts.end())
				Application::Get()->activeProject->scripts.emplace(newPath, it->second);
		}
	}
}