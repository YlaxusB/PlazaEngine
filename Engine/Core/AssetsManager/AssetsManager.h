#pragma once

namespace Plaza {
	enum AssetType {
		NONE,
		MODEL,
		TEXTURE,
		SCENE,
		SCRIPT,
		UNKNOWN
	};
	struct Asset {
		uint64_t mAssetUuid = 0;
		AssetType mAssetType = AssetType::NONE;
		std::filesystem::path mPath;
	};

	class AssetsListStructure : public std::unordered_map<uint64_t, Asset*> {
	public:

	};

	class AssetsModelListStructure : public std::vector<Model*> {

	};



	class AssetsManager {
	public:
		static inline AssetsListStructure mAssets = AssetsListStructure();
		static inline std::unordered_map<AssetType, std::unordered_set<uint64_t>> mTypeMap = []() {
			std::unordered_map<AssetType, std::unordered_set<uint64_t>> map;
			for (int i = 0; i <= UNKNOWN; ++i) {
				map.emplace(static_cast<AssetType>(i), std::unordered_set<uint64_t>());
			}
			return map;
			}();
		static inline AssetsModelListStructure mMemoryModels = AssetsModelListStructure();

		static Asset* NewAsset(uint64_t uuid, AssetType assetType, std::string path) {
			Asset* newAsset = new Asset();
			newAsset->mAssetUuid = uuid;
			newAsset->mAssetType = assetType;
			newAsset->mPath = std::filesystem::path{ path };
			AssetsManager::AddAsset(newAsset);
			return newAsset;
		}
		static Asset* NewAsset(AssetType assetType, std::filesystem::path path) {
			Asset* newAsset = new Asset();
			newAsset->mAssetUuid = Plaza::UUID::NewUUID();
			newAsset->mAssetType = assetType;
			newAsset->mPath = path;
			AssetsManager::AddAsset(newAsset);
			return newAsset;
		}
		static Asset* NewAsset(AssetType assetType, std::string path) {
			return NewAsset(assetType, std::filesystem::path{ path });
		}

		static Asset* NewAsset() {
			Asset* newAsset = new Asset();
			newAsset->mAssetUuid = Plaza::UUID::NewUUID();
			newAsset->mAssetType = AssetType::NONE;
			AssetsManager::AddAsset(newAsset);
			return newAsset;
		}

		static Asset* GetAsset(uint64_t uuid) {
			const auto& it = mAssets.find(uuid);
			if (it != mAssets.end())
				return mAssets.at(uuid);
			return new Asset();
		}

		static void AddAsset(Asset* asset) {
			AssetsManager::mAssets.emplace(asset->mAssetUuid, asset);
			AssetsManager::mTypeMap.find(asset->mAssetType)->second.emplace(asset->mAssetUuid);
		}
		static void AddModel(Model* model) {
			AssetsManager::mMemoryModels.push_back(model);
		}

		static uint64_t CheckAssetPath(YAML::Node& data) {
			if (data["AssetUuid"])
				return data["AssetUuid"].as<uint64_t>();
			else
				return 0;
		}
		static uint64_t CheckAssetPath(std::filesystem::path path) {
			std::ifstream stream(path);
			std::stringstream strStream;
			strStream << stream.rdbuf();

			YAML::Node data = YAML::Load(strStream.str());
			if (!data) {
				std::cout << "File is empty!" << std::endl;
				return 0;
			}

			return CheckAssetPath(data);
		}
	};
}