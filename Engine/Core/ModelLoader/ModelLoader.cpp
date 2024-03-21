#include "Engine/Core/PreCompiledHeaders.h"
#include "ModelLoader.h"
#include "Engine/Core/Engine.h"
#include "Engine/Application/Serializer/ModelSerializer.h"
#include "Engine/Core/ModelLoader/Model.h"
#include "Engine/Components/Core/Entity.h"
#include "Engine/Application/Serializer/FileSerializer/FileSerializer.h"
#include "Engine/Core/Renderer/Renderer.h"

namespace Plaza {
	Material DefaultMaterial() {
		Material material;
		material.diffuse->rgba = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
		material.specular->rgba = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
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
					if (model->meshes.find(meshRenderer->aiMeshName) != model->meshes.end())
						newMeshRenderer->mesh = model->meshes.at(meshRenderer->aiMeshName).get();
					newMeshRenderer->material = meshRenderer->material;
					if (model->materials.find(meshRenderer->aiMeshName) != model->materials.end())
						newMeshRenderer->material = model->materials.at(meshRenderer->aiMeshName).get();//Application->activeScene->materials.at(meshRenderer->material->uuid).get();
					//newMeshRenderer->material = MaterialFileSerializer::DeSerialize(newMeshRenderer->material->filePath);
					//newMeshRenderer->material->LoadTextures(std::filesystem::path{ newMeshRenderer->material->filePath }.parent_path().string());

					if (model->meshes.find(meshRenderer->aiMeshName) != model->meshes.end() && model->materials.find(meshRenderer->aiMeshName) != model->materials.end())
						newMeshRenderer->renderGroup = Application->activeScene->AddRenderGroup(newMeshRenderer->mesh, newMeshRenderer->material);//make_shared<RenderGroup>(newMeshRenderer->mesh, newMeshRenderer->material);
					else
						newMeshRenderer->renderGroup = Application->activeScene->renderGroups[0];
					//newMeshRenderer->material = Application->activeScene->materials.at(entity->GetComponent<MeshRenderer>()->material->uuid);

					if (Application->activeScene->materials.find(it->second->uuid) == Application->activeScene->materials.end()) {
						//MaterialFileSerializer::Serialize(newMeshRenderer->material->filePath, newMeshRenderer->material.get());
						//Application->activeScene->materials.emplace(newMeshRenderer->material->uuid, newMeshRenderer->material);
						//Application->activeScene->AddMaterial(newMeshRenderer->material.get());
					}
					else {
						//newMeshRenderer->material = Application->activeScene->materials.at(Application->activeScene->materialsNames.at(newMeshRenderer->material->name));
					}
					//Application->activeScene->AddRenderGroup(newMeshRenderer->renderGroup);
					//Application->activeScene->renderGroups.emplace(newMeshRenderer->renderGroup->uuid, newMeshRenderer->renderGroup);
					newGameObject->AddComponent<MeshRenderer>(newMeshRenderer, true);
					//newGameObject->AddComponent<MeshRenderer>(model->meshRenderers.find(meshRenderer->aiMeshName)->second.get());
					Collider* collider = new Collider(newGameObject->uuid);
					if (newGameObject->HasComponent<MeshRenderer>())
						if (newGameObject->GetComponent<MeshRenderer>()->mesh)
						{
							collider->CreateShape(ColliderShape::ColliderShapeEnum::MESH, transform, newMeshRenderer->mesh);
							//collider->AddMeshShape(new Mesh(*newMeshRenderer->mesh));
							newGameObject->AddComponent<Collider>(collider);
						}
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
		ModelLoader::modelScale = model.get()->scale;
		LoadModelMeshes(filePath, model.get()->meshRenderers, model.get(), model.get()->useTangent, meshesMap);
		ModelLoader::modelScale = 0.01f;
		unordered_set<std::string> loadedNames = unordered_set<std::string>();
		//for (auto [key, meshRenderer] : model->meshRenderers) {
		//	if (!loadedNames.contains(meshRenderer->material->filePath)) {
		//		if (Application->activeScene->materials.find(meshRenderer->material->uuid) != Application->activeScene->materials.end()) {
		//			Application->activeScene->materials.find(meshRenderer->material->uuid)->second = std::shared_ptr<Material>(MaterialFileSerializer::DeSerialize(meshRenderer->material->filePath));
		//			//Application->activeScene->materials.find(meshRenderer->material->uuid)->second->LoadTextures();
		//			loadedNames.emplace(meshRenderer->material->filePath);
		//		}

		//	}
		//}
		EngineClass::models.emplace(model->uuid, move(model));
		// delete(model)   //////////////////////////////////////////////////////////////////////////////////////////////
		return uuid;
	}

	Entity* ModelLoader::LoadModelToGame(string const& path, std::string modelName, bool useTangent) {
		vector<Texture*>* textures_loaded = new vector<Texture*>;
		vector<Material>* materialsLoaded = new vector<Material>;
		vector<Mesh*>* meshes = new vector<Mesh*>();
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
		Entity* modelMainObject = new Entity(modelName, Application->activeScene->mainSceneEntity);
		//modelMainObject->AddComponent(new Transform());
		unsigned int index = 0;
		ModelLoader::ProcessNode(scene->mRootNode, scene, *meshes, *textures_loaded, *materialsLoaded, &directory, modelMainObject, index, useTangent, path, modelName);
		modelMainObject->GetComponent<Transform>()->UpdateChildrenTransform();

		Editor::selectedGameObject = modelMainObject;
		delete textures_loaded;
		delete meshes;
		return modelMainObject;
	}

	void ModelLoader::LoadModelMeshes(string filePath, unordered_map<uint64_t, shared_ptr<MeshRenderer>>& meshRenderers, Model* model, bool useTangent, std::map<std::string, uint64_t> meshesMap) {
		vector<Texture*>* texturesLoaded = new vector<Texture*>;
		vector<shared_ptr<Material>> materialsLoaded = vector<shared_ptr<Material>>();
		vector<Mesh>* meshes = new vector<Mesh>;
		string directory = filesystem::path{ filePath }.parent_path().string() + "\\textures";
		// read file via ASSIMP
		Assimp::Importer importer;
		std::string path = model->modelObjectPath;
		if (!path.starts_with(Application->projectPath))
			path = Application->projectPath + "\\" + path;
#ifdef GAME_MODE
		path = model->modelObjectPath;
#endif
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenUVCoords | aiProcess_TransformUVCoords);
		unsigned int index = 0;
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			aiMesh* aiMesh = scene->mMeshes[i];
			Material* processedMaterial = new Material();
			Mesh& mesh = ModelLoader::ProcessMesh(aiMesh, scene, *texturesLoaded, &directory, nullptr, useTangent, processedMaterial);

			mesh.meshId = Plaza::UUID::NewUUID();

			//for (shared_ptr<Material> material : materialsLoaded) {
			//	if (processedMaterial->SameAs(*material.get())) {
			//		processedMaterial = material;
			//	}
			//	else
			//		materialsLoaded.push_back(processedMaterial);
			//}

			mesh.meshName = aiMesh->mName.C_Str() + to_string(index);

			processedMaterial->name = mesh.meshName + "material";
			processedMaterial->filePath = std::string(directory) + "\\" + processedMaterial->name + Standards::materialExtName;

			if (Application->activeScene->materials.find(processedMaterial->uuid) == Application->activeScene->materials.end()) {
				//Application->activeScene->materials.emplace(mesh->material.uuid, &mesh->material);
				//Application->activeScene->AddMaterial(&mesh->material);
				//MaterialFileSerializer::Serialize(mesh->material.filePath, &mesh->material);
			}
			mesh.modelUuid = model->uuid;
			if (meshesMap.size() > 0) {
				mesh.meshId = meshesMap.at(mesh.meshName);
			}
			std::shared_ptr<Mesh> sharedMesh = std::make_shared<VulkanMesh>(dynamic_cast<VulkanMesh&>(mesh));
			std::shared_ptr<Material> sharedMaterial = std::make_shared<Material>(dynamic_cast<Material&>(*processedMaterial));
			Application->editorScene->meshes.emplace(mesh.meshId, sharedMesh);
			model->meshes.emplace(std::string(aiMesh->mName.C_Str() + to_string(index)), sharedMesh);
			model->materials.emplace(std::string(aiMesh->mName.C_Str() + to_string(index)), sharedMaterial);
			//((VulkanMesh&)(model->meshes.at(std::string(aiMesh->mName.C_Str() + to_string(index)))).get()).Drawe();
			//shar->Draw(*Application->shader);
			//Application->editorScene->meshes.at(mesh.meshId)->Draw(*Application->shader);
			//((VulkanMesh&)(Application->editorScene->meshes.at(mesh.meshId))).Draw(*asd);
			if (Application->runningScene)
				Application->runtimeScene->meshes.emplace(mesh.meshId, Application->editorScene->meshes.at(mesh.meshId));
			index++;
		}
	}

	Entity* ModelLoader::LoadModelToGame(string const& path, std::string modelName, aiScene const* scene, bool useTangent, std::string currentPath) {
		vector<Texture*>* textures_loaded = new vector<Texture*>;
		vector<Material>* materialsLoaded = new vector<Material>;
		vector<Mesh*>* meshes = new vector<Mesh*>;
		string directory;
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		Entity* modelMainObject = new Entity(modelName, Application->activeScene->mainSceneEntity);
		//modelMainObject->AddComponent(new Transform());
		unsigned int index = 0;
		ModelLoader::ProcessNode(scene->mRootNode, scene, *meshes, *textures_loaded, *materialsLoaded, &directory, modelMainObject, index, useTangent, currentPath, modelName);
		modelMainObject->GetComponent<Transform>()->UpdateChildrenTransform();

		Editor::selectedGameObject = modelMainObject;
		delete textures_loaded;
		delete meshes;
		return modelMainObject;
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, vector<Mesh*>& meshes, vector<Texture*>& textures_loaded, vector<Material>& materialsLoaded, string* directory, Entity* modelMainObject, unsigned int& index, bool useTangent, std::string currentPath, std::string modelName)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// Get the assimp mesh of the current node
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			// Convert it to my own mesh
			Material* processedMaterial = new Material();
			Mesh& nodeMesh = ProcessMesh(mesh, scene, textures_loaded, directory, node, useTangent, processedMaterial);

			for (Material& material : materialsLoaded) {
				if (processedMaterial->SameAs(material)) {
					*processedMaterial = material;
				}
			}
			materialsLoaded.push_back(*processedMaterial);

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
			MeshRenderer* childMeshRenderer = new MeshRenderer(&nodeMesh);
			childMeshRenderer->instanced = true;
			childMeshRenderer->aiMeshName = string(mesh->mName.C_Str()) + to_string(index);
			processedMaterial->name = childMeshRenderer->aiMeshName + processedMaterial->name;
			processedMaterial->filePath = currentPath + "\\" + modelName + "\\textures\\" + processedMaterial->name;
			childMeshRenderer->material = new Material(*processedMaterial);

			//childMeshRenderer->material->uuid = Plaza::UUID::NewUUID();
			childObject->AddComponent<MeshRenderer>(childMeshRenderer);
			childObject->GetComponent<Transform>()->relativePosition = position;
			parentObject->childrenUuid.push_back(childObject->uuid);
			index++;
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene, meshes, textures_loaded, materialsLoaded, directory, modelMainObject, index, useTangent, currentPath, modelName);
		}
	}

	Mesh& ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, vector<Texture*>& textures_loaded, string* directory, aiNode* node, bool useTangent, Material* outMaterial)
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

			if (mesh->HasTangentsAndBitangents() && useTangent) {
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

		Material convertedMaterial = DefaultMaterial();
		convertedMaterial.uuid = Plaza::UUID::NewUUID();
		//	convertedMaterial.diffuse = Application->mRenderer->LoadTexture();

		convertedMaterial.diffuse = ModelLoader::LoadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse", textures_loaded, directory)[0];
		//    convertedMaterial.specular = ModelLoader::LoadMaterialTextures(material, aiTextureType_SPECULAR, "specular", textures_loaded, directory)[0] ;
		//    convertedMaterial.normal = ModelLoader::LoadMaterialTextures(material, aiTextureType_HEIGHT, "normal", textures_loaded, directory)[0];
		//    convertedMaterial.height = ModelLoader::LoadMaterialTextures(material, aiTextureType_AMBIENT, "height", textures_loaded, directory)[0];
		//    convertedMaterial.metalness = ModelLoader::LoadMaterialTextures(material, aiTextureType_METALNESS, "metalness", textures_loaded, directory) [   0];
		//    convertedMaterial.roughness = ModelLoader::LoadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "roughness", textures_loaded, // d   irectory)[0];
		//    convertedMaterial.aoMap = ModelLoader::LoadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, "aoMap", textures_loaded, directory)[0];


		convertedMaterial.name = std::string(material->GetName().C_Str()) + "_" + std::string(material->GetName().C_Str()) + Standards::materialExtName;
		convertedMaterial.filePath = std::string(directory->c_str()) + "\\" + convertedMaterial.name;
		//convertedMaterial.diffuse->rgba = glm::vec4(INFINITY);
		//convertedMaterial.specular->rgba = glm::vec4(INFINITY);
		//convertedMaterial.normal->rgba = glm::vec4(INFINITY);
		//convertedMaterial.height->rgba = glm::vec4(INFINITY);
		//OpenGLMesh finalMesh = OpenGLMesh(vertices, normals, uvs, tangents, bitangents, indices, //convertedMaterial);
		//finalMesh.material = convertedMaterial;
		//finalMesh.usingNormal = usingNormal;
		*outMaterial = *new Material(convertedMaterial);
		return Application->mRenderer->CreateNewMesh(vertices, normals, uvs, tangents, bitangents, indices, convertedMaterial, usingNormal);
	}
}