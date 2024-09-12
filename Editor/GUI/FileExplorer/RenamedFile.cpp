#include "Engine/Core/PreCompiledHeaders.h"
#include "RenamedFile.h"
namespace Plaza::Editor {
	void RenamedFileManager::Run(std::string oldPath, std::string newPath) {
		std::string newExtension = std::filesystem::path{ newPath }.extension().string();

		/* Material */
		if (newExtension == Standards::materialExtName) {
			if (AssetsManager::mMaterialsNames.find(oldPath) != AssetsManager::mMaterialsNames.end()) {
				uint64_t materialUuid = AssetsManager::mMaterialsNames.at(oldPath);
				AssetsManager::mMaterials.at(materialUuid)->mAssetName = newPath;
				AssetsManager::mMaterialsNames.emplace(newPath, materialUuid);
				AssetsManager::mMaterialsNames.erase(oldPath);
				AssetsManager::ChangeAssetPath(AssetsManager::mMaterials.at(materialUuid)->mAssetUuid, newPath);
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