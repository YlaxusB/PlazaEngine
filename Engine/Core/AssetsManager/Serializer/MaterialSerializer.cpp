#include "AssetsSerializer.h"

namespace Plaza {
	void AssetsSerializer::SerializeMaterial(Material* material, std::filesystem::path outPath) {
		std::ofstream os(outPath.string(), std::ios::binary);
		cereal::BinaryOutputArchive archive(os);
		archive(*material);
		os.close();
	}
}