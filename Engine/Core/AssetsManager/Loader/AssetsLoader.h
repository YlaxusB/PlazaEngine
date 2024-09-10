#pragma once
#include "Engine/Core/AssetsManager/Metadata/Metadata.h"
#include "ThirdParty/cereal/cereal/types/polymorphic.hpp"

namespace Plaza {
	class AssetsLoader {
	public:
		static inline const std::unordered_set<std::string> mSupportedLoadFormats = {
			Standards::metadataExtName,
			Standards::modelExtName,
			Standards::materialExtName,
			Standards::animationExtName
		};

		static inline const std::unordered_set<std::string> mSupportedTextureLoadFormats = {
			".png",
			".jpg",
			".jpeg",
			".dds",
			".hdr",
			".tga"
		};

		static void LoadAsset(Asset* asset);
		static void LoadMetadata(Asset* asset) {
			Asset metadataContent = Metadata::DeSerializeMetadata(asset->mAssetPath.string());
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

			Texture* texture = Application::Get()->mRenderer->LoadTexture(asset->mAssetPath.string());
			texture->mAssetUuid = asset->mAssetUuid;
			AssetsManager::mTextures.emplace(asset->mAssetUuid, texture);
			return texture;
		}
		static Material* LoadMaterial(Asset* asset, Scene* scene = nullptr);
		static void LoadModel(Asset* asset) {};
		static Animation& LoadAnimation(Asset* asset, Scene* scene = nullptr);
	};
}