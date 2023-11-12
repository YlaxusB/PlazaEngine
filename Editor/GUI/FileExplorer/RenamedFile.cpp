#include "Engine/Core/PreCompiledHeaders.h"
#include "RenamedFile.h"
namespace Plaza::Editor {
	void RenamedFileManager::Run(std::string oldPath, std::string newPath) {
		std::string newExtension = std::filesystem::path{ newPath }.extension().string();

		/* Material */
		if (newExtension == Standards::materialExtName) {
			if (Application->activeScene->materialsNames.find(oldPath) != Application->activeScene->materialsNames.end()) {
				uint64_t materialUuid = Application->activeScene->materialsNames.at(oldPath);
				Application->activeScene->materials.at(materialUuid)->name = newPath;
				Application->activeScene->materialsNames.emplace(newPath, materialUuid);
				Application->activeScene->materialsNames.erase(oldPath);
			}
		}
		/* C# Script */
		else if (newExtension == ".cs") {
			auto it = Application->activeProject->scripts.find(oldPath);
			if (it != Application->activeProject->scripts.end())
				Application->activeProject->scripts.emplace(newPath, it->second);
		}
	}
}