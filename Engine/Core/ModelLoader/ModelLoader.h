#pragma once
using namespace std;
#include "Engine/Vendor/assimp/scene.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
namespace Plaza {
	class ModelLoader
	{
	public:
		static double modelScale;
		static vector<string> supportedFormats;

		static Entity* LoadImportedModelToScene(uint64_t modelUuid, string filePath = "");
		static uint64_t LoadImportedModelToMemory(string filePath, std::map<std::string, uint64_t> meshesMap = std::map<std::string, uint64_t>());
		static Entity* LoadModelToGame(string const& path, std::string modelName, bool useTangent);
		static Entity* LoadModelToGame(string const& path, std::string modelName, aiScene const* scene, bool useTangent);
		static void LoadModelMeshes(string filePath, unordered_map<uint64_t, shared_ptr<MeshRenderer>>& meshRenderers, Model* model, bool useTangent, std::map<std::string, uint64_t> meshesMap = std::map<std::string, uint64_t>());
		static void ProcessNode(aiNode* node, const aiScene* scene, vector<Mesh>& meshes, vector<Texture>& textures_loaded, string* directory, Entity* modelMainObject, unsigned int& index, bool useTangent);
		static Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, vector<Texture>& textures_loaded, string* directory, aiNode* node, bool useTangent);

		static vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, vector<Texture>& textures_loaded, string* directory);
		static unsigned int TextureFromFile(const char* path, const string& directory, bool gamma);
		static bool CheckFlippedTextures(const std::string& modelPath);
	};
	inline vector<string> ModelLoader::supportedFormats = { ".obj", ".fbx" };
	inline double ModelLoader::modelScale = 0.01;
}