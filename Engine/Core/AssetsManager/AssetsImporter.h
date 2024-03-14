#pragma once

namespace Plaza {
	enum AssetExtension {
		OBJ,
		FBX,
		PNG,
		JPG,
		JPEG
	};
	struct AssetImported {
		AssetExtension mExtension;
		std::string mPath;
	};

	class AssetsImporter {
	public:
		void ImportAsset(std::string path);
		void ImportModel(AssetImported asset);
		void ImportOBJ(AssetImported asset);
		void ImportFBX(AssetImported asset);
		void ImportTexture(AssetImported asset);
	private:
		const std::unordered_map<std::string, AssetExtension> mExtensionMapping = {
			{"obj", OBJ},
			{"fbx", FBX},
			{"png", PNG},
			{"jpg", JPG},
			{"jpeg", JPEG}
		};
	};
}