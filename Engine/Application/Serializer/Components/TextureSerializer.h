#pragma once
#include "ComponentSerializer.h"
#include "Engine/Core/Renderer/Texture.h"
namespace Plaza {
	class ComponentSerializer::TextureSerializer {
	public:
		static void Serialize(YAML::Emitter& out, Texture& texture);
		static Texture* DeSerialize(YAML::Node data);
	};
}