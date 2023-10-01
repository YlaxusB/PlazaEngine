#include "Engine/Core/PreCompiledHeaders.h"
#include "ColliderSerializer.h"
#include "Engine/Core/Physics.h"
namespace Plaza {
	void ComponentSerializer::ColliderSerializer::Serialize(YAML::Emitter& out, Collider& collider) {
		out << YAML::Key << "ColliderComponent" << YAML::BeginMap;

		out << YAML::Key << "Uuid" << YAML::Value << collider.uuid;
		out << YAML::Key << "Shapes" << YAML::Value << YAML::BeginSeq;
		for (ColliderShape* shape : collider.mShapes) {
			out << YAML::BeginMap;
			out << YAML::Key << "Shape" << YAML::Value << shape->mEnum;
			out << YAML::Key << "MeshUuid" << YAML::Value << shape->mMeshUuid;
			out << YAML::EndMap;
		}
		out << YAML::EndSeq; // Shapes

		out << YAML::EndMap; // Collider Component
	}

	Collider* ComponentSerializer::ColliderSerializer::DeSerialize(YAML::Node data) {
		Collider* collider = new Collider(data["Uuid"].as<uint64_t>(), nullptr);
		Transform* transform = &Application->activeScene->transformComponents.at(collider->uuid);
		for (auto shapeDeserialized : data["Shapes"]) {
			if (shapeDeserialized["MeshUuid"].as<uint64_t>())
				collider->CreateShape(ColliderShapeEnum(shapeDeserialized["Shape"].as<int>()), transform, Application->activeScene->meshes.at(shapeDeserialized["MeshUuid"].as<uint64_t>()).get());
			else
				collider->CreateShape(ColliderShapeEnum(shapeDeserialized["Shape"].as<int>()), transform, 0);
		}
		return collider;
	}
}