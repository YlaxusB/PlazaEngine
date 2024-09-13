#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsImporter.h"
#include "Engine/Core/AssetsManager/Serializer/AssetsSerializer.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
#include "Editor/GUI/Utils/Filesystem.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"
#include "Engine/Core/AssetsManager/Metadata/Metadata.h"

namespace Plaza {
	std::string AssetsImporter::ImportAsset(std::string path, uint64_t uuid, AssetsImporterSettings settings) {
		std::filesystem::path filePath = std::filesystem::path{ path };
		if (!std::filesystem::exists(filePath))
			return "";

		std::string extension = filePath.extension().string();

		if (AssetsImporter::mExtensionMapping.find(extension) == AssetsImporter::mExtensionMapping.end())
			return "";

		AssetImported asset = AssetImported({ extension, path });

		std::string outDirectory = settings.outDirectory.empty() ? Editor::Gui::FileExplorer::currentDirectory : settings.outDirectory;
		std::string outPath = outDirectory + "\\" + Editor::Utils::Filesystem::GetUnrepeatedName(std::filesystem::path{ path }.stem().string());

		Entity* mainEntity;
		switch (AssetsImporter::mExtensionMapping.at(extension)) {
		case AssetExtension::OBJ:
			if (settings.mImportModel) {
				mainEntity = AssetsImporter::ImportOBJ(asset, std::filesystem::path{});
				SerializablePrefab prefab = AssetsSerializer::SerializePrefab(mainEntity, std::filesystem::path{ outPath + Standards::modelExtName });
				Scene::GetActiveScene()->RemoveEntity(mainEntity->uuid);
				Asset* asset = AssetsManager::NewAsset<Asset>(prefab.assetUuid, outPath + Standards::modelExtName);
				AssetsLoader::LoadAsset(asset);
			}
			break;
		case AssetExtension::FBX:
			if (settings.mImportModel) {
				mainEntity = AssetsImporter::ImportFBX(asset, std::filesystem::path{});
				SerializablePrefab prefab = AssetsSerializer::SerializePrefab(mainEntity, std::filesystem::path{ outPath + Standards::modelExtName });
				Scene::GetActiveScene()->RemoveEntity(mainEntity->uuid);
				Asset* asset = AssetsManager::NewAsset<Asset>(prefab.assetUuid, outPath + Standards::modelExtName);
				AssetsLoader::LoadAsset(asset);
			}
			if (settings.mImportAnimations)
				ImportAnimation(path, outDirectory);
			//AssetsLoader::LoadPrefab(AssetsManager::NewAsset(AssetType::MODEL, outPath + Standards::modelExtName));
			break;
		case AssetExtension::PNG:
			return AssetsImporter::ImportTexture(asset, uuid);
			break;
		case AssetExtension::JPEG:
			return AssetsImporter::ImportTexture(asset, uuid);
			break;
		case AssetExtension::JPG:
			return AssetsImporter::ImportTexture(asset, uuid);
			break;
		case AssetExtension::DDS:
			return AssetsImporter::ImportTexture(asset, uuid);
			break;
		case AssetExtension::TGA:
			return AssetsImporter::ImportTexture(asset, uuid);
			break;
		case AssetExtension::PSD:
			return AssetsImporter::ImportTexture(asset, uuid);
			break;
		}

		return outPath;
	}

	void AssetsImporter::ImportModel(AssetImported asset) {

	}

	void AssetsImporter::ImportAnimation(std::filesystem::path filePath, std::filesystem::path outFolder, AssetsImporterSettings settings) {
		if (!std::filesystem::exists(filePath))
			return;

		std::string extension = filePath.extension().string();
		if (AssetsImporter::mExtensionMapping.find(extension) == AssetsImporter::mExtensionMapping.end())
			return;

		AssetImported asset = AssetImported({ extension, filePath.string() });

		std::vector<Animation> loadedAnimations = std::vector<Animation>();

		switch (AssetsImporter::mExtensionMapping.at(extension)) {
		case AssetExtension::FBX:
			loadedAnimations = AssetsImporter::ImportAnimationFBX(asset.mPath);
			break;
		}

		for (Animation& animation : loadedAnimations) {
			std::string animationOutPath = Editor::Utils::Filesystem::GetUnrepeatedPath(outFolder.string() + "\\" + animation.mAssetName + Standards::animationExtName);
			AssetsSerializer::SerializeAnimation(animation, animationOutPath);
			AssetsManager::NewAsset<Animation>(AssetType::ANIMATION, animationOutPath);
			AssetsManager::AddAnimation(animation);
		}
	}

	std::string AssetsImporter::ImportTexture(AssetImported importedAsset, uint64_t uuid) {
		std::string outPath = Editor::Gui::FileExplorer::currentDirectory + "\\" + std::filesystem::path{ importedAsset.mPath }.filename().string();
		outPath = Editor::Utils::Filesystem::GetUnrepeatedPath(outPath);

		std::filesystem::copy(importedAsset.mPath, outPath);

		Asset asset{ uuid <= 0 ? Plaza::UUID::NewUUID() : uuid, std::filesystem::path{ importedAsset.mPath }.filename().string() , outPath };
		Metadata::CreateMetadataFile(&asset);
		AssetsManager::AddAsset(new Asset(asset));

		return outPath;
	}
}