#include "Engine/Core/PreCompiledHeaders.h"
#include "RigidBodySerializer.h"
#include "Engine/Core/Physics.h"
namespace Plaza {
	void ComponentSerializer::RigidBodySerializer::Serialize(YAML::Emitter& out, RigidBody& rigidBody) {
		out << YAML::Key << "RigidBodyComponent" << YAML::BeginMap;

		out << YAML::Key << "Uuid" << YAML::Value << rigidBody.uuid;

		out << YAML::EndMap; // RigidBody Component
	}

	/*
						RigidBody* rigidBody = new RigidBody(entity.uuid, Application->runningScene);
					rigidBody->uuid = entity.uuid;
					//Collider* collider = new Collider(entity.uuid);
					//entity.AddComponent<Collider>(collider);
					//entity.GetComponent<Collider>()->Init();
					entity.AddComponent<RigidBody>(rigidBody);
	*/

	RigidBody* ComponentSerializer::RigidBodySerializer::DeSerialize(YAML::Node data) {
		RigidBody* rigidBody = new RigidBody(data["Uuid"].as<uint64_t>(), Application->runningScene);
		return rigidBody;
	}
}