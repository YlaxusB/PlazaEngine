#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Core/Texture.h"
namespace Engine {
	class ComponentSerializer::TextureSerializer {
	public:
		static void Serialize(YAML::Emitter& out, Texture& texture);
	};
}