#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsManager.h"

#include "Engine/Core/AssetsManager/Importer/AssetsImporter.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"
#include "Engine/Core/AssetsManager/AssetsReader.h"

namespace Plaza {
	Asset* AssetsManager::GetAssetOrImport(std::string path, uint64_t uuid, std::string outDirectory) {
		Asset* asset = AssetsManager::GetAsset(path);
		if (!asset) {
			std::string importedAssetPath = AssetsImporter::ImportAsset(path, uuid, AssetsImporterSettings{ outDirectory });
			asset = AssetsManager::GetAsset(importedAssetPath);
			if (asset)
				AssetsLoader::LoadAsset(asset);
		}
		return asset;
	}

	void AssetsManager::ReadFolderContent(std::string path, bool readSubFolders) {
		for (auto entry = filesystem::recursive_directory_iterator(path, filesystem::directory_options::skip_permission_denied); entry != filesystem::end(entry); ++entry) {
			if (entry->is_directory() && entry->path().filename().string().ends_with(".ignore")) {
				entry.disable_recursion_pending();
			}

			AssetsReader::ReadAssetAtPath(entry->path());
		}
	}
}