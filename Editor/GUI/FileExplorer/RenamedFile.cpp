#include "Engine/Core/PreCompiledHeaders.h"
#include "RenamedFile.h"
namespace Plaza::Editor {
	void RenamedFileManager::Run(std::string oldPath, std::string newPath) {
		std::string newExtension = std::filesystem::path{ newPath }.extension().string();

		/* Material */
		if (newExtension == Standards::materialExtName) {
			uint64_t materialUuid = Application->activeScene->materialsNames.at(oldPath);
			Application->activeScene->materials.at(materialUuid)->name = newPath;
			Application->activeScene->materialsNames.emplace(newPath, materialUuid);
			Application->activeScene->materialsNames.erase(oldPath);
		}
	}
}