#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Physics/Collider.h"
namespace Plaza {
	class ComponentSerializer::ColliderSerializer {
	public:
		static void Serialize(YAML::Emitter& out, Collider& collider);
		static Collider* DeSerialize(YAML::Node data);
	};
}