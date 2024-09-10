#pragma once
#include <filesystem>
#include <string>

namespace Plaza {
	struct Asset {
	public:
		Asset(uint64_t assetUuid = 0, std::string assetName = "", std::filesystem::path assetPath = "") : mAssetUuid(assetUuid), mAssetName(assetName), mAssetPath(assetPath) {
			mAssetUuid == 0 ? Plaza::UUID::NewUUID() : mAssetUuid;
			mAssetExtension = assetPath.extension().string();
		}
		uint64_t mAssetUuid = 0;
		std::string mAssetExtension = "";
		std::filesystem::path mAssetPath;
		std::string mAssetName = "";
	};
}