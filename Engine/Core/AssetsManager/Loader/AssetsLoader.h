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

		static inline const std::unordered_set<std::string> mSupportedTextureLoadFormats = {
			".png",
			".jpg",
			".jpeg",
			".dds",
			".hdr",
			".tga"
		};

		static void LoadAsset(Asset* asset) {

			if (asset->mAssetExtension == Standards::metadataExtName)
				AssetsLoader::LoadMetadata(asset);
			else if (asset->mAssetExtension == Standards::modelExtName)
				AssetsLoader::LoadPrefab(asset);
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
		static void LoadPrefab(Asset* asset);
		static void LoadPrefabToMemory(Asset* asset);
		static void LoadPrefabToScene(LoadedModel* model, bool loadToScene);
		static void LoadScript(Asset* asset) {};
		static Texture* LoadTexture(Asset* asset) {
			if (!asset)
				return new Texture();
			if (AssetsManager::mTextures.find(asset->mAssetUuid) != AssetsManager::mTextures.end())
				return AssetsManager::mTextures.at(asset->mAssetUuid);

			Texture* texture = Application->mRenderer->LoadTexture(asset->mPath.string());
			texture->mAssetUuid = asset->mAssetUuid;
			AssetsManager::mTextures.emplace(asset->mAssetUuid, texture);
			return texture;
		}
		static Material* LoadMaterial(Asset* asset);
		static void LoadModel(Asset* asset) {};
	};
}