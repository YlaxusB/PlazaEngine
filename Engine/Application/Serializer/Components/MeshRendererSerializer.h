#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
namespace Engine {
	class ComponentSerializer::MeshRendererSerializer {
	public:
		static void Serialize(YAML::Emitter& out, MeshRenderer& meshRenderer);
		static void DeSerialize(YAML::Emitter& out, MeshRenderer& meshRenderer);
	};
}