#include "AssetsSerializer.h"

namespace Plaza {
	void AssetsSerializer::SerializeAnimation(Animation& animation, std::filesystem::path outPath) {
		std::ofstream os(outPath.string(), std::ios::binary);
		cereal::BinaryOutputArchive archive(os);
		archive(animation);
		os.close();
	}
}

