#include "Engine/Core/PreCompiledHeaders.h"
#include "TransformSerializer.h"
namespace Engine {
	void ComponentSerializer::TransformSerializer::Serialize(YAML::Emitter& out, Transform& transform) {
		out << YAML::Key << "TransformComponent";
		out << YAML::BeginMap;

		glm::vec3& relativePosition = transform.relativePosition;
		out << YAML::Key << "Position" << YAML::Value << relativePosition;
		glm::vec3& relativeRotation = transform.rotation;
		out << YAML::Key << "Rotation" << YAML::Value << relativeRotation;
		glm::vec3& scale = transform.scale;
		out << YAML::Key << "Scale" << YAML::Value << scale;

		out << YAML::EndMap;
	}

	Transform* ComponentSerializer::TransformSerializer::DeSerialize(YAML::Node data) {
		Transform* transform = new Transform();
		transform->relativePosition = data["Position"].as<glm::vec3>();
		transform->rotation = data["Rotation"].as<glm::vec3>();
		transform->scale = data["Scale"].as<glm::vec3>();
		return transform;
	}
}