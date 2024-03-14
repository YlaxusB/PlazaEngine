#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsImporter.h"

namespace Plaza {
	void AssetsImporter::ImportAsset(std::string path) {
		std::filesystem::path filePath = std::filesystem::path{ path };
		if (!std::filesystem::exists(filePath))
			return;

		std::string extension = filePath.extension().string();
		AssetImported asset = AssetImported({ AssetsImporter::mExtensionMapping.at(extension), path });
		switch (asset.mExtension) {
		case AssetExtension::OBJ:
			AssetsImporter::ImportOBJ(asset);
			break;
		case AssetExtension::FBX:
			AssetsImporter::ImportFBX(asset);
			break;
		case AssetExtension::PNG:
			AssetsImporter::ImportTexture(asset);
			break;
		case AssetExtension::JPEG:
			AssetsImporter::ImportTexture(asset);
			break;
		case AssetExtension::JPG:
			AssetsImporter::ImportTexture(asset);
			break;
		}
	}

	void AssetsImporter::ImportModel(AssetImported asset) {

	}

	void AssetsImporter::ImportOBJ(AssetImported asset) {

	}

	void AssetsImporter::ImportFBX(AssetImported asset) {

	}

	void AssetsImporter::ImportTexture(AssetImported asset) {

	}
}