#pragma once

namespace Plaza {

	enum AssetType {
		NONE,
		MODEL,
		MATERIAL,
		TEXTURE,
		SCENE,
		SCRIPT,
		DLL,
		MESH,
		UNKNOWN,
	};
	struct Asset {
		uint64_t mAssetUuid = 0;
		std::string mAssetExtension = "";
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
		static inline std::map<std::filesystem::path, uint64_t> mAssetsUuidByPath = std::map<std::filesystem::path, uint64_t>();
		static inline std::map<std::string, AssetType> mAssetTypeByExtension = std::map<std::string, AssetType>();
		static inline std::unordered_map<AssetType, std::unordered_set<uint64_t>> mTypeMap = std::unordered_map<AssetType, std::unordered_set<uint64_t>>();

		static inline AssetsModelListStructure mMemoryModels = AssetsModelListStructure();

		static void Init() {
			mTypeMap = []() {
				std::unordered_map<AssetType, std::unordered_set<uint64_t>> map;
				for (int i = 0; i <= UNKNOWN; ++i) {
					map.emplace(static_cast<AssetType>(i), std::unordered_set<uint64_t>());
				}
				return map;
				}();

				AssetsManager::mAssetTypeByExtension.emplace(Standards::modelExtName, AssetType::MODEL);
				AssetsManager::mAssetTypeByExtension.emplace(Standards::materialExtName, AssetType::MATERIAL);
				AssetsManager::mAssetTypeByExtension.emplace(".png", AssetType::TEXTURE);
				AssetsManager::mAssetTypeByExtension.emplace(".jpg", AssetType::TEXTURE);
				AssetsManager::mAssetTypeByExtension.emplace(".jpeg", AssetType::TEXTURE);
				AssetsManager::mAssetTypeByExtension.emplace(Standards::sceneExtName, AssetType::SCENE);
				AssetsManager::mAssetTypeByExtension.emplace("", AssetType::NONE);
		}

		static Asset* NewAsset(uint64_t uuid, AssetType assetType, std::string path) {
			Asset* newAsset = new Asset();
			newAsset->mAssetUuid = uuid;
			newAsset->mPath = std::filesystem::path{ path };
			newAsset->mAssetExtension = newAsset->mPath.extension().string();
			AssetsManager::AddAsset(newAsset);
			return newAsset;
		}
		static Asset* NewAsset(AssetType assetType, std::filesystem::path path) {
			Asset* newAsset = new Asset();
			newAsset->mAssetUuid = Plaza::UUID::NewUUID();
			newAsset->mAssetExtension = path.extension().string();
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
			AssetsManager::AddAsset(newAsset);
			return newAsset;
		}

		static Asset* GetAsset(uint64_t uuid) {
			const auto& it = mAssets.find(uuid);
			if (it != mAssets.end())
				return mAssets.at(uuid);
			return nullptr;
		}

		static Asset* GetAsset(std::filesystem::path path) {
			const auto& it = AssetsManager::mAssetsUuidByPath.find(path);
			if (it != mAssetsUuidByPath.end())
				return GetAsset(it->second);
			return nullptr;
		}

		static void AddAsset(Asset* asset) {
			AssetsManager::mAssets.emplace(asset->mAssetUuid, asset);
			AssetsManager::mTypeMap.find(mAssetTypeByExtension.at(asset->mAssetExtension))->second.emplace(asset->mAssetUuid);
			AssetsManager::mAssetsUuidByPath.emplace(asset->mPath, asset->mAssetUuid);
		}
		static void AddModel(Model* model) {
			AssetsManager::mMemoryModels.push_back(model);
		}

		static Asset* LoadFileAsAsset(std::filesystem::path path) {
			std::ifstream stream(path);
			std::stringstream strStream;
			strStream << stream.rdbuf();

			YAML::Node data = YAML::Load(strStream.str());
			if (!data || !data["AssetUuid"]) {
				std::cout << "File is empty!" << std::endl;
				return new Asset();
			}

			return AssetsManager::NewAsset(data["AssetUuid"].as<uint64_t>(), AssetsManager::mAssetTypeByExtension.at(path.extension().string()), path.string());
		}

		static Asset* LoadMetadataAsAsset(std::filesystem::path path) {
			std::ifstream binaryFile(path, std::ios::binary);
			if (!binaryFile.is_open()) {
				std::cerr << "Error opening file for writing!" << std::endl;
				return nullptr;
			}

			uint64_t uuid = 0;
			binaryFile.read(reinterpret_cast<char*>(&uuid), sizeof(uint64_t));
			std::string contentPath = Plaza::Utils::ReadBinaryString(binaryFile);
			std::string extension = Plaza::Utils::ReadBinaryString(binaryFile);

			binaryFile.close();

			return AssetsManager::NewAsset(uuid, AssetsManager::mAssetTypeByExtension.at(extension), contentPath);
		}

		static Asset* LoadBinaryFileAsAsset(std::filesystem::path path) {
			std::ifstream binaryFile(path, std::ios::binary);
			if (!binaryFile.is_open()) {
				std::cerr << "Error opening file for writing!" << std::endl;
				return nullptr;
			}

			uint64_t uuid = 0;
			binaryFile.read(reinterpret_cast<char*>(&uuid), sizeof(uint64_t));

			binaryFile.close();

			return AssetsManager::NewAsset(uuid, AssetsManager::mAssetTypeByExtension.at(path.extension().string()), path.string());
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

		static void RemoveAssetUuidPath(uint64_t assetUuid) {
			const auto& it = AssetsManager::mAssetsUuidByPath.find(AssetsManager::GetAsset(assetUuid)->mPath);
			if (it != AssetsManager::mAssetsUuidByPath.end())
				AssetsManager::mAssetsUuidByPath.erase(it);
		}

		static void ChangeAssetPath(uint64_t assetUuid, std::string newPath) {
			AssetsManager::RemoveAssetUuidPath(assetUuid);
			AssetsManager::GetAsset(assetUuid)->mPath = newPath;
			AssetsManager::mAssetsUuidByPath.emplace(std::filesystem::path{ newPath }, assetUuid);
		}

		static Asset* GetAssetOrImport(std::string path);
	};
}