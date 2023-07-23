#include "Engine/Core/PreCompiledHeaders.h"
#include "SceneSerializer.h"
#include "Engine/Vendor/yaml/include/yaml-cpp/yaml.h"
namespace Engine {
	YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	void SerializeGameObject(YAML::Emitter& out, GameObject* gameObject) {
		out << YAML::BeginMap;
		out << YAML::Key << "GameObject" << YAML::Value << gameObject->id;

		if (gameObject->GetComponent<Transform>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			glm::vec3& position = gameObject->transform->position;
			out << YAML::Key << "Position" << YAML::Value << position;
			out << YAML::EndMap;
		}
		out << YAML::EndMap;
	}

	void Serializer::Serialize(const std::string filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		for (GameObject* gameObject : gameObjects) {
			SerializeGameObject(out, gameObject);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void Serializer::DeSerialize()
	{
	}

}