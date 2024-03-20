#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsImporter.h"
#include "Engine/Core/AssetsManager/Serializer/AssetsSerializer.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
#include "Editor/GUI/Utils/Filesystem.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"

namespace Plaza {
	void AssetsImporter::ImportAsset(std::string path) {
		std::filesystem::path filePath = std::filesystem::path{ path };
		if (!std::filesystem::exists(filePath))
			return;

		std::string extension = filePath.extension().string();
		AssetImported asset = AssetImported({ AssetsImporter::mExtensionMapping.at(extension), path });

		std::string outDirectory = Editor::Gui::FileExplorer::currentDirectory;
		std::string outPath = outDirectory + "\\" + Editor::Utils::Filesystem::GetUnrepeatedName(std::filesystem::path{ path }.filename().string());

		switch (asset.mExtension) {
		case AssetExtension::OBJ:
			Entity* mainEntity = AssetsImporter::ImportOBJ(asset, std::filesystem::path{});
			AssetsSerializer::SerializePrefab(mainEntity, std::filesystem::path{outPath});
			Application->activeScene->RemoveEntity(mainEntity->uuid);
			//Application->activeScene->GetEntity(mainEntity->uuid)->~Entity();
			AssetsLoader::LoadPrefab(outPath);
			break;
		//case AssetExtension::FBX:
	//		AssetsImporter::ImportFBX(asset);
	//		break;
		//case AssetExtension::PNG:
	//		AssetsImporter::ImportTexture(asset);
	//		break;
		//case AssetExtension::JPEG:
	//		AssetsImporter::ImportTexture(asset);
	//		break;
		//case AssetExtension::JPG:
	//		AssetsImporter::ImportTexture(asset);
	//		break;
		}

		
	}

	void AssetsImporter::ImportModel(AssetImported asset) {

	}

	void AssetsImporter::ImportFBX(AssetImported asset) {

	}

	void AssetsImporter::ImportTexture(AssetImported asset) {

	}
}