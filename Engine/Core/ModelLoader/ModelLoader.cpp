#include "Engine/Core/PreCompiledHeaders.h"
#include "ModelLoader.h"
#include "Engine/Core/Engine.h"
#include "Engine/Application/Serializer/ModelSerializer.h"
#include "Engine/Core/ModelLoader/Model.h"
#include "Engine/Components/Core/Entity.h"

namespace Plaza {
	Material DefaultMaterial() {
		Material material;
		material.diffuse.rgba = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
		material.specular.rgba = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		material.shininess = 3.0f;
		return material;
	}

	Entity* ModelLoader::LoadImportedModelToScene(uint64_t modelUuid, string filePath) {
		Model* model = nullptr;
		auto it = EngineClass::models.find(modelUuid);
		if (it != EngineClass::models.end()) {
			model = it->second.get();
			//Entity* mainObject = model->gameObjects.front().get();
			unordered_map<uint64_t, Entity*> modelInstanceGameObjects = unordered_map<uint64_t, Entity*>();
			modelInstanceGameObjects.clear();
			unsigned int index = 0;
			Entity* mainModelObject = nullptr;
			for (const auto& gameObjectSharedPointer : model->gameObjects) {
				Entity* entity = gameObjectSharedPointer.get();
				uint64_t parentUuid = 0;
				if (modelInstanceGameObjects.contains(entity->parentUuid)) {
					parentUuid = modelInstanceGameObjects.at(entity->parentUuid)->uuid;
				}
				if (parentUuid == 0) {
					parentUuid = Application->activeScene->mainSceneEntity->uuid;
				}
				//Application->activeScene->entities.at(newGameObject->parentUuid).childrenUuid.push_back(newGameObject->uuid);

				Entity* newGameObject = new Entity(entity->name, &Application->activeScene->entities.at(parentUuid));
				Transform* transform = model->transforms.at(entity->uuid).get();//entity->GetComponent<Transform>();
				newGameObject->GetComponent<Transform>()->relativePosition = transform->relativePosition;
				newGameObject->GetComponent<Transform>()->rotation = transform->rotation;
				newGameObject->GetComponent<Transform>()->scale = transform->scale;

				const auto& it = model->meshRenderers.find(entity->uuid);
				if (it != model->meshRenderers.end()) {
					MeshRenderer* meshRenderer = model->meshRenderers.at(entity->uuid).get();
					MeshRenderer* newMeshRenderer = new MeshRenderer();
					newMeshRenderer->instanced = true;
					newMeshRenderer->uuid = newGameObject->uuid;
					newMeshRenderer->mesh = shared_ptr<Mesh>(model->meshes.at(meshRenderer->aiMeshName));
					newGameObject->AddComponent<MeshRenderer>(newMeshRenderer, true);
					//newGameObject->AddComponent<MeshRenderer>(model->meshRenderers.find(meshRenderer->aiMeshName)->second.get());
					Collider* collider = new Collider(newGameObject->uuid);
					collider->AddMeshShape(new Mesh(*newMeshRenderer->mesh));
					newGameObject->AddComponent<Collider>(collider);
				}
				modelInstanceGameObjects.emplace(entity->uuid, newGameObject);
				/*
				for (shared_ptr component : entity->components) {
					newGameObject->AddComponent<Component>(component.get());
				}
				*/

				index++;

				if (!mainModelObject) {
					mainModelObject = newGameObject;
				}
			}
			modelInstanceGameObjects[model->gameObjects.front().get()->uuid]->GetComponent<Transform>()->UpdateChildrenTransform();
			Application->activeScene->mainSceneEntity->childrenUuid.push_back(modelInstanceGameObjects[model->gameObjects.front().get()->uuid]->uuid);
			Application->activeScene->mainSceneEntity->GetComponent<Transform>()->UpdateChildrenTransform();
		}
		else {
			if (!filePath.empty()) {
				uint64_t modelUuid = ModelLoader::LoadImportedModelToMemory(filePath);
				LoadImportedModelToScene(modelUuid);
			}
			return nullptr;
		}
	}

	uint64_t ModelLoader::LoadImportedModelToMemory(string filePath, std::map<std::string, uint64_t> meshesMap) {
		Entity* ent = Application->activeScene->mainSceneEntity;
		unique_ptr<Model> model = make_unique<Model>(*ModelSerializer::DeSerializeModel(filePath));
		model.get()->meshRenderers;
		uint64_t uuid = model->uuid;
		LoadModelMeshes(filePath, model.get()->meshRenderers, model.get(), meshesMap);
		EngineClass::models.emplace(model->uuid, move(model));
		// delete(model)   //////////////////////////////////////////////////////////////////////////////////////////////
		return uuid;
	}

	Entity* ModelLoader::LoadModelToGame(string const& path, std::string modelName) {
		vector<Texture>* textures_loaded = new vector<Texture>;
		vector<Mesh>* meshes = new vector<Mesh>;
		string directory;
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords | aiProcess_TransformUVCoords);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return nullptr;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		Entity* modelMainObject = new Entity(modelName, Application->activeScene->gameObjects.front().get());
		//modelMainObject->AddComponent(new Transform());
		unsigned int index = 0;
		ModelLoader::ProcessNode(scene->mRootNode, scene, *meshes, *textures_loaded, &directory, modelMainObject, index);
		modelMainObject->GetComponent<Transform>()->UpdateChildrenTransform();

		Editor::selectedGameObject = modelMainObject;
		delete textures_loaded;
		delete meshes;
		return modelMainObject;
	}

	void ModelLoader::LoadModelMeshes(string filePath, unordered_map<uint64_t, shared_ptr<MeshRenderer>>& meshRenderers, Model* model, std::map<std::string, uint64_t> meshesMap) {
		vector<Texture>* texturesLoaded = new vector<Texture>;
		vector<Mesh>* meshes = new vector<Mesh>;
		string directory = filesystem::path{ filePath }.parent_path().string() + "\\textures";
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(model->modelObjectPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords | aiProcess_TransformUVCoords);
		unsigned int index = 0;
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* aiMesh = scene->mMeshes[i];
			Mesh* mesh = new Mesh(ModelLoader::ProcessMesh(aiMesh, scene, *texturesLoaded, &directory, nullptr));
			mesh->meshName = aiMesh->mName.C_Str() + to_string(index);
			mesh->modelUuid = model->uuid;
			if (meshesMap.size() > 0) {
				mesh->meshId = meshesMap.at(mesh->meshName);
			}

			Application->editorScene->meshes.emplace(mesh->meshId, make_shared<Mesh>(*mesh));
			model->meshes.emplace(std::string(aiMesh->mName.C_Str() + to_string(index)), Application->editorScene->meshes.at(mesh->meshId));
			if (Application->runningScene)
				Application->runtimeScene->meshes.emplace(mesh->meshId, Application->editorScene->meshes.at(mesh->meshId));
			index++;
		}
	}

	Entity* ModelLoader::LoadModelToGame(string const& path, std::string modelName, aiScene const* scene) {
		vector<Texture>* textures_loaded = new vector<Texture>;
		vector<Mesh>* meshes = new vector<Mesh>;
		string directory;
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		Entity* modelMainObject = new Entity(modelName, Application->activeScene->mainSceneEntity);
		//modelMainObject->AddComponent(new Transform());
		unsigned int index = 0;
		ModelLoader::ProcessNode(scene->mRootNode, scene, *meshes, *textures_loaded, &directory, modelMainObject, index);
		modelMainObject->GetComponent<Transform>()->UpdateChildrenTransform();

		Editor::selectedGameObject = modelMainObject;
		delete textures_loaded;
		delete meshes;
		return modelMainObject;
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, vector<Mesh>& meshes, vector<Texture>& textures_loaded, string* directory, Entity* modelMainObject, unsigned int& index)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// Get the assimp mesh of the current node
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			// Convert it to my own mesh
			Mesh nodeMesh = ProcessMesh(mesh, scene, textures_loaded, directory, node);
			std::string childName = std::string(node->mName.C_Str());
			// Get the position of the mesh
			aiMatrix4x4 transformationMatrix = node->mTransformation;
			glm::vec3 position;
			position.x = transformationMatrix.a4 * modelScale;
			position.y = transformationMatrix.b4 * modelScale;
			position.z = transformationMatrix.c4 * modelScale;
			// Finds the parent Object, if its not found or there inst any, then it just assigns to the model main entity
			std::string parentName = node->mParent->mName.C_Str();
			Entity* parentObject = nullptr;
			if (parentName != "RootNode" || parentObject == nullptr) {
				parentObject = modelMainObject;
			}
			else {
				parentObject = Application->activeScene->gameObjects.front().get();
			}
			Entity* childObject = new Entity(childName, parentObject);
			MeshRenderer* childMeshRenderer = new MeshRenderer(nodeMesh);
			childMeshRenderer->instanced = true;
			childMeshRenderer->aiMeshName = string(mesh->mName.C_Str()) + to_string(index);
			childObject->AddComponent<MeshRenderer>(childMeshRenderer);
			childObject->GetComponent<Transform>()->relativePosition = position;
			parentObject->childrenUuid.push_back(childObject->uuid);
			index++;
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene, meshes, textures_loaded, directory, modelMainObject, index);
		}
	}

	Mesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, vector<Texture>& textures_loaded, string* directory, aiNode* node)
	{
		// data to fill
		//vector<Vertex> vertices;
		vector<glm::vec3> vertices;
		vector<glm::vec3> normals;
		vector<glm::vec3> tangents;
		vector<glm::vec3> bitangents;
		vector<glm::vec2> uvs;
		vector<unsigned int> indices;
		vector<Texture> textures;
		aiColor3D color(0.f, 0.f, 0.f);
		scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);

		bool usingNormal = false;
		// walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			glm::vec3 vertex;
			glm::vec3 normal;
			glm::vec3 tangent;
			glm::vec3 bitangent;
			glm::vec2 uv;
			//Vertex* vertex = new Vertex(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = mesh->mVertices[i].x * modelScale;
			vector.y = mesh->mVertices[i].y * modelScale;
			vector.z = mesh->mVertices[i].z * modelScale;
			vertex = vector;
			vertices.push_back(vertex);
			// normals
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x * modelScale;
				vector.y = mesh->mNormals[i].y * modelScale;
				vector.z = mesh->mNormals[i].z * modelScale;
				normal = vector;
				normals.push_back(normal);
			}

			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				uv = vec;
				uvs.push_back(uv);
			}
			else
				uv = glm::vec2(0.0f, 0.0f);

			if (mesh->HasTangentsAndBitangents()) {
				usingNormal = true;
				// tangent
				vector.x = mesh->mTangents[i].x * modelScale;
				vector.y = mesh->mTangents[i].y * modelScale;
				vector.z = mesh->mTangents[i].z * modelScale;
				tangent = vector;
				tangents.push_back(tangent);
				// bitangent
				vector.x = mesh->mBitangents[i].x * modelScale;
				vector.y = mesh->mBitangents[i].y * modelScale;
				vector.z = mesh->mBitangents[i].z * modelScale;
				bitangent = vector;
				bitangents.push_back(bitangent);
			}

		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		vector<Texture> diffuseMaps = ModelLoader::LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", textures_loaded, directory);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		vector<Texture> specularMaps = ModelLoader::LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", textures_loaded, directory);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = ModelLoader::LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", textures_loaded, directory);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = ModelLoader::LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", textures_loaded, directory);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		// return a mesh object created from the extracted mesh data
		Material convertedMaterial = DefaultMaterial();
		if (diffuseMaps.size() > 0 && diffuseMaps[0].id != 0)
			convertedMaterial.diffuse = diffuseMaps[0];

		if (specularMaps.size() > 0)
			convertedMaterial.specular = specularMaps[0];

		if (normalMaps.size() > 0)
			convertedMaterial.normal = normalMaps[0];

		if (heightMaps.size() > 0)
			convertedMaterial.height = heightMaps[0];

		Mesh finalMesh = Mesh(vertices, normals, uvs, tangents, bitangents, indices, convertedMaterial);
		finalMesh.usingNormal = usingNormal;
		return finalMesh;
	}
}