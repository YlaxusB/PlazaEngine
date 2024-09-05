#pragma once
#include <filesystem>
#include <string>

namespace Plaza {
	struct Asset {
	public:
		uint64_t mAssetUuid = 0;
		std::string mAssetExtension = "";
		std::filesystem::path mAssetPath;
		std::string mAssetName = "";
	};
}