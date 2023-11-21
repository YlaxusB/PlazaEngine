#include "Engine/Core/PreCompiledHeaders.h"
#include "SceneSerializer.h"

#include "Engine/Core/Engine.h"
#include "Editor/Settings/EditorSettings.h"
#include "Engine/Core/ModelLoader/ModelLoader.h"

#include "Engine/Application/Serializer/Components/TransformSerializer.h"
#include "Engine/Application/Serializer/Components/MeshRendererSerializer.h"
#include "Engine/Application/Serializer/Components/ColliderSerializer.h"
#include "Engine/Application/Serializer/Components/RigidBodySerializer.h"
#include "Engine/Application/Serializer/Components/CsScriptComponentSerializer.h"
#include "Engine/Application/Serializer/Components/CameraSerializer.h"
#include "Engine/Application/Serializer/Components/AudioListenerSerializer.h"
#include "Engine/Application/Serializer/Components/AudioSourceSerializer.h"

#include "Editor/DefaultAssets/Models/DefaultModels.h"

namespace Plaza {
	void SerializeGameObjects(YAML::Emitter& out, Entity* entity) {
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity->uuid;
		out << YAML::Key << "Name" << YAML::Value << entity->name;
		out << YAML::Key << "ParentID" << YAML::Value << entity->parentUuid;
		out << YAML::Key << "Components" << YAML::BeginMap;
		if (Transform* transform = entity->GetComponent<Transform>()) {
			ComponentSerializer::TransformSerializer::Serialize(out, *transform);
		}
		if (MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>()) {
			ComponentSerializer::MeshRendererSerializer::Serialize(out, *meshRenderer);
		}
		if (Camera* camera = entity->GetComponent<Camera>()) {
			ComponentSerializer::CameraSerializer::Serialize(out, *camera);
		}
		if (Collider* collider = entity->GetComponent<Collider>()) {
			ComponentSerializer::ColliderSerializer::Serialize(out, *collider);
		}
		if (RigidBody* rigidBody = entity->GetComponent<RigidBody>()) {
			ComponentSerializer::RigidBodySerializer::Serialize(out, *rigidBody);
		}
		if (entity->GetComponent<CsScriptComponent>()) {
			ComponentSerializer::CsScriptComponentSerializer::SerializeAll(out, entity->uuid);
		}
		if (AudioSource* audioSource = entity->GetComponent<AudioSource>()) {
			ComponentSerializer::AudioSourceSerializer::Serialize(out, *audioSource);
		}
		if (AudioListener* audioListener = entity->GetComponent<AudioListener>()) {
			ComponentSerializer::AudioListenerSerializer::Serialize(out, *audioListener);
		}
		out << YAML::EndMap;

		out << YAML::EndMap;
	}

	void SerializeMeshes(YAML::Emitter& out, MeshRenderer* meshRenderer) {
		Mesh* mesh = meshRenderer->mesh;
		if (mesh) {
			out << YAML::BeginMap;
			out << YAML::Key << "Mesh" << YAML::Value << mesh->uuid;
			out << YAML::Key << "MeshId" << YAML::Value << mesh->meshId;
			out << YAML::Key << "Name" << YAML::Value << mesh->meshName;
			out << YAML::Key << "TemporaryMesh" << YAML::Value << mesh->temporaryMesh;
			out << YAML::Key << "ModelUuid" << YAML::Value << mesh->modelUuid;
			out << YAML::EndMap;
		}
	}

	void SerializeModels(YAML::Emitter& out, Model* model) {
		out << YAML::BeginMap;
		out << YAML::Key << "Model" << YAML::Value << model->uuid;
		out << YAML::Key << "Name" << YAML::Value << model->modelName;
		std::string modelPath = model->modelPlazaPath;
		if (modelPath.starts_with(Application->projectPath))
			modelPath = modelPath.substr(Application->projectPath.length(), modelPath.length() - Application->projectPath.length() + 2);
		if (modelPath.starts_with("\\"))
			modelPath = modelPath.substr(1, modelPath.length() - 1);
		out << YAML::Key << "ModelPath" << YAML::Value << modelPath;
		out << YAML::Key << "ObjectPath" << YAML::Value << model->modelObjectPath;
		out << YAML::Key << "TexturesPath" << YAML::Value << model->texturesPaths;
		out << YAML::EndMap;
	}

	void SerializeScene(YAML::Emitter& out, Entity* sceneEntity) {
		out << YAML::Key << "Uuid" << YAML::Value << sceneEntity->uuid;
		out << YAML::Key << "Name" << YAML::Value << sceneEntity->name;
	}

	void Serializer::Serialize(const std::string filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << YAML::BeginMap;
		SerializeScene(out, Application->activeScene->mainSceneEntity);
		out << YAML::EndMap;
		/* Entities */
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		for (auto& [key, value] : Application->activeScene->entities) {
			SerializeGameObjects(out, &value);
		}
		out << YAML::EndSeq;

		/* Models */
		out << YAML::Key << "Models" << YAML::Value << YAML::BeginSeq;
		for (auto& [key, value] : EngineClass::models) {
			SerializeModels(out, value.get());
		}
		out << YAML::EndSeq;

		/* Meshes (Only their uuids and paths) */
		out << YAML::Key << "Meshes" << YAML::Value << YAML::BeginSeq;
		for (auto& [key, value] : Application->activeScene->meshRendererComponents) {
			SerializeMeshes(out, &value);
		}
		out << YAML::EndSeq;


		out << YAML::EndMap;
		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void Serializer::DeSerialize(const std::string filePath, bool deserializingProject)
	{
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			std::cout << "No Scene" << std::endl;

		std::string sceneName = data["Scene"]["Name"].as<std::string>();

		/* Scene */
		if (!deserializingProject)
			Application->editorScene = new Scene();
		Application->activeScene = Application->editorScene;
		Editor::DefaultModels::Init();
		Entity* oldScene = Application->activeScene->mainSceneEntity;
		Entity* newScene = new Entity(data["Scene"]["Name"].as<std::string>(), nullptr, true, data["Scene"]["Uuid"].as<uint64_t>());
		free(Application->activeScene->mainSceneEntity);
		Application->activeScene->mainSceneEntity = newScene;
		if (filePath.starts_with(Application->projectPath))
			Application->activeScene->filePath = filePath.substr(Application->projectPath.length() + 1, filePath.length() - Application->projectPath.length());
		else
			Application->activeScene->filePath = filePath;

		std::cout << "Deserializing meshes \n";
		/* Models and Meshes */
		std::map<uint64_t, std::string> models = std::map<uint64_t, std::string>();
		auto modelsDeserialized = data["Models"];
		if (modelsDeserialized) {
			for (auto model : modelsDeserialized) {
				std::string modelPath = Application->projectPath + "\\" + model["ModelPath"].as<std::string>();
				models.emplace(model["Model"].as<uint64_t>(), modelPath);
			}
		}
		// A map of model uuid and another map of mesh name and mesh id
		std::map<uint64_t, std::map<std::string, uint64_t>> meshes = std::map<uint64_t, std::map<std::string, uint64_t>>();

		auto meshesDeserialized = data["Meshes"];
		if (meshesDeserialized) {
			for (auto mesh : meshesDeserialized) {
				auto meshModelIt = meshes.find(mesh["ModelUuid"].as<uint64_t>());
				if (mesh["ModelUuid"].as<uint64_t>()) {
					if (meshModelIt != meshes.end()) {
						meshModelIt->second.emplace(mesh["Name"].as<std::string>(), mesh["MeshId"].as<uint64_t>());
					}
					else {
						std::map<std::string, uint64_t> newMap = std::map<std::string, uint64_t>();
						newMap.emplace(mesh["Name"].as<std::string>(), mesh["MeshId"].as<uint64_t>());
						meshes.emplace(mesh["ModelUuid"].as<uint64_t>(), newMap);
					}
				}
			}
		}

		std::cout << "Loading meshes \n";
		for (auto& [key, modelPath] : models) {
			auto meshIt = meshes.find(key);
			if (meshIt != meshes.end()) {
				std::cout << "Loading model: " << modelPath << "\n";
				if (std::filesystem::exists(modelPath))
					ModelLoader::LoadImportedModelToMemory(modelPath, meshIt->second);
			}
		}


		std::cout << "Loading entities \n";
		/* Entities */
		auto gameObjectsDeserialized = data["Entities"];
		if (gameObjectsDeserialized) {
			for (auto entity : gameObjectsDeserialized) {
				std::string name = entity["Name"].as<std::string>();
				Entity* newEntity;
				auto parentIt = Application->activeScene->entities.find(entity["ParentID"].as<std::uint64_t>());
				if (entity["ParentID"].as<std::uint64_t>() != entity["Entity"].as<uint64_t>()) {
					if (entity["ParentID"].as<std::uint64_t>() && parentIt != Application->activeScene->entities.end()) {
						newEntity = new Entity(name, &Application->activeScene->entities.at(entity["ParentID"].as<std::uint64_t>()), true, entity["Entity"].as<uint64_t>());
					}
					else {
						newEntity = new Entity(name, Application->activeScene->mainSceneEntity, true, entity["Entity"].as<uint64_t>());
					}
					newEntity->parentUuid = entity["ParentID"].as<std::uint64_t>();
					if (newEntity)
						if (entity["Components"]) {
							if (entity["Components"]["TransformComponent"]) {
								newEntity->GetComponent<Transform>()->relativePosition = entity["Components"]["TransformComponent"]["Position"].as<glm::vec3>();
								newEntity->GetComponent<Transform>()->rotation = entity["Components"]["TransformComponent"]["Rotation"].as<glm::vec3>();
								newEntity->GetComponent<Transform>()->scale = entity["Components"]["TransformComponent"]["Scale"].as<glm::vec3>();
								//newEntity->GetComponent<Transform>()->UpdateLocalMatrix();
								//newEntity->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
							}
							if (entity["Components"]["MeshRendererComponent"]) {
								newEntity->AddComponent<MeshRenderer>(ComponentSerializer::MeshRendererSerializer::DeSerialize(entity["Components"]["MeshRendererComponent"]));
							}
							if (entity["Components"]["CameraComponent"]) {
								newEntity->AddComponent<Camera>(ComponentSerializer::CameraSerializer::DeSerialize(entity["Components"]["CameraComponent"]));
							}
							if (entity["Components"]["ColliderComponent"]) {
								newEntity->AddComponent<Collider>(ComponentSerializer::ColliderSerializer::DeSerialize(entity["Components"]["ColliderComponent"]));
							}
							if (entity["Components"]["RigidBodyComponent"]) {
								newEntity->AddComponent<RigidBody>(ComponentSerializer::RigidBodySerializer::DeSerialize(entity["Components"]["RigidBodyComponent"]));
							}
							if (entity["Components"]["CsScriptComponent"]) {
								for (auto script : entity["Components"]["CsScriptComponent"]["Scripts"]) {
									newEntity->AddComponent<CsScriptComponent>(ComponentSerializer::CsScriptComponentSerializer::DeSerialize(script));
								}
							}
							if (entity["Components"]["AudioSourceComponent"]) {
								newEntity->AddComponent<AudioSource>(ComponentSerializer::AudioSourceSerializer::DeSerialize(entity["Components"]["AudioSourceComponent"]));
							}
							if (entity["Components"]["AudioListenerComponent"]) {
								newEntity->AddComponent<AudioListener>(ComponentSerializer::AudioListenerSerializer::DeSerialize(entity["Components"]["AudioListenerComponent"]));
							}
						}
					//newEntity->GetComponent<Transform>()->UpdateChildrenTransform();
					//meshRenderer->mesh = cubeMesh;
					//delete meshRenderer;
				}
			}
			/* Loop again to assign the correct parents */
			for (auto entity : gameObjectsDeserialized) {
				uint64_t entityUuid = entity["Entity"].as<uint64_t>();
				uint64_t parentUuid = entity["ParentID"].as<uint64_t>();
				if (parentUuid && parentUuid != entityUuid && Application->activeScene->entities.find(parentUuid) != Application->activeScene->entities.end())
					Application->activeScene->entities.at(entity["Entity"].as<uint64_t>()).ChangeParent(Application->activeScene->entities.at(entity["Entity"].as<uint64_t>()).GetParent(), Application->activeScene->entities.at(entity["ParentID"].as<uint64_t>()));
				else
					Application->activeScene->entities.at(entity["Entity"].as<uint64_t>()).ChangeParent(Application->activeScene->entities.at(entity["Entity"].as<uint64_t>()).GetParent(), *Application->activeScene->mainSceneEntity);
				Application->activeScene->entities.at(entity["Entity"].as<uint64_t>()).GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
			}
			//delete oldScene;
			std::cout << "Finished Deserialization \n";
		}
	}

}