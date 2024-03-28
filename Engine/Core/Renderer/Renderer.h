#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Mesh.h"
#include "Engine/Core/RenderGroup.h"
#include "Shadows.h"
#include "Skybox.h"
#include "Picking.h"

namespace Plaza { 

	enum RendererAPI {
		OpenGL,
		Vulkan
	};
	class Renderer {
	public:
		uint32_t mCurrentFrame = 0;

		RendererAPI api;
		Shadows* mShadows;
		Skybox* mSkybox;
		Picking* mPicking;
		const unsigned int mMaxFramesInFlight = 2;

		virtual void Init() = 0;
		virtual void UpdateProjectManager() = 0;
		virtual void InitShaders(std::string shadersFolder) = 0;
		virtual void AddInstancesToRender() = 0;
		virtual void RenderShadowMap(Shader& shader) = 0;
		virtual void RenderInstances(Shader& shader) = 0;
		virtual void RenderBloom() = 0;
		virtual void RenderScreenSpaceReflections() = 0;
		virtual void RenderFullScreenQuad() = 0;
		virtual void RenderOutline() = 0;
		virtual void RenderHDR() = 0;

		virtual Shadows* GetShadows() = 0;

		virtual Mesh& CreateNewMesh(vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> uvs, vector<glm::vec3> tangent, vector<glm::vec3> bitangent, vector<unsigned int> indices, Material& material, bool usingNormal) = 0;
		virtual Mesh* RestartMesh(Mesh* mesh) = 0;
		virtual void DrawRenderGroupInstanced(RenderGroup* renderGroup) = 0;

		virtual void InitGUI() = 0;
		virtual void NewFrameGUI() = 0;
		virtual void UpdateGUI() = 0;
		virtual ImTextureID GetFrameImage() = 0;

		virtual void Destroy() = 0;

		virtual Texture* LoadTexture(std::string path, uint64_t uuid = 0) {
			return new Texture();
		}
		virtual Texture* LoadImGuiTexture(std::string path) {
			return new Texture();
		}

		virtual void CopyLastFramebufferToFinalDrawBuffer() = 0;
	};
}