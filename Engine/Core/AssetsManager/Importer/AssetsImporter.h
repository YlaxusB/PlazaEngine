#pragma once
//#include "ThirdParty/OpenFBX/src/ofbx.h"
//#define UFBX_STATIC
#include "ThirdParty/ufbx/ufbx/ufbx.h"
//#include <include/ufbx.c>
#include "ThirdParty/tinyobjloader/tiny_obj_loader.h"
#include "Engine/Components/Rendering/Material.h"
#include "Engine/Components/Rendering/AnimationComponent.h"

namespace Plaza {
	enum AssetExtension {
		OBJ,
		FBX,
		GLTF,
		PNG,
		JPG,
		JPEG,
		DDS,
		TGA,
		PSD
	};
	struct AssetImported {
		std::string mExtension;
		std::string mPath;
	};
	struct AssetsImporterSettings {
		std::string outDirectory = "";
		bool mImportModel = true;
		bool mImportTextures = true;
		bool mImportMaterials = true;
		bool mImportAnimations = true;
		glm::vec2 mFlipTextures = glm::vec2(1.0f, 1.0f);
	};

	class AssetsImporter {
	public:
		static inline glm::vec3 mModelImporterScale = glm::vec3(1.0f);//glm::vec3(0.01f);
		static std::string ImportAsset(std::string path, uint64_t uuid = 0, AssetsImporterSettings settings = AssetsImporterSettings{});
		static void ImportModel(AssetImported asset);
		static Entity* ImportOBJ(AssetImported asset, std::filesystem::path outPath, AssetsImporterSettings settings = AssetsImporterSettings{});
		static Entity* ImportFBX(AssetImported asset, std::filesystem::path outPath, AssetsImporterSettings settings = AssetsImporterSettings{});
		static Entity* ImportGLTF(AssetImported asset, std::filesystem::path outPath, AssetsImporterSettings settings = AssetsImporterSettings{});
		static std::string ImportTexture(AssetImported asset, uint64_t uuid = 0);
		static void ImportAnimation(std::filesystem::path filePath, std::filesystem::path outFolder, AssetsImporterSettings settings = AssetsImporterSettings{});
		static std::vector<Animation> ImportAnimationFBX(std::string filePath, AssetsImporterSettings settings = AssetsImporterSettings{});
	private:
		static inline const std::unordered_map<std::string, AssetExtension> mExtensionMapping = {
			{".obj", OBJ},
			{".fbx", FBX},
			{".gltf", GLTF},
			{".png", PNG},
			{".jpg", JPG},
			{".jpeg", JPEG},
			{".dds", DDS},
			{".tga", TGA},
			{".psd", PSD}
		};

		static Material* FbxModelMaterialLoader(const ufbx_material* ufbxMaterial, const std::string materialFolderPath, std::unordered_map<std::string, uint64_t>& loadedTextures);
		static Material* ObjModelMaterialLoader(const tinyobj::material_t* tinyobjMaterial, const std::string materialFolderPath, std::unordered_map<std::string, uint64_t>& loadedTextures);
	};
}