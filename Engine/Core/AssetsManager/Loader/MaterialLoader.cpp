#include "AssetsLoader.h"

namespace Plaza {
	Material* AssetsLoader::LoadMaterial(Asset* asset, Scene* scene) {
		Material* material = new Material();

		std::ifstream file(asset->mPath, std::ios::binary);
		if (!file.is_open()) {
			std::cerr << "Error: Failed to open file for reading: " << asset->mPath.string() << std::endl;
			return nullptr;
		}

		file.read(reinterpret_cast<char*>(&material->mAssetUuid), sizeof(uint64_t));
		material->uuid = material->mAssetUuid;
		asset->mAssetUuid = material->mAssetUuid;
		material->name = Plaza::Utils::ReadBinaryString(file);
		file.read(reinterpret_cast<char*>(&material->diffuse->rgba), sizeof(material->diffuse->rgba));
		file.read(reinterpret_cast<char*>(&material->diffuse->mAssetUuid), sizeof(material->diffuse->mAssetUuid));
		file.read(reinterpret_cast<char*>(&material->normal->mAssetUuid), sizeof(material->normal->mAssetUuid));
		file.read(reinterpret_cast<char*>(&material->roughness->mAssetUuid), sizeof(material->roughness->mAssetUuid));
		file.read(reinterpret_cast<char*>(&material->roughnessFloat), sizeof(material->roughnessFloat));
		file.read(reinterpret_cast<char*>(&material->metalness->mAssetUuid), sizeof(material->metalness->mAssetUuid));
		file.read(reinterpret_cast<char*>(&material->metalnessFloat), sizeof(material->metalnessFloat));
		file.read(reinterpret_cast<char*>(&material->intensity), sizeof(material->intensity));
		file.read(reinterpret_cast<char*>(&material->shininess), sizeof(material->shininess));

		file.close();

		if (!scene)
			Application->activeScene->AddMaterial(material);
		else
			scene->AddMaterial(material);
		return material;
	}
}