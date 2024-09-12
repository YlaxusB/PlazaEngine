#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsReader.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"

namespace Plaza {
	Asset* AssetsReader::ReadAssetAtPath(std::filesystem::path path) {
		std::string extension = path.extension().string();
		if (AssetsLoader::mSupportedLoadFormats.find(extension) == AssetsLoader::mSupportedLoadFormats.end())
			return nullptr;
		Asset* asset = AssetsManager::NewAsset(AssetsSerializer::DeSerializeFile<Asset>(path.string()));
		AssetsLoader::LoadAsset(asset);
		return asset;
	}
}

//static inline string engineExtName = ".plz";
//static inline string projectExtName = engineExtName + "prj";
//static inline string sceneExtName = engineExtName + "scn";
//static inline string materialExtName = engineExtName + "mat";
//static inline string modelExtName = engineExtName + "mod";
//static inline string scriptConfigExtName = engineExtName + "scp";
//static inline string metadataExtName = engineExtName + "meta";
//static inline string animationExtName = engineExtName + "anim";

//const std::string extension = entry->path().extension().string();
//if (entry->is_regular_file() && extension != "")
//{
//	if (extension == Standards::metadataExtName) {
//		Asset* asset = AssetsManager::LoadMetadataAsAsset(entry->path());
//		if (AssetsLoader::mSupportedTextureLoadFormats.find(asset->GetExtension()) != AssetsLoader::mSupportedTextureLoadFormats.end()) {
//			AssetsManager::mTextures.emplace(asset->mAssetUuid, Application::Get()->mRenderer->LoadTexture(asset->mAssetPath.string(), asset->mAssetUuid));
//		}
//	}
//	else if (AssetsLoader::mSupportedLoadFormats.find(extension) != AssetsLoader::mSupportedLoadFormats.end()) {
//		Asset* asset = AssetsManager::LoadBinaryFileAsAsset(entry->path());
//		if (extension == ".plzmat" || extension == Standards::animationExtName)
//		{
//			AssetsLoader::LoadAsset(asset);
//		}
//		else if (extension == ".plzmod") {
//			std::ifstream binaryFile(entry->path(), std::ios::binary);
//			uint64_t uuid = 0;
//			binaryFile.read(reinterpret_cast<char*>(&uuid), sizeof(uint64_t));
//			binaryFile.close();
//			AssetsManager::NewAsset(uuid, AssetType::MODEL, entry->path().string());
//		}
//		else if (extension == Standards::animationExtName) {
//			std::ifstream binaryFile(entry->path(), std::ios::binary);
//			uint64_t uuid = 0;
//			binaryFile.read(reinterpret_cast<char*>(&uuid), sizeof(uint64_t));
//			binaryFile.close();
//			Asset* asset = AssetsManager::NewAsset(uuid, AssetType::ANIMATION, entry->path().string());
//			AssetsLoader::LoadAnimation(asset);
//		}
//	}
//
//	if (entry->is_regular_file() && entry->path().extension() == ".cs") {
//		scripts.emplace(entry->path().string(), Script());
//	}
//}