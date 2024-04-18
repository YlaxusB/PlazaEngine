#pragma once
#include <vector>
#include "Engine/Components/Rendering/Material.h"
using namespace std;
namespace Plaza {
	class Model {
	public:
		uint64_t uuid;
		vector<shared_ptr<Entity>> gameObjects;
		unordered_map<uint64_t, shared_ptr<MeshRenderer>> meshRenderers;
		unordered_map<uint64_t, shared_ptr<Transform>> transforms;
		string modelName;
		string modelObjectPath;
		string modelPlazaPath;
		vector<string> texturesPaths;
		vector<string> materialsPaths;
		std::unordered_map<std::string, shared_ptr<Material>> materials;
		std::unordered_map<std::string, shared_ptr<Mesh>> meshes;
		float scale = 1.0f;
		bool useTangent = false;
		Model() = default;
		Model(Model& other) = default;
	};
}