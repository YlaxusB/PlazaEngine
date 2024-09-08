#include "AssetsSerializer.h"

namespace Plaza {
	void AssetsSerializer::SerializeMesh(Mesh* mesh, std::filesystem::path outPath) {
		std::ofstream os(outPath.string(), std::ios::binary);
		cereal::BinaryOutputArchive archive(os);
		archive(*mesh);
		os.close();
	}
}