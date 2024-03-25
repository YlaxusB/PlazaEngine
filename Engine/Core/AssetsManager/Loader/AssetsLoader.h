#pragma once
#include "Engine/Core/AssetsManager/Metadata/Metadata.h"

namespace Plaza {
	class AssetsLoader {
	public:
		static inline const std::unordered_set<std::string> mSupportedLoadFormats = {
			Standards::metadataExtName,
			Standards::modelExtName,
			Standards::materialExtName
		};

		static void LoadAsset(Asset* asset) {

			if (asset->mAssetExtension == Standards::metadataExtName)
				AssetsLoader::LoadMetadata(asset);
			else if (asset->mAssetExtension == Standards::modelExtName)
				AssetsLoader::LoadPrefab(asset->mPath.string());
			else if (asset->mAssetExtension == Standards::materialExtName)
				AssetsLoader::LoadMaterial(asset);
		}
		static void LoadMetadata(Asset* asset) {
			Asset metadataContent = Metadata::DeSerializeMetadata(asset->mPath.string());
			if (AssetsManager::mAssetTypeByExtension.find(metadataContent.mAssetExtension) == AssetsManager::mAssetTypeByExtension.end())
				return;
			AssetType type = AssetsManager::mAssetTypeByExtension.at(metadataContent.mAssetExtension);

			switch (type) {
			case AssetType::TEXTURE:
				AssetsLoader::LoadTexture(&metadataContent);
				break;
			}
		}
		static void LoadPrefab(std::string path);
		static void LoadScript(Asset* asset) {};
		static void LoadTexture(Asset* asset) {};
		static void LoadMaterial(Asset* asset) {
			Material* material = new Material();
			material->uuid = asset->mAssetUuid;
			material->name = asset->mPath.filename().string();
			Application->activeScene->AddMaterial(material);
		};
		static void LoadModel(Asset* asset) {};
	};
}