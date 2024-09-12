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
				archive(mAssetUuid, mAssetName, mExtension, mContentName);
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
			AssetsSerializer::SerializeFile<MetadataStructure>(metadata, metadataPath.string());
			//Metadata::SerializeMetadata(asset->mAssetUuid, asset->mAssetPath.string(), asset->GetExtension());
		}

		static Asset ConvertMetadataToAsset(MetadataStructure metadata) {
			std::string contentPath = metadata.mAssetPath.parent_path().string() + "\\" + metadata.mContentName;
			Asset asset(metadata.mAssetUuid, metadata.mAssetPath.filename().string(), contentPath);
			return asset;
		}

		static void SerializeMetadata(uint64_t uuid, std::string assetPath, std::string extension) {
			std::filesystem::path path(assetPath);
			std::string outName = path.parent_path().string() + "\\" + path.filename().string() + Standards::metadataExtName;
			std::ofstream binaryFile(outName, std::ios::binary);

			binaryFile.write(reinterpret_cast<const char*>(&uuid), sizeof(uuid));
			Plaza::Utils::SaveStringAsBinary(binaryFile, assetPath);
			Plaza::Utils::SaveStringAsBinary(binaryFile, extension);

			binaryFile.close();
		}

		static Asset DeSerializeMetadata(std::string metadaPath) {
			std::ifstream binaryFile(metadaPath, std::ios::binary);
			Asset asset = Asset();

			binaryFile.read(reinterpret_cast<char*>(&asset.mAssetUuid), sizeof(uint64_t));
			asset.mAssetPath = std::filesystem::path{ Plaza::Utils::ReadBinaryString(binaryFile) };

			binaryFile.close();

			return asset;
		}
	};
}