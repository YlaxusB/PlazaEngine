#pragma once
#include "Engine/Core/Renderer/Mesh.h"
#include "Renderer.h"

namespace Plaza {
	class VulkanMesh : public Mesh {
	public:
		RendererAPI api = RendererAPI::Vulkan;
	};
}