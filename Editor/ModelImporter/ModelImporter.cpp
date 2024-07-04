#include "Engine/Core/PreCompiledHeaders.h"
#include "ModelImporter.h"
#include "Engine/Core/ModelLoader/ModelLoader.h"
#include "Engine/Core/ModelLoader/Model.h"
#include "Engine/Core/Engine.h"
#include "Editor/GUI/FileExplorer/CreateFolder.h"
#include "Engine/Application/Serializer/ModelSerializer.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
#include "ThirdParty/assimp/include/assimp/Importer.hpp"
#include "ThirdParty/assimp/include/assimp/scene.h"
#include "ThirdParty/assimp/include/assimp/postprocess.h"
#include "Engine/Application/Serializer/FileSerializer/FileSerializer.h"
namespace Plaza::Editor {
	void DeleteChildrene(Entity* entity) {
		for (uint64_t child : entity->childrenUuid) {
			DeleteChildrene(&Application->activeScene->entities[child]);
		}
		delete(entity);
	}

	unordered_set<uint64_t> serializedMaterials = unordered_set<uint64_t>();

	void SerializeEntityAndChildsMaterials(Entity* entity) {
		for (uint64_t child : entity->childrenUuid) {
			SerializeEntityAndChildsMaterials(&Application->activeScene->entities.at(child));
		}
		if (entity->HasComponent<MeshRenderer>() && entity->GetComponent<MeshRenderer>()->mMaterials.size() > 0) {
			//if (serializedMaterials.find(entity->GetComponent<MeshRenderer>()->material->uuid) == serializedMaterials.end()) {
			//	MaterialFileSerializer::Serialize(entity->GetComponent<MeshRenderer>()->material->filePath, entity->GetComponent<MeshRenderer>()->material);
			//	serializedMaterials.emplace(entity->GetComponent<MeshRenderer>()->material->uuid);
			//}
		}
	}


	vector<aiTextureType> types = { aiTextureType_DIFFUSE, aiTextureType_HEIGHT, aiTextureType_AMBIENT, aiTextureType_NORMALS, aiTextureType_SPECULAR };
	void ModelImporter::ImportModel(string const& path, std::string modelName, std::string extension, std::string modelPath) {
		string modelPathInEngine = Editor::CreateFolder(path, modelName);
		string sourcesFolderPath = Editor::CreateFolder(modelPathInEngine, "sources");
		string texturesFolderPath = Editor::CreateFolder(modelPathInEngine, "textures");
		string texturesOriginalPath = std::filesystem::path(modelPath).parent_path().string();
		string isFbx = std::filesystem::path(modelPath).filename().extension() == ".fbx" ? "Textures\\" : "";

		vector<string> loadedTextures;
		Assimp::Importer importer;
		importer.SetPropertyBool(AI_CONFIG_IMPORT_OGRE_MATERIAL_FILE, true);
		//importer.SetPropertyString(AI_CONFIG_IMPORT_MTL, "path_to_your_mtl_directory/");
		const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		aiNode* node = scene->mRootNode;
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[i];
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture> textures;
			for (aiTextureType type : types) {
				for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
				{
					aiString str;
					material->GetTexture(type, i, &str);
					string fileName = std::filesystem::path(str.C_Str()).filename().string();

					string directory = filesystem::path{ modelPath }.parent_path().string() + "\\textures";
					vector<Texture*> textures_loaded = vector<Texture*>();
					aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
					// 1. diffuse maps
					vector<Texture*> diffuseMaps = ModelLoader::LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", textures_loaded, &directory);
					//  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
					// 2. specular maps
					vector<Texture*> specularMaps = ModelLoader::LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", textures_loaded, &directory);
					//  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
					// 3. normal maps
					std::vector<Texture*> normalMaps = ModelLoader::LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", textures_loaded, &directory);
					//  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
					// 4. height maps
					std::vector<Texture*> heightMaps = ModelLoader::LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", textures_loaded, &directory);
					//  textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

					// return a mesh object created from the extracted mesh data
					Material convertedMaterial = *new Material();
					if (diffuseMaps.size() > 0 && diffuseMaps[0]->GetTextureID() != 0)
						convertedMaterial.diffuse = diffuseMaps[0];

					//if (specularMaps.size() > 0)
					//	convertedMaterial.specular = specularMaps[0];

					if (normalMaps.size() > 0)
						convertedMaterial.normal = normalMaps[0];

					if (heightMaps.size() > 0)
						convertedMaterial.height = heightMaps[0];

					if (!convertedMaterial.diffuse->path.empty())
						convertedMaterial.diffuse->path = std::filesystem::path(convertedMaterial.diffuse->path).filename().string();
					//if (!convertedMaterial.specular->path.empty())
					//	convertedMaterial.specular->path = std::filesystem::path(convertedMaterial.specular->path).filename().string();
					if (!convertedMaterial.normal->path.empty())
						convertedMaterial.normal->path = std::filesystem::path(convertedMaterial.normal->path).filename().string();
					if (!convertedMaterial.height->path.empty())
						convertedMaterial.height->path = std::filesystem::path(convertedMaterial.height->path).filename().string();


					convertedMaterial.name = filesystem::path{ modelPath }.stem().string() + material->GetName().C_Str() + "_" + std::to_string(i) + Standards::materialExtName;
					convertedMaterial.filePath = texturesFolderPath + "\\" + convertedMaterial.name;
					//Application->activeScene->materials.emplace(convertedMaterial.uuid, &convertedMaterial);
					//Application->activeScene->AddMaterial(&convertedMaterial);
					//MaterialFileSerializer::Serialize(convertedMaterial.filePath, &convertedMaterial);

				}
			}
		}
		for (string texturePath : loadedTextures) {
			//std::filesystem::copy(texturesOriginalPath + "\\" + isFbx + texturePath, texturesFolderPath + "\\" + texturePath);
		}
		std::filesystem::copy(modelPath, sourcesFolderPath + "\\" + modelName + extension);
		Entity* mainObject = ModelLoader::LoadModelToGame(modelPath, modelName, scene, true, path);

		/* Serialize all the materials */
		SerializeEntityAndChildsMaterials(mainObject);
		serializedMaterials.clear();

		std::string serializePath = modelPathInEngine + "\\" + filesystem::path(modelPath).stem().string() + Standards::modelExtName;
		ModelSerializer::SerializeModel(mainObject, AssetsManager::NewAsset(AssetType::MODEL, serializePath), sourcesFolderPath + "\\" + modelName + extension);
		Editor::selectedGameObject = nullptr;
		Gui::changeSelectedGameObject(nullptr);
		//mainObject->Delete();
		//delete(mainObject);
		//DeleteChildrene(mainObject);
		Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
	}
}
