#pragma once
using namespace std;
#include "Engine/Vendor/assimp/scene.h"
namespace Plaza {
	class ModelLoader
	{
	public:
		static double modelScale;
		static vector<string> supportedFormats;

		static Entity* LoadImportedModelToScene(uint64_t modelUuid, string filePath = "");
		static Entity* LoadImportedModelToMemory(string filePath);
		static Entity* LoadModelToGame(string const& path, std::string modelName);
		static Entity* LoadModelToGame(string const& path, std::string modelName, aiScene const* scene);
		static void LoadModelMeshes(string filePath, unordered_map<uint64_t, shared_ptr<MeshRenderer>>& meshRenderers, Model* model);
		static void ProcessNode(aiNode* node, const aiScene* scene, vector<Mesh>& meshes, vector<Texture>& textures_loaded, string* directory, Entity* modelMainObject, unsigned int& index);
		static Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, vector<Texture>& textures_loaded, string* directory, aiNode* node);

		static vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, vector<Texture>& textures_loaded, string* directory);
		static unsigned int TextureFromFile(const char* path, const string& directory, bool gamma);
		static bool CheckFlippedTextures(const std::string& modelPath);
	};
	inline vector<string> ModelLoader::supportedFormats = { ".obj", ".fbx" };
	inline double ModelLoader::modelScale = 0.01;
}