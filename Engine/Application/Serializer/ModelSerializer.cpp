#include "Engine/Core/PreCompiledHeaders.h"
#include "ModelSerializer.h"
#include "Engine/Application/Serializer/Components/MaterialSerializer.h"
#include "Engine/Application/Serializer/Components/TransformSerializer.h"
namespace Engine {
	void SerializeGameObject(YAML::Emitter& out, GameObject* gameObject) {
		//out << YAML::BeginMap;
		out << YAML::BeginMap;
		out << YAML::Key << "GameObject" << gameObject->uuid;
		out << YAML::Key << "Uuid" <<YAML::Value << gameObject->uuid;
		out << YAML::Key << "Name" << YAML::Value << gameObject->name;
		out << YAML::Key << "ParentID" << YAML::Value << (gameObject->parent != nullptr ? gameObject->parent->uuid : 0);
		out << YAML::Key << "Components" << YAML::Value << YAML::BeginMap;
		if (gameObject->GetComponent<Transform>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			glm::vec3& relativePosition = gameObject->transform->relativePosition;
			out << YAML::Key << "Position" << YAML::Value << relativePosition;
			glm::vec3& relativeRotation = gameObject->transform->rotation;
			out << YAML::Key << "Rotation" << YAML::Value << relativeRotation;
			glm::vec3& scale = gameObject->transform->scale;
			out << YAML::Key << "Scale" << YAML::Value << scale;

			out << YAML::EndMap;
		}
		MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
		if (meshRenderer) {
			if (meshRenderer->mesh) {
				out << YAML::Key << "MeshComponent" << YAML::Value << YAML::BeginMap;
				out << YAML::Key << "AiMeshName" << YAML::Value << meshRenderer->aiMeshName;
				out << YAML::Key << "MeshName" << YAML::Value << gameObject->name;
				out << YAML::Key << "MeshUUID" << YAML::Value << meshRenderer->mesh.get()->uuid;
				ComponentSerializer::MaterialSerializer::Serialize(out, meshRenderer->mesh.get()->material);
				out << YAML::EndMap;
			}
		}
		out << YAML::EndMap;
		out << YAML::EndMap;
	}

	void ModelSerializer::SerializeModel(GameObject* mainObject, string filePath, string modelFilePath) {
		uint64_t modelUuid = Engine::UUID::NewUUID();
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Model" << YAML::Value << mainObject->name;
		out << YAML::Key << "ModelFilePath" << YAML::Value << modelFilePath;
		out << YAML::Key << "ModelUuid" << YAML::Value << modelUuid;
		out << YAML::Key << "MainObject" << YAML::Value << YAML::BeginSeq;
		SerializeGameObject(out, mainObject);
		out << YAML::EndSeq;
		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;
		for (GameObject* gameObject : mainObject->children) {
			SerializeGameObject(out, gameObject);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	Model* ModelSerializer::DeSerializeModel(string filePath) {
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		YAML::Node data = YAML::Load(strStream.str());
		Model* model = new Model();
		model->modelName = data["Model"].as<string>();
		model->uuid = data["ModelUuid"].as<uint64_t>();
		model->modelFilePath = data["ModelFilePath"].as<string>();
		const YAML::Node& gameObjects = data["GameObjects"];
		for (const auto& gameObjectEntry : gameObjects) {
			const auto& componentsEntry = gameObjectEntry["Components"];
			GameObject* gameObject = new GameObject(gameObjectEntry["Name"].as<string>(), nullptr, false);
			gameObject->transform = ComponentSerializer::TransformSerializer::DeSerialize(componentsEntry["TransformComponent"]);
			gameObject->ReplaceComponent<Transform>(gameObject->GetComponent<Transform>(), gameObject->transform);
			if(componentsEntry["MeshComponent"]) {
				MeshRenderer* oldMeshRenderer = gameObject->GetComponent<MeshRenderer>();
				MeshRenderer* newMeshRenderer = new MeshRenderer();
				newMeshRenderer->aiMeshName = componentsEntry["MeshComponent"]["AiMeshName"].as<string>();
				gameObject->AddComponent<MeshRenderer>(newMeshRenderer);
			}
			model->gameObjects.push_back(make_shared<GameObject>(*gameObject));
		}
		return model;
	}


	uint64_t ModelSerializer::ReadUUID(string filePath) {
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		return data["ModelUuid"].as<uint64_t>();
	}
}