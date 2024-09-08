#include "AssetsLoader.h"

namespace Plaza {
	Material* AssetsLoader::LoadMaterial(Asset* asset, Scene* scene) {
		std::ifstream is(asset->mAssetPath, std::ios::binary);
		cereal::BinaryInputArchive archive(is);
		Material deserializedMaterial;
		archive(deserializedMaterial);
		is.close();

		Material* material = new Material(deserializedMaterial);
		if (!scene)
			Application::Get()->activeScene->AddMaterial(material);
		else
			scene->AddMaterial(material);
		return material;
	}
}