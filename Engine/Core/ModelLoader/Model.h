#pragma once
#include <vector>
#include "Engine/Components/Rendering/Material.h"
using namespace std;
namespace Engine {
	class Model {
	public:
		uint64_t uuid;
		vector<shared_ptr<GameObject>> gameObjects;
		unordered_map<string, shared_ptr<MeshRenderer>> meshRenderers;
		string modelName;
		string modelFilePath;
		vector<string> texturesPaths;
		vector<string> materialsPaths;
		vector<Material> materials;
		vector<Mesh> meshes;
		Model() = default;
		Model(Model& other) = default;
	};
}