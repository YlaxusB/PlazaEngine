#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Rendering/Material.h"
namespace Engine {
	class ComponentSerializer;
	class ComponentSerializer::MaterialSerializer {
	public:
		static void Serialize(YAML::Emitter& out, Material& material);
		static void DeSerialize(YAML::Emitter& out, Material& material);
	};
}