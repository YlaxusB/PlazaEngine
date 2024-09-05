#include "AssetsSerializer.h"

namespace Plaza {
	void AssetsSerializer::SerializeMaterial(Material* material, std::filesystem::path outPath) {
		std::ofstream file(outPath.string(), std::ios::binary);
		if (!file.is_open()) {
			std::cerr << "Error: Failed to open file for writing: " << outPath.string() << std::endl;
			return;
		}
		 
		file.write(reinterpret_cast<const char*>(&material->mAssetUuid), sizeof(uint64_t));
		Plaza::Utils::SaveStringAsBinary(file, material->mAssetName);
		file.write(reinterpret_cast<const char*>(&material->diffuse->rgba), sizeof(material->diffuse->rgba));
		file.write(reinterpret_cast<const char*>(&material->diffuse->mAssetUuid), sizeof(material->diffuse->mAssetUuid));
		file.write(reinterpret_cast<const char*>(&material->normal->mAssetUuid), sizeof(material->normal->mAssetUuid));
		file.write(reinterpret_cast<const char*>(&material->roughness->mAssetUuid), sizeof(material->roughness->mAssetUuid));
//		file.write(reinterpret_cast<const char*>(&material->roughnessFloat), sizeof(material->roughnessFloat));
		file.write(reinterpret_cast<const char*>(&material->metalness->mAssetUuid), sizeof(material->metalness->mAssetUuid));
//		file.write(reinterpret_cast<const char*>(&material->metalnessFloat), sizeof(material->metalnessFloat));
//		file.write(reinterpret_cast<const char*>(&material->intensity), sizeof(material->intensity));
//		file.write(reinterpret_cast<const char*>(&material->shininess), sizeof(material->shininess));

		file.close();
	}
}