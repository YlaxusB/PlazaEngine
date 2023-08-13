#pragma once
#include <vector>
#include "Engine/Components/Rendering/Material.h"
using namespace std;
namespace Engine {
	class Model {
	public:
		uint64_t uuid;
		vector<shared_ptr<GameObject>> gameObjects;
		unordered_map<uint64_t, shared_ptr<MeshRenderer>> meshRenderers;
		unordered_map<uint64_t, shared_ptr<Transform>> transforms;
		string modelName;
		string modelFilePath;
		vector<string> texturesPaths;
		vector<string> materialsPaths;
		vector<Material> materials;
		std::unordered_map<std::string, shared_ptr<Mesh>> meshes;
		Model() = default;
		Model(Model& other) = default;
	};
}