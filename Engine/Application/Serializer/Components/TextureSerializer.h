#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Rendering/Texture.h"
namespace Engine {
	class ComponentSerializer::TextureSerializer {
	public:
		static void Serialize(YAML::Emitter& out, Texture& texture);
	};
}