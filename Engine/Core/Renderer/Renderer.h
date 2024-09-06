#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Mesh.h"
#include "Engine/Components/Rendering/Material.h"
#include "Engine/Core/RenderGroup.h"
#include "Shadows.h"
#include "Lighting.h"
#include "Skybox.h"
#include "Picking.h"
#include "GuiRenderer.h"
#include "RenderGraph.h"

namespace Plaza {

	enum RendererAPI {
		OpenGL,
		Vulkan
	};

	struct TrackedImage {
		ImTextureID mTextureID = 0;
		std::chrono::system_clock::time_point mCreationDate = std::chrono::system_clock::now();
		std::string name;
	};

	class Renderer {
	public:

		std::vector<TrackedImage> mTrackedImages = std::vector<TrackedImage>();

		uint32_t mCurrentFrame = 0;

		float exposure = 2.0f;//4.5f;
		float gamma = 2.0f;//2.0f;
		glm::vec3 sunColor = glm::vec3(1.0f);
		float mSkyboxIntensity = 1.0f;

		RendererAPI api;
		Shadows* mShadows;
		Lighting* mLighting;
		Skybox* mSkybox;
		Picking* mPicking;
		GuiRenderer* mGuiRenderer;
		PlazaRenderGraph* mRenderGraph;
		const unsigned int mMaxFramesInFlight = 2;
		const uint64_t mMaxBindlessTextures = 16536;

		virtual void Init() = 0;
		virtual void InitializeRenderGraph(PlazaRenderGraph* renderGraph) = 0;
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

		virtual Mesh& CreateNewMesh(
			vector<glm::vec3>& vertices,
			vector<glm::vec3>& normals,
			vector<glm::vec2>& uvs,
			vector<glm::vec3>& tangent,
			vector<unsigned int>& indices,
			vector<unsigned int>& materialsIndices,
			bool usingNormal,
			vector<BonesHolder> boneIds = vector<BonesHolder>(),
			vector<Bone>  uniqueBonesInfo = vector<Bone>()) = 0;
		virtual void DeleteMesh(Mesh& mesh) = 0;
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

		static glm::vec3 ReconstructWorldPositionFromDepth(glm::vec2 screenPosition, glm::vec2 screenSize, float depth, Camera* camera) {
			glm::mat4 viewProjection = camera->GetProjectionMatrix() * camera->GetViewMatrix();
			float ndcZ = depth * 2.0f - 1.0f;
			glm::vec4 ndcPosition = glm::vec4(screenPosition * (1.0f / screenSize) * 2.0f - 1.0f, depth, 1.0f);
			ndcPosition.y *= -1.0f;
			glm::vec4 worldPosition = glm::inverse(viewProjection) * ndcPosition;
			return glm::vec3(worldPosition) / worldPosition.w;
			/*
mat4 viewProjection = pushConstants.projection * pushConstants.view;
float depth = (texture(gSceneDepth, TexCoords).r);
float ndcZ = (depth) * 2.0f - 1.0f;
 vec4 ndcPosition = vec4(gl_FragCoord.xy * (1.0f / screenSize) * 2.0 - 1.0, (depth), 1.0);
 ndcPosition.y *= -1;
 vec4 worldPosition = inverse(viewProjection) * ndcPosition;
 worldPosition.xyz /= worldPosition.w;
vec3 FragPos = worldPosition.xyz;
			  */
		}

		virtual void CopyLastFramebufferToFinalDrawBuffer() = 0;
	};
}