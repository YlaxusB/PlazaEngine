#pragma once
#include "Engine/Core/AssetsManager/Importer/AssetsImporter.h"

namespace Plaza {
	class Metadata {
	public:
		struct MetadataStructure : public Asset {
			AssetExtension mExtension;
			std::string mContentName;

			template <class Archive>
			void serialize(Archive& archive) {
				archive(PL_SER(mAssetUuid), PL_SER(mAssetName), PL_SER(mExtension), PL_SER(mContentName));
			}
		};

		static void CreateMetadataFile(Asset* asset) {
			MetadataStructure metadata;
			metadata.mAssetUuid = asset->mAssetUuid;
			metadata.mAssetName = asset->mAssetName + Standards::metadataExtName;
			metadata.mAssetPath = asset->mAssetPath;
			metadata.mContentName = asset->mAssetName;
			std::filesystem::path metadataPath = asset->mAssetPath;
			metadataPath.replace_extension(Standards::metadataExtName).string();
			AssetsSerializer::SerializeFile<MetadataStructure>(metadata, metadataPath.string(), Application::Get()->mSettings.mMetaDataSerializationMode);
		}

		static Asset ConvertMetadataToAsset(MetadataStructure metadata) {
			std::string contentPath = metadata.mAssetPath.parent_path().string() + "\\" + metadata.mContentName;
			Asset asset(metadata.mAssetUuid, metadata.mAssetPath.filename().string(), contentPath);
			asset.mAssetName = metadata.mContentName;
			return asset;
		}
	};
}