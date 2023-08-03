#pragma once
#include <vector>
#include "Engine/Components/Core/Material.h"
using namespace std;
namespace Engine {
	class Model {
		vector<shared_ptr<GameObject>> gameObjects;
		string modelName;
		vector<string> texturesPaths;
		vector<string> materialsPaths;
		vector<Material> materials;
	};
}