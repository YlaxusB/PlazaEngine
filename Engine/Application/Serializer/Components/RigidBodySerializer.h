#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Physics/RigidBody.h"
namespace Plaza {
	class ComponentSerializer::RigidBodySerializer {
	public:
		static void Serialize(YAML::Emitter& out, RigidBody& rigidBody);
		static RigidBody* DeSerialize(YAML::Node data);
	};
}