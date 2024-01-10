#pragma once

namespace Plaza {
	enum RendererAPI {
		OpenGL,
		Vulkan
	};
	class Renderer {
	public:
		virtual void Init() = 0;
		virtual void InitShaders(std::string shadersFolder) = 0;
		virtual void AddInstancesToRender() = 0;
		virtual void RenderShadowMap(Shader& shader) = 0;
		virtual void RenderInstances(Shader& shader) = 0;
		virtual void RenderBloom() = 0;
		virtual void RenderScreenSpaceReflections() = 0;
		virtual void RenderFullScreenQuad() = 0;
		virtual void RenderOutline() = 0;
		virtual void RenderHDR() = 0;

		virtual void CopyLastFramebufferToFinalDrawBuffer() = 0;
	};
}