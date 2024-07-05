#include "AssetsLoader.h"

namespace Plaza {
	Animation& AssetsLoader::LoadAnimation(Asset* asset, Scene* scene) {
		std::ifstream is(asset->mPath, std::ios::binary);
		cereal::BinaryInputArchive archive(is);
		Animation animation;
		archive(animation);
		is.close();

		return AssetsManager::AddAnimation(animation);
	}
}