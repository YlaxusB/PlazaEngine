#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>

namespace Plaza {
	enum AssetExtension {
		OBJ,
		FBX,
		GLTF,
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
		static inline glm::vec3 mModelImporterScale = glm::vec3(1.0f);
		static void ImportAsset(std::string path);
		static void ImportModel(AssetImported asset);
		static Entity* ImportOBJ(AssetImported asset, std::filesystem::path outPath);
		static Entity* ImportFBX(AssetImported asset, std::filesystem::path outPath);
		static Entity* ImportGLTF(AssetImported asset, std::filesystem::path outPath);
		static void ImportTexture(AssetImported asset);
	private:
		static inline const std::unordered_map<std::string, AssetExtension> mExtensionMapping = {
			{".obj", OBJ},
			{".fbx", FBX},
			{".gltf", GLTF},
			{".png", PNG},
			{".jpg", JPG},
			{".jpeg", JPEG}
		};
	};
}