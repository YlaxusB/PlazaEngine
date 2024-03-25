#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsImporter.h"
#include "Engine/Core/AssetsManager/Serializer/AssetsSerializer.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
#include "Editor/GUI/Utils/Filesystem.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"
#include "Engine/Core/AssetsManager/Metadata/Metadata.h"

namespace Plaza {
	std::string AssetsImporter::ImportAsset(std::string path) {
		std::filesystem::path filePath = std::filesystem::path{ path };
		if (!std::filesystem::exists(filePath))
			return "";

		std::string extension = filePath.extension().string();

		if (AssetsImporter::mExtensionMapping.find(extension) == AssetsImporter::mExtensionMapping.end())
			return "";


		AssetImported asset = AssetImported({ extension, path });

		std::string outDirectory = Editor::Gui::FileExplorer::currentDirectory;
		std::string outPath = outDirectory + "\\" + Editor::Utils::Filesystem::GetUnrepeatedName(std::filesystem::path{ path }.filename().string());

		Entity* mainEntity;
		switch (AssetsImporter::mExtensionMapping.at(extension)) {
		case AssetExtension::OBJ:
			mainEntity = AssetsImporter::ImportOBJ(asset, std::filesystem::path{});
			AssetsSerializer::SerializePrefab(mainEntity, std::filesystem::path{ outPath });
			Application->activeScene->RemoveEntity(mainEntity->uuid);
			AssetsLoader::LoadPrefab(outPath);
			break;
		case AssetExtension::FBX:
			mainEntity = AssetsImporter::ImportFBX(asset, std::filesystem::path{});
			AssetsSerializer::SerializePrefab(mainEntity, std::filesystem::path{ outPath });
			Application->activeScene->RemoveEntity(mainEntity->uuid);
			AssetsLoader::LoadPrefab(outPath);
			break;
		case AssetExtension::PNG:
			return AssetsImporter::ImportTexture(asset);
			break;
		case AssetExtension::JPEG:
			return AssetsImporter::ImportTexture(asset);
			break;
		case AssetExtension::JPG:
			return AssetsImporter::ImportTexture(asset);
			break;
		}

		return outPath;
	}

	void AssetsImporter::ImportModel(AssetImported asset) {

	}

	std::string AssetsImporter::ImportTexture(AssetImported importedAsset) {
		std::string outPath = Editor::Gui::FileExplorer::currentDirectory + "\\" + std::filesystem::path{importedAsset.mPath}.filename().string();
		outPath = Editor::Utils::Filesystem::GetUnrepeatedPath(outPath);

		std::filesystem::copy(importedAsset.mPath, outPath);

		Asset asset{ Plaza::UUID::NewUUID(), importedAsset.mExtension, outPath };
		Metadata::CreateMetadataFile(&asset);
		AssetsManager::AddAsset(new Asset(asset));

		return outPath;
	}
}