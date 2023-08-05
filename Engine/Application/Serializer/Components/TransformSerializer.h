#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Core/Transform.h"
namespace Engine {
	class ComponentSerializer::TransformSerializer {
	public:
		static void Serialize(YAML::Emitter& out, Transform& transform);
		static Transform* DeSerialize(YAML::Node data);
	};
}