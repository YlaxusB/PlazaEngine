#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Engine/stb_image.h"

#include "Engine/Components/Core/Mesh.h"
#include "Engine/Shaders/Shader.h"
//#include "Engine/Components/Core/Model.h"
#include "Engine/Components/Core/GameObject.h"
#include "Engine/Editor/Editor.h"
#include "Engine/Components/Core/Material.h"

#include "Engine/Application/Serializer/Components/MeshSerializer.h"
using namespace std;

//unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

using Engine::Mesh;
namespace Engine {
	namespace ModelLoader
	{
		vector<string> supportedFormats = { ".obj", ".fbx", ".usdz"};
		vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, vector<Texture>& textures_loaded, string* directory);
		void processNode(aiNode* node, const aiScene* scene, vector<Mesh>& meshes, vector<Texture>& textures_loaded, string* directory, GameObject* modelMainObject);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene, vector<Texture>& textures_loaded, string* directory, aiNode* node);
		double modelScale = 0.01;




		


		// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
		

		

		

		// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
		



		class ModelLoader {
		public:
		};
	};
}