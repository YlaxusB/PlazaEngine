#pragma once
#include "ComponentSerializer.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
namespace Plaza {
	class ComponentSerializer::MeshRendererSerializer {
	public:
		static void Serialize(YAML::Emitter& out, MeshRenderer& meshRenderer);
		static MeshRenderer* DeSerialize(YAML::Node data);
	};
}