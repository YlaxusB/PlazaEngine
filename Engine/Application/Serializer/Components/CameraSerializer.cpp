#include "Engine/Core/PreCompiledHeaders.h"
#include "CameraSerializer.h"
namespace Plaza {
	void ComponentSerializer::CameraSerializer::Serialize(YAML::Emitter& out, Camera& camera) {
		out << YAML::Key << "CameraComponent" << YAML::BeginMap;

		out << YAML::Key << "Uuid" << YAML::Value << camera.mUuid;
		out << YAML::Key << "Position" << YAML::Value << camera.Position;
		out << YAML::Key << "FOV" << YAML::Value << camera.Zoom;
		out << YAML::Key << "FarPlane" << YAML::Value << camera.farPlane;
		out << YAML::Key << "NearPlane" << YAML::Value << camera.nearPlane;
		out << YAML::Key << "AspectRatio" << YAML::Value << camera.aspectRatio;
		out << YAML::Key << "MainCamera" << YAML::Value << camera.mainCamera;
		out << YAML::Key << "Pitch" << YAML::Value << camera.Pitch;
		out << YAML::Key << "Yaw" << YAML::Value << camera.Yaw;

		out << YAML::EndMap; // Camera Component
	}

	Camera* ComponentSerializer::CameraSerializer::DeSerialize(YAML::Node data) {
		Camera* camera = new Camera();
		camera->mUuid = data["Uuid"].as<uint64_t>();
		camera->Position = data["Position"].as<glm::vec3>();
		camera->Zoom = data["FOV"].as<float>();
		camera->farPlane = data["FarPlane"].as<float>();
		camera->nearPlane = data["NearPlane"].as<float>();
		camera->aspectRatio = data["AspectRatio"].as<float>();
		camera->mainCamera = data["MainCamera"].as<bool>();
		camera->Pitch = data["Pitch"].as<float>();
		camera->Yaw = data["Yaw"].as<float>();
		return camera;
	}
}