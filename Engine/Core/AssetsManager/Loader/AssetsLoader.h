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
			Standards::animationExtName,
			Standards::sceneExtName
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
			Metadata::MetadataStructure metadata = *AssetsSerializer::DeSerializeFile<Metadata::MetadataStructure>(asset->mAssetPath.string(), Application::Get()->mSettings.mMetaDataSerializationMode).get();
			std::string metadataContentExtension = std::filesystem::path{ asset->mAssetPath.parent_path().string() + "\\" + metadata.mContentName }.extension().string();
			bool metadataContentExtensionIsSupported = AssetsManager::mAssetTypeByExtension.find(metadataContentExtension) != AssetsManager::mAssetTypeByExtension.end();
			if (!metadataContentExtensionIsSupported)
				return;
			AssetType type = AssetsManager::mAssetTypeByExtension.at(metadataContentExtension);

			switch (type) {
			case AssetType::TEXTURE:
				AssetsLoader::LoadTexture(AssetsManager::NewAsset<Asset>(std::make_shared<Asset>(Metadata::ConvertMetadataToAsset(metadata))));
				break;
			case AssetType::SCRIPT:
				AssetsLoader::LoadScript(AssetsManager::NewAsset<Asset>(std::make_shared<Asset>(Metadata::ConvertMetadataToAsset(metadata))));
				break;
			default:
				AssetsManager::NewAsset<Asset>(std::make_shared<Asset>(Metadata::ConvertMetadataToAsset(metadata)));
				break;
			}
		}
		static std::shared_ptr<Scene> LoadScene(Asset* asset, SerializationMode serializationMode);
		static void LoadPrefab(Asset* asset);
		static void LoadPrefabToMemory(Asset* asset);
		static void LoadPrefabToScene(LoadedModel* model, bool loadToScene);
		static void LoadScript(Asset* asset) {
			AssetsManager::AddScript(static_cast<Script*>(asset));
		};
		static Texture* LoadTexture(Asset* asset) {
			if (!asset)
				return new Texture();
			if (AssetsManager::GetTexture(asset->mAssetUuid) != nullptr)
				return AssetsManager::GetTexture(asset->mAssetUuid);

			Texture* texture = Application::Get()->mRenderer->LoadTexture(asset->mAssetPath.string());
			texture->mAssetUuid = asset->mAssetUuid;
			AssetsManager::mTextures.emplace(asset->mAssetUuid, texture);
			return texture;
		}
		static Material* LoadMaterial(Asset* asset, SerializationMode serializationMode);
		static void LoadModel(Asset* asset) {};
		static Animation& LoadAnimation(Asset* asset, SerializationMode serializationMode);
	};
}