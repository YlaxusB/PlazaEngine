#include "Engine/Core/PreCompiledHeaders.h"
#include "ModelSerializer.h"
namespace Engine {
	void SerializeGameObject(YAML::Emitter& out, GameObject* gameObject) {
		out << YAML::BeginMap;
		out << YAML::Key << "GameObject" << YAML::Value << gameObject->uuid;
		out << YAML::Key << "Name" << YAML::Value << gameObject->name;
		out << YAML::Key << "ParentID" << YAML::Value << (gameObject->parent != nullptr ? gameObject->parent->name : "");
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
		out << YAML::EndMap;
	}

	void ModelSerializer::SerializeModel(GameObject* mainObject, string filePath) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Model: " << YAML::Value << mainObject->name;
		out << YAML::Key << "MainObject: " << YAML::Value << YAML::BeginSeq;
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

	void ModelSerializer::DeSerializeModel(string filePath) {

	}
}