#include "Engine/Core/PreCompiledHeaders.h"
#include "RigidBodySerializer.h"
#include "Engine/Core/Physics.h"
namespace Plaza {
	void ComponentSerializer::RigidBodySerializer::Serialize(YAML::Emitter& out, RigidBody& rigidBody) {
		out << YAML::Key << "RigidBodyComponent" << YAML::BeginMap;

		out << YAML::Key << "Uuid" << YAML::Value << rigidBody.uuid;
		out << YAML::Key << "DynamicLockFlags" << YAML::Value << (unsigned int)rigidBody.rigidDynamicLockFlags;

		out << YAML::EndMap; // RigidBody Component
	}

	RigidBody* ComponentSerializer::RigidBodySerializer::DeSerialize(YAML::Node data) {
		RigidBody* rigidBody = new RigidBody(data["Uuid"].as<uint64_t>(), Application->runningScene);
		rigidBody->rigidDynamicLockFlags = physx::PxRigidDynamicLockFlags(data["DynamicLockFlags"].as<unsigned int>());
		return rigidBody;
	}
}