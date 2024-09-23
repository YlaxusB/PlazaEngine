#include "AssetsLoader.h"

namespace Plaza {
	Material* AssetsLoader::LoadMaterial(Asset* asset, SerializationMode serializationMode) {
		Material* material = new Material(*AssetsSerializer::DeSerializeFile<Material>(asset->mAssetPath.string(), serializationMode).get());
		AssetsManager::AddMaterial(material);
		return material;
	}
}