#include "Engine/Core/PreCompiledHeaders.h"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_vulkan.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"

namespace Plaza {
	void VulkanRenderGraph::BuildDefaultRenderGraph() {
		PlImageUsage inImageUsageFlags = static_cast<PlImageUsage>(PL_IMAGE_USAGE_COLOR_ATTACHMENT | PL_IMAGE_USAGE_SAMPLED | PL_IMAGE_USAGE_TRANSFER_DST | PL_IMAGE_USAGE_TRANSFER_SRC);
		PlImageUsage outImageUsageFlags = static_cast<PlImageUsage>(PL_IMAGE_USAGE_TRANSFER_DST | PL_IMAGE_USAGE_TRANSFER_SRC | PL_IMAGE_USAGE_SAMPLED | PL_IMAGE_USAGE_COLOR_ATTACHMENT);
		PlImageUsage depthTextureUsageFlags = static_cast<PlImageUsage>(PL_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT | PL_IMAGE_USAGE_SAMPLED | PL_IMAGE_USAGE_TRANSFER_DST | PL_IMAGE_USAGE_TRANSFER_SRC);
		PlImageUsage equirectangularImageUsage = static_cast<PlImageUsage>(PL_IMAGE_USAGE_COLOR_ATTACHMENT | PL_IMAGE_USAGE_SAMPLED | PL_IMAGE_USAGE_TRANSFER_DST | PL_IMAGE_USAGE_TRANSFER_SRC);
		const unsigned int bufferCount = Application->mRenderer->mMaxFramesInFlight;
		const unsigned int shadowMapResolution = 2048;
		const unsigned int skyboxResolution = 512;
		const unsigned int irradianceSize = 64;
		const unsigned int brdfSize = 512;

		this->AddTexture(make_shared<VulkanTexture>(VulkanRenderer::GetRenderer()->mMaxBindlessTextures, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(1, 1, 1), 1, 1, "TexturesBuffer"));
		this->AddTexture(make_shared<VulkanTexture>(1, equirectangularImageUsage, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(skyboxResolution, skyboxResolution, 1), 1, 1, "EquirectangularTexture"));
		this->AddTexture(make_shared<VulkanTexture>(1, equirectangularImageUsage, PL_TYPE_2D, PL_VIEW_TYPE_CUBE, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(skyboxResolution, skyboxResolution, 1), 1, 6, "CubeMapTexture"));
		this->AddTexture(make_shared<VulkanTexture>(1, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R16G16_SFLOAT, glm::vec3(brdfSize, brdfSize, 1), 1, 1, "SamplerBRDFLUT"));
		this->AddTexture(make_shared<VulkanTexture>(1, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_CUBE, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(skyboxResolution, skyboxResolution, 1), 0, 6, "PreFilterMap"));
		this->AddTexture(make_shared<VulkanTexture>(1, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_CUBE, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(irradianceSize, irradianceSize, 1), 1, 6, "IrradianceMap"));
		this->AddTexture(make_shared<VulkanTexture>(1, depthTextureUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D_ARRAY, PL_FORMAT_D32_SFLOAT_S8_UINT, glm::vec3(shadowMapResolution, shadowMapResolution, 1), 1, VulkanRenderer::GetRenderer()->mShadows->mCascadeCount, "ShadowsDepthMap"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "GPosition"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "GDiffuse"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "GNormal"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "GOthers"));
		this->AddTexture(make_shared<VulkanTexture>(1, depthTextureUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_D32_SFLOAT_S8_UINT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "SceneDepth"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "SceneTexture"));

		this->GetTexture<Texture>("EquirectangularTexture")->mPath = Application->enginePath + "\\Editor\\DefaultAssets\\Skybox\\" + "autumn_field_4k.hdr";
		this->GetTexture<Texture>("EquirectangularTexture")->mIsHdr = true;

		struct ShadowPassUBO {
			glm::mat4 lightSpaceMatrices[32];
		};
		this->AddBuffer(std::make_shared<PlVkBuffer>(PL_BUFFER_UNIFORM_BUFFER, 1, sizeof(ShadowPassUBO), bufferCount, PL_BUFFER_USAGE_UNIFORM_BUFFER, PL_MEMORY_USAGE_CPU_TO_GPU, "ShadowPassUBO"));
		this->AddBuffer(std::make_shared<PlVkBuffer>(PL_BUFFER_UNIFORM_BUFFER, 1, sizeof(VulkanRenderer::UniformBufferObject), bufferCount, PL_BUFFER_USAGE_UNIFORM_BUFFER, PL_MEMORY_USAGE_CPU_TO_GPU, "GPassUBO"));
		this->AddBuffer(std::make_shared<PlVkBuffer>(PL_BUFFER_STORAGE_BUFFER, 1024 * 16, sizeof(glm::mat4), bufferCount, PL_BUFFER_USAGE_STORAGE_BUFFER, PL_MEMORY_USAGE_CPU_TO_GPU, "BoneMatricesBuffer"));
		this->AddBuffer(std::make_shared<PlVkBuffer>(PL_BUFFER_STORAGE_BUFFER, 1024 * 16, sizeof(VulkanRenderer::MaterialData), bufferCount, PL_BUFFER_USAGE_STORAGE_BUFFER, PL_MEMORY_USAGE_CPU_TO_GPU, "MaterialsBuffer"));
		this->AddBuffer(std::make_shared<PlVkBuffer>(PL_BUFFER_STORAGE_BUFFER, 1024 * 256 * 64, sizeof(unsigned int), bufferCount, PL_BUFFER_USAGE_STORAGE_BUFFER, PL_MEMORY_USAGE_CPU_TO_GPU, "RenderGroupOffsetsBuffer"));
		this->AddBuffer(std::make_shared<PlVkBuffer>(PL_BUFFER_STORAGE_BUFFER, 1024 * 256 * 64, sizeof(unsigned int), bufferCount, PL_BUFFER_USAGE_STORAGE_BUFFER, PL_MEMORY_USAGE_CPU_TO_GPU, "RenderGroupMaterialsOffsetsBuffer"));
		this->AddBuffer(std::make_shared<PlVkBuffer>(PL_BUFFER_STORAGE_BUFFER, 1024 * 32, sizeof(Lighting::LightStruct), bufferCount, static_cast<PlBufferUsage>(PL_BUFFER_USAGE_STORAGE_BUFFER | PL_BUFFER_USAGE_TRANSFER_DST), PL_MEMORY_USAGE_CPU_TO_GPU, "LightsBuffer"));
		this->AddBuffer(std::make_shared<PlVkBuffer>(PL_BUFFER_STORAGE_BUFFER, 1024 * 32, sizeof(Lighting::Tile), bufferCount, static_cast<PlBufferUsage>(PL_BUFFER_USAGE_STORAGE_BUFFER | PL_BUFFER_USAGE_TRANSFER_DST), PL_MEMORY_USAGE_CPU_TO_GPU, "ClustersBuffer"));

		this->AddRenderPass(std::make_shared<VulkanRenderPass>("Shadow Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_INDIRECT_BUFFER_SHADOW_MAP, glm::vec2(shadowMapResolution, shadowMapResolution), false))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 0, PlBufferType::PL_BUFFER_UNIFORM_BUFFER, PL_STAGE_VERTEX, this->GetSharedBuffer("ShadowPassUBO")))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 1, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, this->GetSharedBuffer("BoneMatricesBuffer")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, this->GetSharedTexture("ShadowsDepthMap")));

		this->GetRenderPass("Shadow Pass")->mMultiViewCount = VulkanRenderer::GetRenderer()->mShadows->mCascadeCount;
		this->GetRenderPass("Shadow Pass")->AddPipeline(pl::pipelineCreateInfo(
			"ShadowMapping",
			PL_RENDER_INDIRECT_BUFFER_SHADOW_MAP,
			{ pl::pipelineShaderStageCreateInfo(PL_STAGE_VERTEX, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\shadows\\cascadedShadowDepthShaders.vert"), "main"),
				pl::pipelineShaderStageCreateInfo(PL_STAGE_FRAGMENT, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\shadows\\cascadedShadowDepthShaders.frag"), "main") },
			{ pl::vertexInputBindingDescription(0, sizeof(Vertex), PL_VERTEX_INPUT_RATE_VERTEX), pl::vertexInputBindingDescription(1, sizeof(glm::vec4) * 4, PL_VERTEX_INPUT_RATE_INSTANCE) },
			{ pl::vertexInputAttributeDescription(0, 0, PL_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)),
			pl::vertexInputAttributeDescription(5, 1, PL_FORMAT_R32G32B32A32_SFLOAT, 0),
			pl::vertexInputAttributeDescription(6, 1, PL_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 4),
			pl::vertexInputAttributeDescription(7, 1, PL_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 8),
			pl::vertexInputAttributeDescription(8, 1, PL_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 12) },
			PL_TOPOLOGY_TRIANGLE_LIST,
			false,
			pl::pipelineRasterizationStateCreateInfo(false, false, PL_POLYGON_MODE_FILL, 1.0f, false, 0.0f, 0.0f, 0.0f, PL_CULL_MODE_NONE, PL_FRONT_FACE_COUNTER_CLOCKWISE),
			pl::pipelineColorBlendStateCreateInfo({ pl::pipelineColorBlendAttachmentState(true) }),
			pl::pipelineDepthStencilStateCreateInfo(true, true, PL_COMPARE_OP_LESS),
			pl::pipelineViewportStateCreateInfo(1, 1),
			pl::pipelineMultisampleStateCreateInfo(PL_SAMPLE_COUNT_1_BIT, 0),
			{ PL_DYNAMIC_STATE_VIEWPORT, PL_DYNAMIC_STATE_SCISSOR },
			{}
		));

		this->AddRenderPassCallback("Shadow Pass", [&](PlazaRenderGraph* plazaRenderGraph, PlazaRenderPass* plazaRenderPass) {
			static ShadowPassUBO ubo{};
			for (unsigned int i = 0; i < VulkanRenderer::GetRenderer()->mShadows->mCascades.size(); ++i) {
				ubo.lightSpaceMatrices[i] = VulkanRenderer::GetRenderer()->mShadows->mUbo[Application->mRenderer->mCurrentFrame].lightSpaceMatrices[i];
			}
			for (unsigned int i = VulkanRenderer::GetRenderer()->mShadows->mCascades.size(); i < 32; ++i) {
				ubo.lightSpaceMatrices[i] = glm::mat4(1.0f);
			}
			plazaRenderGraph->GetSharedBuffer("ShadowPassUBO")->UpdateData<ShadowPassUBO>(Application->mRenderer->mCurrentFrame, ubo);
			});


		glm::vec2 gPassSize = Application->appSizes->sceneSize;
		this->AddRenderPass(std::make_shared<VulkanRenderPass>("Deferred Geometry Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_INDIRECT_BUFFER, glm::vec2(gPassSize.x, gPassSize.y), true))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 0, PlBufferType::PL_BUFFER_UNIFORM_BUFFER, PL_STAGE_ALL, this->GetSharedBuffer("GPassUBO")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(VulkanRenderer::GetRenderer()->mMaxBindlessTextures, 0, 20, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("TexturesBuffer")))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 19, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, this->GetSharedBuffer("MaterialsBuffer")))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 1, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, this->GetSharedBuffer("BoneMatricesBuffer")))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 2, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, this->GetSharedBuffer("RenderGroupOffsetsBuffer")))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 3, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, this->GetSharedBuffer("RenderGroupMaterialsOffsetsBuffer")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 6, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("SamplerBRDFLUT")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 7, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("PreFilterMap")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 8, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("IrradianceMap")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 9, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, this->GetSharedTexture("ShadowsDepthMap")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("GPosition")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 1, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("GNormal")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 2, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("GDiffuse")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 3, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("GOthers")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 4, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, this->GetSharedTexture("SceneDepth")));

		this->GetRenderPass("Deferred Geometry Pass")->AddPipeline(pl::pipelineCreateInfo(
			"MainShaders",
			PL_RENDER_INDIRECT_BUFFER,
			{ pl::pipelineShaderStageCreateInfo(PL_STAGE_VERTEX, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\deferred\\geometryPass.vert"), "main"),
				pl::pipelineShaderStageCreateInfo(PL_STAGE_FRAGMENT, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\deferred\\geometryPass.frag"), "main") },
			VertexGetBindingDescription(),
			VertexGetAttributeDescriptions(),
			PL_TOPOLOGY_TRIANGLE_LIST,
			false,
			pl::pipelineRasterizationStateCreateInfo(false, false, PL_POLYGON_MODE_FILL, 1.0f, false, 0.0f, 0.0f, 0.0f, PL_CULL_MODE_BACK, PL_FRONT_FACE_COUNTER_CLOCKWISE),
			pl::pipelineColorBlendStateCreateInfo({ pl::pipelineColorBlendAttachmentState(true) , pl::pipelineColorBlendAttachmentState(true)  ,pl::pipelineColorBlendAttachmentState(true)  ,pl::pipelineColorBlendAttachmentState(true) }),
			pl::pipelineDepthStencilStateCreateInfo(true, true, PL_COMPARE_OP_LESS_OR_EQUAL),
			pl::pipelineViewportStateCreateInfo(1, 1),
			pl::pipelineMultisampleStateCreateInfo(PL_SAMPLE_COUNT_1_BIT, 0),
			{ PL_DYNAMIC_STATE_VIEWPORT, PL_DYNAMIC_STATE_SCISSOR },
			{}
		));

		struct DeferredGeometrySkyboxPC {
			glm::mat4 projection;
			glm::mat4 view;
			float skyboxIntensity;
			float gamma;
			float exposure;
		};
		this->GetRenderPass("Deferred Geometry Pass")->AddPipeline(pl::pipelineCreateInfo(
			"Skybox",
			PL_RENDER_INDIRECT_BUFFER_SPECIFIC_MESH,
			{ pl::pipelineShaderStageCreateInfo(PL_STAGE_VERTEX, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\skybox.vert"), "main"),
				pl::pipelineShaderStageCreateInfo(PL_STAGE_FRAGMENT, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\skybox.frag"), "main") },
			VertexGetBindingDescription(),
			VertexGetAttributeDescriptions(),
			PL_TOPOLOGY_TRIANGLE_LIST,
			false,
			pl::pipelineRasterizationStateCreateInfo(false, false, PL_POLYGON_MODE_FILL, 1.0f, false, 0.0f, 0.0f, 0.0f, PL_CULL_MODE_NONE, PL_FRONT_FACE_COUNTER_CLOCKWISE),
			pl::pipelineColorBlendStateCreateInfo({ pl::pipelineColorBlendAttachmentState(true) , pl::pipelineColorBlendAttachmentState(true)  ,pl::pipelineColorBlendAttachmentState(true)  ,pl::pipelineColorBlendAttachmentState(true) }),
			pl::pipelineDepthStencilStateCreateInfo(true, false, PL_COMPARE_OP_LESS_OR_EQUAL),
			pl::pipelineViewportStateCreateInfo(1, 1),
			pl::pipelineMultisampleStateCreateInfo(PL_SAMPLE_COUNT_1_BIT, 0),
			{ PL_DYNAMIC_STATE_VIEWPORT, PL_DYNAMIC_STATE_SCISSOR },
			{ pl::pushConstantRange(PL_STAGE_ALL, 0, sizeof(DeferredGeometrySkyboxPC)) },
			{ 1 }
		));

		this->AddRenderPassCallback("Deferred Geometry Pass", [&](PlazaRenderGraph* plazaRenderGraph, PlazaRenderPass* plazaRenderPass) {
			static VulkanRenderer::UniformBufferObject ubo{};
			ubo.projection = Application->activeCamera->GetProjectionMatrix();
			ubo.view = Application->activeCamera->GetViewMatrix();
			ubo.model = glm::mat4(1.0f);

			ubo.cascadeCount = 9;
			ubo.farPlane = 15000.0f;
			ubo.nearPlane = 0.01f;

			glm::vec3 lightDir = VulkanRenderer::GetRenderer()->mShadows->mLightDirection;
			glm::vec3 lightDistance = glm::vec3(100.0f, 400.0f, 0.0f);
			glm::vec3 lightPos;

			ubo.lightDirection = glm::vec4(lightDir, 1.0f);
			ubo.viewPos = glm::vec4(Application->activeCamera->Position, 1.0f);

			ubo.directionalLightColor = glm::vec4(VulkanRenderer::GetRenderer()->mLighting->directionalLightColor * VulkanRenderer::GetRenderer()->mLighting->directionalLightIntensity);
			ubo.directionalLightColor.a = 1.0f;
			ubo.ambientLightColor = glm::vec4(VulkanRenderer::GetRenderer()->mLighting->ambientLightColor * VulkanRenderer::GetRenderer()->mLighting->ambientLightIntensity);
			ubo.ambientLightColor.a = 1.0f;
			ubo.gamma = VulkanRenderer::GetRenderer()->gamma;

			for (int i = 0; i < VulkanRenderer::GetRenderer()->mShadows->mCascades.size(); ++i) {
				ubo.lightSpaceMatrices[i] = VulkanRenderer::GetRenderer()->mShadows->mUbo[VulkanRenderer::GetRenderer()->mCurrentFrame].lightSpaceMatrices[i];
				if (i <= 8)
					ubo.cascadePlaneDistances[i] = glm::vec4(VulkanRenderer::GetRenderer()->mShadows->shadowCascadeLevels[i], 1.0f, 1.0f, 1.0f);
				else
					ubo.cascadePlaneDistances[i] = glm::vec4(VulkanRenderer::GetRenderer()->mShadows->shadowCascadeLevels[8], 1.0f, 1.0f, 1.0f);
			}

			ubo.showCascadeLevels = Application->showCascadeLevels;
			plazaRenderGraph->GetSharedBuffer("GPassUBO")->UpdateData<VulkanRenderer::UniformBufferObject>(Application->mRenderer->mCurrentFrame, ubo);

			plazaRenderPass->mPipelines[1]->UpdatePushConstants<DeferredGeometrySkyboxPC>(0, DeferredGeometrySkyboxPC(Application->activeCamera->GetProjectionMatrix(), Application->activeCamera->GetViewMatrix(),
				VulkanRenderer::GetRenderer()->mSkyboxIntensity, VulkanRenderer::GetRenderer()->gamma, VulkanRenderer::GetRenderer()->exposure));
			});

		this->GetRenderPass("Deferred Geometry Pass")->GetInputResource<PlazaShadersBinding>("TexturesBuffer")->mMaxBindlessResources = VulkanRenderer::GetRenderer()->mMaxBindlessTextures;


		this->AddRenderPass(std::make_shared<VulkanRenderPass>("Deferred Lighting Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_FULL_SCREEN_QUAD, gPassSize, false))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GPosition")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 1, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GNormal")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 2, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GDiffuse")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 3, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GOthers")))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 4, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, this->GetSharedBuffer("LightsBuffer")))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 5, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, this->GetSharedBuffer("ClustersBuffer")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("SceneTexture")));

		struct DeferredLightingPassConstants {
			glm::vec3 viewPos;
			float time;
			glm::mat4 view;
			glm::mat4 projection;
			int lightCount;
			glm::vec4 ambientLightColor;
		};
		this->GetRenderPass("Deferred Lighting Pass")->AddPipeline(pl::pipelineCreateInfo(
			"LightingPassShaders",
			PL_RENDER_FULL_SCREEN_QUAD,
			{ pl::pipelineShaderStageCreateInfo(PL_STAGE_VERTEX, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\lighting\\deferredPass.vert"), "main"),
				pl::pipelineShaderStageCreateInfo(PL_STAGE_FRAGMENT, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\lighting\\deferredPass.frag"), "main") },
			VertexGetBindingDescription(),
			VertexGetAttributeDescriptions(),
			PL_TOPOLOGY_TRIANGLE_LIST,
			false,
			pl::pipelineRasterizationStateCreateInfo(false, false, PL_POLYGON_MODE_FILL, 1.0f, false, 0.0f, 0.0f, 0.0f, PL_CULL_MODE_NONE, PL_FRONT_FACE_COUNTER_CLOCKWISE),
			pl::pipelineColorBlendStateCreateInfo({ pl::pipelineColorBlendAttachmentState(true) }),
			pl::pipelineDepthStencilStateCreateInfo(false, false, PL_COMPARE_OP_ALWAYS),
			pl::pipelineViewportStateCreateInfo(1, 1),
			pl::pipelineMultisampleStateCreateInfo(PL_SAMPLE_COUNT_1_BIT, 0),
			{ PL_DYNAMIC_STATE_VIEWPORT, PL_DYNAMIC_STATE_SCISSOR },
			{ pl::pushConstantRange(PL_STAGE_FRAGMENT, 0, sizeof(DeferredLightingPassConstants)) }
		));

		this->AddRenderPassCallback("Deferred Lighting Pass", [&](PlazaRenderGraph* plazaRenderGraph, PlazaRenderPass* plazaRenderPass) {
			plazaRenderPass->mPipelines[0]->UpdatePushConstants<DeferredLightingPassConstants>(0, DeferredLightingPassConstants(Application->activeCamera->Position, 0.0f, Application->activeCamera->GetViewMatrix(),
				Application->activeCamera->GetProjectionMatrix(), VulkanRenderer::GetRenderer()->mLighting->mLights.size(), VulkanRenderer::GetRenderer()->mLighting->ambientLightColor * VulkanRenderer::GetRenderer()->mLighting->ambientLightIntensity));
			});

		this->OrderPasses();
	}

	VulkanRenderGraph* VulkanRenderGraph::BuildSkyboxRenderGraph() {
		VulkanRenderGraph* skyboxRenderGraph = new VulkanRenderGraph();

		const unsigned int faceSize = 512;
		const unsigned int brdfSize = 512;
		const unsigned int irradianceSize = 64;

		static EquirectangularToCubeMapPC pushConstants{};

		PlPipelineCreateInfo pipelineCreateInfo = pl::pipelineCreateInfo(
			"EquirectangularToCubeMapShaders",
			PL_RENDER_CUBE,
			{ pl::pipelineShaderStageCreateInfo(PL_STAGE_VERTEX, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\equirectangularToCubemap.vert"), "main"),
				pl::pipelineShaderStageCreateInfo(PL_STAGE_FRAGMENT, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\equirectangularToCubemap.frag"), "main") },
			{ },
			{ },
			PL_TOPOLOGY_TRIANGLE_LIST,
			false,
			pl::pipelineRasterizationStateCreateInfo(false, false, PL_POLYGON_MODE_FILL, 1.0f, false, 0.0f, 0.0f, 0.0f, PL_CULL_MODE_NONE, PL_FRONT_FACE_COUNTER_CLOCKWISE),
			pl::pipelineColorBlendStateCreateInfo({ pl::pipelineColorBlendAttachmentState(true) }),
			pl::pipelineDepthStencilStateCreateInfo(false, false, PL_COMPARE_OP_ALWAYS),
			pl::pipelineViewportStateCreateInfo(1, 1),
			pl::pipelineMultisampleStateCreateInfo(PL_SAMPLE_COUNT_1_BIT, 0),
			{ PL_DYNAMIC_STATE_VIEWPORT, PL_DYNAMIC_STATE_SCISSOR },
			{ pl::pushConstantRange(PL_STAGE_ALL, 0, sizeof(EquirectangularToCubeMapPC)) }
		);

		skyboxRenderGraph->AddRenderPass(std::make_shared<VulkanRenderPass>("EquirectangularToCubeMapPass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_CUBE, glm::vec2(faceSize, faceSize), true))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 1, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("EquirectangularTexture")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, this->GetSharedTexture("CubeMapTexture")))
			->AddPipeline(pipelineCreateInfo);

		pipelineCreateInfo.pipelineName = "BrdfGeneratorShaders";
		pipelineCreateInfo.renderMethod = PL_RENDER_FULL_SCREEN_QUAD;
		pipelineCreateInfo.shaderStages = {
		pl::pipelineShaderStageCreateInfo(PL_STAGE_VERTEX, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\brdfGenerator.vert"), "main"),
		pl::pipelineShaderStageCreateInfo(PL_STAGE_FRAGMENT, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\brdfGenerator.frag"), "main") };
		pipelineCreateInfo.pushConstants = {};
		skyboxRenderGraph->AddRenderPass(std::make_shared<VulkanRenderPass>("BrdfGeneratorPass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_FULL_SCREEN_QUAD, glm::vec2(brdfSize, brdfSize), true))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, this->GetSharedTexture("SamplerBRDFLUT")))
			->AddPipeline(pipelineCreateInfo);

		pipelineCreateInfo.pipelineName = "IrradianceGeneratorShaders";
		pipelineCreateInfo.renderMethod = PL_RENDER_CUBE;
		pipelineCreateInfo.shaderStages = {
		pl::pipelineShaderStageCreateInfo(PL_STAGE_VERTEX, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\equirectangularToCubemap.vert"), "main"),
		pl::pipelineShaderStageCreateInfo(PL_STAGE_FRAGMENT, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\irradianceGenerator.frag"), "main") };
		pipelineCreateInfo.pushConstants = { pl::pushConstantRange(PL_STAGE_ALL, 0, sizeof(EquirectangularToCubeMapPC)) };
		skyboxRenderGraph->AddRenderPass(std::make_shared<VulkanRenderPass>("IrradianceGeneratorPass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_CUBE, glm::vec2(irradianceSize, irradianceSize), false))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 2, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("CubeMapTexture")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, this->GetSharedTexture("IrradianceMap")))
			->AddPipeline(pipelineCreateInfo);

		pipelineCreateInfo.pipelineName = "PreFilteredGeneratorShaders";
		pipelineCreateInfo.renderMethod = PL_RENDER_CUBE;
		pipelineCreateInfo.shaderStages = {
		pl::pipelineShaderStageCreateInfo(PL_STAGE_VERTEX, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\equirectangularToCubemap.vert"), "main"),
		pl::pipelineShaderStageCreateInfo(PL_STAGE_FRAGMENT, VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\prefilterEnvGenerator.frag"), "main") };
		pipelineCreateInfo.pushConstants = { pl::pushConstantRange(PL_STAGE_ALL, 0, sizeof(EquirectangularToCubeMapPC)) };
		skyboxRenderGraph->AddRenderPass(std::make_shared<VulkanRenderPass>("PreFilteredGeneratorPass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_CUBE, glm::vec2(faceSize, faceSize), false))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("CubeMapTexture")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, this->GetSharedTexture("PreFilterMap")))
			->AddPipeline(pipelineCreateInfo);

		return skyboxRenderGraph;
	}

	void VulkanRenderGraph::RunSkyboxRenderGraph(VulkanRenderGraph* renderGraph) {
		renderGraph->mCompiledBindings = mCompiledBindings;
		renderGraph->Compile();

		const unsigned int faceSize = 512;
		const unsigned int irradianceSize = 64;
		const unsigned int numMips = this->GetSharedTexture("PreFilterMap")->mMipCount;
		PlTextureFormat skyboxFormat = PL_FORMAT_R32G32B32A32_SFLOAT;

		const std::vector<glm::mat4> matrices = {
			glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		};

		const std::vector<glm::mat4> equirectangularToCubeMapMatrices = {
			glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		};

		EquirectangularToCubeMapPC converterPushConstants{};
		converterPushConstants.first = true;

		VkCommandBuffer commandBuffer = VulkanRenderer::GetRenderer()->BeginSingleTimeCommands();

		renderGraph->GetRenderPass("EquirectangularToCubeMapPass")->mPipelines[0]->mPushConstants[0].mData = new EquirectangularToCubeMapPC();;
		for (uint32_t i = 0; i < 6; i++) {
			VkImageViewCreateInfo layerImageViewCreateInfo = {};
			layerImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			layerImageViewCreateInfo.image = this->GetTexture<VulkanTexture>("CubeMapTexture")->mImage;//mSkyboxTexture->mImage;
			layerImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			layerImageViewCreateInfo.format = PlImageFormatToVkFormat(skyboxFormat);
			layerImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			layerImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			layerImageViewCreateInfo.subresourceRange.levelCount = 1;
			layerImageViewCreateInfo.subresourceRange.baseArrayLayer = i;
			layerImageViewCreateInfo.subresourceRange.layerCount = 1;

			VkImageView layerImageView;
			vkCreateImageView(VulkanRenderer::GetRenderer()->mDevice, &layerImageViewCreateInfo, nullptr, &layerImageView);

			std::vector<VkImageView> frameBufferAttachments{ layerImageView };

			VkFramebuffer framebuffer = VulkanRenderer::GetRenderer()->CreateFramebuffer(renderGraph->GetRenderPass("EquirectangularToCubeMapPass")->mRenderPass,
				glm::vec2(faceSize, faceSize), frameBufferAttachments.data(), frameBufferAttachments.size(), 1);

			converterPushConstants.mvp = glm::perspective((float)(glm::pi<double>() / 2.0), 1.0f, 0.1f, static_cast<float>(faceSize)) * equirectangularToCubeMapMatrices[i];
			renderGraph->GetRenderPass("EquirectangularToCubeMapPass")->mFrameBuffer = framebuffer;
			renderGraph->GetRenderPass("EquirectangularToCubeMapPass")->UpdateCommandBuffer(commandBuffer);
			renderGraph->GetRenderPass("EquirectangularToCubeMapPass")->mPipelines[0]->mPushConstants[0].mData = &converterPushConstants;
			memcpy(renderGraph->GetRenderPass("EquirectangularToCubeMapPass")->mPipelines[0]->mPushConstants[0].mData, &converterPushConstants, sizeof(EquirectangularToCubeMapPC));
			renderGraph->GetRenderPass("EquirectangularToCubeMapPass")->Execute(renderGraph);
		}

		VulkanRenderer::GetRenderer()->EndSingleTimeCommands(commandBuffer);

		commandBuffer = VulkanRenderer::GetRenderer()->BeginSingleTimeCommands();
		renderGraph->GetRenderPass("BrdfGeneratorPass")->UpdateCommandBuffer(commandBuffer);
		renderGraph->GetRenderPass("BrdfGeneratorPass")->Execute(renderGraph);
		VulkanRenderer::GetRenderer()->EndSingleTimeCommands(commandBuffer);

		commandBuffer = VulkanRenderer::GetRenderer()->BeginSingleTimeCommands();
		renderGraph->GetRenderPass("IrradianceGeneratorPass")->mPipelines[0]->mPushConstants[0].mData = new EquirectangularToCubeMapPC();
		converterPushConstants.first = false;
		converterPushConstants.deltaPhi = (2.0f * float(3.14159265358979323846)) / 180.0f;
		converterPushConstants.deltaTheta = (0.5f * float(3.14159265358979323846)) / 64.0f;
		for (uint32_t i = 0; i < 6; i++) {
			VkImageViewCreateInfo layerImageViewCreateInfo = {};
			layerImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			layerImageViewCreateInfo.image = this->GetTexture<VulkanTexture>("IrradianceMap")->mImage;//mSkyboxTexture->mImage;
			layerImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			layerImageViewCreateInfo.format = PlImageFormatToVkFormat(skyboxFormat);
			layerImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			layerImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			layerImageViewCreateInfo.subresourceRange.levelCount = 1;
			layerImageViewCreateInfo.subresourceRange.baseArrayLayer = i;
			layerImageViewCreateInfo.subresourceRange.layerCount = 1;

			VkImageView layerImageView;
			vkCreateImageView(VulkanRenderer::GetRenderer()->mDevice, &layerImageViewCreateInfo, nullptr, &layerImageView);

			std::vector<VkImageView> frameBufferAttachments{ layerImageView };

			VkFramebuffer framebuffer = VulkanRenderer::GetRenderer()->CreateFramebuffer(renderGraph->GetRenderPass("IrradianceGeneratorPass")->mRenderPass,
				glm::vec2(irradianceSize, irradianceSize), frameBufferAttachments.data(), frameBufferAttachments.size(), 1);

			converterPushConstants.mvp = glm::perspective((float)(glm::pi<double>() / 2.0), 1.0f, 0.1f, static_cast<float>(faceSize)) * matrices[i];
			renderGraph->GetRenderPass("IrradianceGeneratorPass")->mFrameBuffer = framebuffer;
			renderGraph->GetRenderPass("IrradianceGeneratorPass")->UpdateCommandBuffer(commandBuffer);
			renderGraph->GetRenderPass("IrradianceGeneratorPass")->mPipelines[0]->mPushConstants[0].mData = &converterPushConstants;
			memcpy(renderGraph->GetRenderPass("IrradianceGeneratorPass")->mPipelines[0]->mPushConstants[0].mData, &converterPushConstants, sizeof(EquirectangularToCubeMapPC));
			renderGraph->GetRenderPass("IrradianceGeneratorPass")->Execute(renderGraph);
		}
		VulkanRenderer::GetRenderer()->EndSingleTimeCommands(commandBuffer);

		commandBuffer = VulkanRenderer::GetRenderer()->BeginSingleTimeCommands();
		renderGraph->GetRenderPass("PreFilteredGeneratorPass")->mPipelines[0]->mPushConstants[0].mData = new EquirectangularToCubeMapPC();
		converterPushConstants.first = false;
		converterPushConstants.deltaPhi = (2.0f * float(3.14159265358979323846)) / 180.0f;
		converterPushConstants.deltaTheta = (0.5f * float(3.14159265358979323846)) / 64.0f;
		for (uint32_t m = 0; m < numMips; m++) {
			converterPushConstants.roughness = (float)m / (float)(numMips - 1);
			for (uint32_t i = 0; i < 6; i++) {
				glm::vec2 currentSize = glm::vec2(faceSize * std::pow(0.5f, m), faceSize * std::pow(0.5f, m));
				VkImageViewCreateInfo layerImageViewCreateInfo = {};
				layerImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				layerImageViewCreateInfo.image = this->GetTexture<VulkanTexture>("PreFilterMap")->mImage;//mSkyboxTexture->mImage;
				layerImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				layerImageViewCreateInfo.format = PlImageFormatToVkFormat(skyboxFormat);
				layerImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				layerImageViewCreateInfo.subresourceRange.baseMipLevel = m;
				layerImageViewCreateInfo.subresourceRange.levelCount = 1;
				layerImageViewCreateInfo.subresourceRange.baseArrayLayer = i;
				layerImageViewCreateInfo.subresourceRange.layerCount = 1;

				VkImageView layerImageView;
				vkCreateImageView(VulkanRenderer::GetRenderer()->mDevice, &layerImageViewCreateInfo, nullptr, &layerImageView);

				std::vector<VkImageView> frameBufferAttachments{ layerImageView };

				VkFramebuffer framebuffer = VulkanRenderer::GetRenderer()->CreateFramebuffer(renderGraph->GetRenderPass("PreFilteredGeneratorPass")->mRenderPass,
					currentSize, frameBufferAttachments.data(), frameBufferAttachments.size(), 1);

				converterPushConstants.mvp = glm::perspective((float)(glm::pi<double>() / 2.0), 1.0f, 0.1f, static_cast<float>(faceSize)) * matrices[i];
				renderGraph->GetRenderPass("PreFilteredGeneratorPass")->mFrameBuffer = framebuffer;
				renderGraph->GetRenderPass("PreFilteredGeneratorPass")->UpdateCommandBuffer(commandBuffer);
				renderGraph->GetRenderPass("PreFilteredGeneratorPass")->mPipelines[0]->mPushConstants[0].mData = &converterPushConstants;
				memcpy(renderGraph->GetRenderPass("PreFilteredGeneratorPass")->mPipelines[0]->mPushConstants[0].mData, &converterPushConstants, sizeof(EquirectangularToCubeMapPC));
				renderGraph->GetRenderPass("PreFilteredGeneratorPass")->mRenderSize = currentSize;
				renderGraph->GetRenderPass("PreFilteredGeneratorPass")->Execute(renderGraph);
			}
		}
		VulkanRenderer::GetRenderer()->EndSingleTimeCommands(commandBuffer);
	}

	void VulkanRenderPass::CompilePipeline(PlPipelineCreateInfo createInfo) {
		std::shared_ptr<VulkanPlazaPipeline> pipeline = std::make_shared<VulkanPlazaPipeline>();
		pipeline->mRenderPass = this->mRenderPass;
		pipeline->mFramebuffer = this->mFrameBuffer;
		pipeline->mShaders->mDescriptorSetLayout = mDescriptorSetLayout;
		pipeline->mShaders->mDescriptorSets = mDescriptorSets;

		std::vector<VkPushConstantRange> pushConstants{};
		for (PlPushConstantRange range : createInfo.pushConstants) {
			pushConstants.push_back(plvk::pushConstantRange(PlRenderStageToVkShaderStage(range.stageFlags), range.offset, range.size));
			pipeline->mPushConstants.push_back(PlPushConstants(range.stageFlags, range.offset, range.size));
		}
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = plvk::pipelineLayoutCreateInfo(1, &pipeline->mShaders->mDescriptorSetLayout, pushConstants.size(), pushConstants.data());

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
		for (size_t i = 0; i < createInfo.shaderStages.size(); ++i) {
			const PlPipelineShaderStageCreateInfo& stage = createInfo.shaderStages[i];

			VkPipelineShaderStageCreateInfo shaderStage = {};
			shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStage.stage = static_cast<VkShaderStageFlagBits>(PlRenderStageToVkShaderStage(stage.stage));
			shaderStage.pName = createInfo.shaderStages[i].name.c_str();

			auto shaderCode = VulkanShaders::ReadFile(stage.shadersPath.c_str());
			shaderStage.module = VulkanShaders::CreateShaderModule(shaderCode, VulkanRenderer::GetRenderer()->mDevice);

			shaderStages.push_back(shaderStage);
		}

		std::vector<VkVertexInputBindingDescription> bindings = std::vector<VkVertexInputBindingDescription>();

		for (const PlVertexInputBindingDescription& description : createInfo.vertexBindingDescriptions) {
			VkVertexInputBindingDescription binding{};
			binding.binding = description.binding;
			binding.stride = description.stride;
			binding.inputRate = PlVertexInputRateToVkVertexInputRate(description.inputRate);
			bindings.push_back(binding);
		}

		std::vector<VkVertexInputAttributeDescription> attributes = std::vector<VkVertexInputAttributeDescription>();
		for (const PlVertexInputAttributeDescription& description : createInfo.vertexAttributeDescriptions) {
			VkVertexInputAttributeDescription attribute{};
			attribute.location = description.location;
			attribute.binding = description.binding;
			attribute.format = PlImageFormatToVkFormat(description.format);
			attribute.offset = description.offset;
			attributes.push_back(attribute);
		}
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = plvk::pipelineVertexInputStateCreateInfo(bindings, attributes);
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = plvk::pipelineInputAssemblyStateCreateInfo(PlTopologyToVkTopology(createInfo.topology), createInfo.primitiveRestartEnable);
		VkPipelineRasterizationStateCreateInfo rasterizationState = plvk::pipelineRasterizationStateCreateInfo(createInfo.rasterization.depthClampEnable, createInfo.rasterization.rasterizerDiscardEnable, PlPolygonModeToVkPolygonMode(createInfo.rasterization.polygonMode), createInfo.rasterization.lineWidth, createInfo.rasterization.depthBiasEnable, createInfo.rasterization.depthBiasConstantFactor, createInfo.rasterization.depthBiasClamp, createInfo.rasterization.depthBiasSlopeFactor, PlCullModeToVkCullMode(createInfo.rasterization.cullMode), PlFrontFaceToVkFrontFace(createInfo.rasterization.frontFace));
		std::vector<VkPipelineColorBlendAttachmentState> blendAttachments{};
		for (const PlPipelineColorBlendAttachmentState& attachment : createInfo.colorBlendState.attachments) {
			blendAttachments.push_back(plvk::pipelineColorBlendAttachmentState(attachment.blendEnable,
				PlBlendFactorToVkBlendFactor(attachment.srcColorBlendFactor),
				PlBlendFactorToVkBlendFactor(attachment.dstColorBlendFactor),
				PlBlendOpToVkBlendOp(attachment.colorBlendOp),
				PlBlendFactorToVkBlendFactor(attachment.srcAlphaBlendFactor),
				PlBlendFactorToVkBlendFactor(attachment.dstAlphaBlendFactor),
				PlBlendOpToVkBlendOp(attachment.alphaBlendOp),
				attachment.colorWriteMask));
		}
		VkPipelineColorBlendStateCreateInfo colorBlendState = plvk::pipelineColorBlendStateCreateInfo(blendAttachments.size(), blendAttachments.data(), createInfo.colorBlendState.logicOpEnable, PlLogicOpToVkLogicOp(createInfo.colorBlendState.logicOp), std::vector<float>(std::begin(createInfo.colorBlendState.blendConstants), std::end(createInfo.colorBlendState.blendConstants)));
		VkPipelineDepthStencilStateCreateInfo depthStencilState = plvk::pipelineDepthStencilStateCreateInfo(createInfo.depthStencilState.depthTestEnable, createInfo.depthStencilState.depthWriteEnable, PlCompareOpToVkCompareOp(createInfo.depthStencilState.depthCompareOp), createInfo.depthStencilState.depthBoundsTestEnable, createInfo.depthStencilState.stencilTestEnable);
		VkPipelineViewportStateCreateInfo viewportState = plvk::pipelineViewportStateCreateInfo(createInfo.viewPortState.viewportCount, createInfo.viewPortState.scissorsCount);
		VkPipelineMultisampleStateCreateInfo multisampleState = plvk::pipelineMultisampleStateCreateInfo(PlSampleCountToVkSampleCount(createInfo.multiSampleState.rasterizationSamples), createInfo.multiSampleState.sampleShadingEnable);
		std::vector<VkDynamicState> dynamicStates{};
		for (PlDynamicState state : createInfo.dynamicStates) {
			dynamicStates.push_back(PlDynamicStateToVkDynamicState(state));
		}
		VkPipelineDynamicStateCreateInfo dynamicState = plvk::pipelineDynamicStateCreateInfo(dynamicStates);
		pipeline->mShaders->InitializeFull(
			VulkanRenderer::GetRenderer()->mDevice,
			pipelineLayoutCreateInfo,
			createInfo.vertexAttributeDescriptions.size() > 0 | createInfo.vertexBindingDescriptions.size() > 0 ? true : false,
			mRenderSize.x,
			mRenderSize.y,
			shaderStages,
			vertexInputInfo,
			inputAssemblyState,
			viewportState,
			rasterizationState,
			multisampleState,
			colorBlendState,
			dynamicState,
			mRenderPass,
			depthStencilState
		);
		pipeline->mCreateInfo = createInfo;
		mPipelines.push_back(pipeline);
	}

	void VulkanRenderGraph::CreatePipeline(PlPipelineCreateInfo createInfo) { }

	void VulkanRenderGraph::AddPipeline() { }

	bool compareRenderPasses(const std::shared_ptr<PlazaRenderPass>& a, const std::shared_ptr<PlazaRenderPass>& b) {
		return a->mExecutionIndex < b->mExecutionIndex;
	}

	void OrderPassDependencies(int16_t  currentIndex, std::shared_ptr<PlazaRenderPass> pass, std::set<std::string>& alreadyOrderedPasses) {
		alreadyOrderedPasses.emplace(pass->mName);
		pass->mExecutionIndex = currentIndex;//pass->mExecutionIndex > currentIndex ? pass->mExecutionIndex : currentIndex;
		for (const auto& dependent : pass->mDependents) {
			OrderPassDependencies(currentIndex + 1, dependent.second, alreadyOrderedPasses);
			//dependency.second->mExecutionIndex = currentIndex;
		}
	}

	void VulkanRenderGraph::OrderPasses() {
		std::map<std::string, BindingModifiers> bindingsModifiers = std::map<std::string, BindingModifiers>();

		for (const auto& pass : mOrderedPasses) {
			for (const auto& resource : pass->mInputBindings) {
				if (bindingsModifiers.find(resource->mName) == bindingsModifiers.end())
					bindingsModifiers.emplace(resource->mName, BindingModifiers(resource));
				bindingsModifiers[resource->mName].readPasses.push_back(pass->mName);
			}
			for (const auto& resource : pass->mOutputBindings) {
				if (bindingsModifiers.find(resource->mName) == bindingsModifiers.end())
					bindingsModifiers.emplace(resource->mName, BindingModifiers(resource));
				bindingsModifiers[resource->mName].writePasses.push_back(pass->mName);
			}
		}

		for (int i = mOrderedPasses.size() - 1; i >= 0; i--) {
			for (const auto& resource : mOrderedPasses[i]->mInputBindings) {
				BindingModifiers binding = bindingsModifiers[resource->mName];
				for (const std::string& name : binding.writePasses)
					mOrderedPasses[i]->mDependencies.emplace(name, this->GetSharedRenderPass(name));
			}

			for (const auto& resource : mOrderedPasses[i]->mOutputBindings) {
				BindingModifiers binding = bindingsModifiers[resource->mName];
				for (const std::string& name : binding.readPasses)
					mOrderedPasses[i]->mDependents.emplace(name, this->GetSharedRenderPass(name));
			}
		}

		std::set<std::string> alreadyOrderedPasses = std::set<std::string>();

		for (int i = 0; i < mOrderedPasses.size(); ++i) {
			if (alreadyOrderedPasses.find(mOrderedPasses[i]->mName) != alreadyOrderedPasses.end() || mOrderedPasses[i]->mDependents.size() == 0)
				continue;
			mOrderedPasses[i]->mExecutionIndex = 0;
			OrderPassDependencies(0, mOrderedPasses[mOrderedPasses[i]->mExecutionIndex], alreadyOrderedPasses);
		}

		std::sort(mOrderedPasses.begin(), mOrderedPasses.end(), compareRenderPasses);
		for (unsigned int i = 0; i < mOrderedPasses.size(); ++i) {
			mOrderedPasses[i]->mExecutionIndex = i;
		}
	}

	void VulkanBufferBinding::Compile() {
		//mBuffer = std::make_shared<PlVkBuffer>();
		mBuffer->CreateBuffer(mBuffer->mMaxItems * mBuffer->mStride, PlBufferUsageToVkBufferUsage(mBuffer->mBufferUsage), PlMemoryUsageToVmaMemoryUsage(mBuffer->mMemoryUsage), 0, mBuffer->mBufferCount);
		mBuffer->CreateMemory(0, mBuffer->mBufferCount);
	}
	void VulkanBufferBinding::Destroy() {

	}

	void VulkanTextureBinding::Compile() {
		if (mMaxBindlessResources > 0)
			return;
		//mTexture = std::make_shared<VulkanTexture>();
		VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		if (this->GetTexture()->mImageUsage & PL_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT)
			aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

		VkImageUsageFlags flags = 0;
		if (mTexture->mViewType == PL_VIEW_TYPE_CUBE)
			flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		if (mTexture->mPath == "") {
			this->GetTexture()->CreateTextureImage(VulkanRenderer::GetRenderer()->mDevice, PlImageFormatToVkFormat(mTexture->mFormat), mTexture->mResolution.x, mTexture->mResolution.y, this->GetTexture()->mMipCount == 0 ? true : false,
				PlImageUsageToVkImageUsage(mTexture->mImageUsage), PlTextureTypeToVkImageType(mTexture->mTextureType), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, mTexture->mLayersCount, flags, false, VK_SHARING_MODE_EXCLUSIVE);
			VulkanRenderer::GetRenderer()->TransitionTextureLayout(*this->GetTexture(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, aspect, this->GetTexture()->mLayersCount, this->GetTexture()->mMipCount);
		}
		else
			this->GetTexture()->CreateTextureImage(VulkanRenderer::GetRenderer()->mDevice, mTexture->mPath, PlImageFormatToVkFormat(mTexture->mFormat), true, mTexture->mIsHdr, PlImageUsageToVkImageUsage(mTexture->mImageUsage));


		this->GetTexture()->CreateTextureSampler();

		this->GetTexture()->CreateImageView(PlImageFormatToVkFormat(mTexture->mFormat), aspect, PlViewTypeToVkImageViewType(mTexture->mViewType), mTexture->mLayersCount, 0);

		VulkanRenderer::GetRenderer()->AddTrackerToImage(this->GetTexture()->mImageView, mTexture->mName, this->GetTexture()->mSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	void VulkanTextureBinding::Destroy() {

	}

	void VulkanRenderPass::Compile(PlazaRenderGraph* renderGraph) {
		glm::vec2 biggestSize = glm::vec2(0.0f);
		std::vector<VkImageView> frameBufferAttachments{};
		// Render Pass
		std::vector<VkSubpassDescription> subPasses{};
		std::vector<VkSubpassDependency> dependencies{};
		std::vector<VkAttachmentDescription> attachmentDescs{};
		std::vector<uint32_t> locations{};
		std::vector<VkAttachmentReference> colorReferences;
		VkAttachmentReference depthReference = {};

		for (const auto& value : mOutputBindings) {
			VulkanTextureBinding* binding = static_cast<VulkanTextureBinding*>(value.get());
			biggestSize = glm::max(biggestSize, glm::vec2(binding->mTexture->mResolution));

			VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			if (binding->mInitialLayout == PL_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
				finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			//if (value->mName == "SceneDepth")
			//	finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL

			VulkanTextureBinding* currentPassBinding = renderGraph->mOrderedPasses[glm::max(mExecutionIndex - 1, 0)]->GetInputResource<VulkanTextureBinding>(binding->GetTexture()->mName);
			VkImageLayout currentLayout = mExecutionIndex == 0 || currentPassBinding == nullptr ? VK_IMAGE_LAYOUT_UNDEFINED : PlImageLayoutToVkImageLayout(currentPassBinding->mInitialLayout); //PlImageLayoutToVkImageLayout(binding->GetTexture()->mFutureLayouts[mExecutionIndex - 1]);
			VulkanTextureBinding* nextPassBinding = mExecutionIndex + 1 < renderGraph->mOrderedPasses.size() ? renderGraph->mOrderedPasses[mExecutionIndex + 1]->GetInputResource<VulkanTextureBinding>(binding->GetTexture()->mName) : nullptr;
			VkImageLayout nextLayout = nextPassBinding ? PlImageLayoutToVkImageLayout(nextPassBinding->mInitialLayout) : finalLayout; //PlImageLayoutToVkImageLayout(binding->GetTexture()->mFutureLayouts[mExecutionIndex]);

			VkAttachmentLoadOp loadOp = currentLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
			VkAttachmentStoreOp storeOp = nextLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_STORE;
			VkAttachmentLoadOp loadStencilOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			VkAttachmentStoreOp storeStencilOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachmentDescs.push_back(
				plvk::attachmentDescription(PlImageFormatToVkFormat(binding->mTexture->mFormat),
					VK_SAMPLE_COUNT_1_BIT,
					loadOp,
					storeOp,
					loadStencilOp,
					storeStencilOp,
					currentLayout,
					nextLayout));//finalLayout));//binding->mTexture->GetLayout()));

			frameBufferAttachments.push_back(binding->GetTexture()->mImageView);
			locations.push_back(binding->mLocation);

			VkClearValue clearValue{};
			if (binding->mInitialLayout == PL_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL || binding->mInitialLayout == PL_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
				clearValue.depthStencil = { 1.0f, 0 };
			else
				clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
			mClearValues.push_back(clearValue);

			if (binding->GetTexture()->mImageUsage & PL_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT) {
				depthReference.attachment = binding->mLocation;
				depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				continue;
			}
			colorReferences.push_back({ binding->mLocation, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });

		}

		// Sort the attachments to make location 0 image view be in index 0
		std::vector<VkAttachmentDescription> temporaryAttachmentDescs = attachmentDescs;
		std::vector<VkImageView> temporaryFrameBufferAttachments = frameBufferAttachments;
		for (unsigned int i = 0; i < locations.size(); ++i) {
			attachmentDescs[i] = temporaryAttachmentDescs[locations[i]];
			frameBufferAttachments[i] = temporaryFrameBufferAttachments[locations[i]];
		}

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.pColorAttachments = colorReferences.data();
		subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
		if (depthReference.layout != VK_IMAGE_LAYOUT_UNDEFINED)
			subpass.pDepthStencilAttachment = &depthReference;
		dependencies.push_back(
			plvk::subpassDependency(VK_SUBPASS_EXTERNAL,
				0,
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
				0));
		dependencies.push_back(plvk::subpassDependency(
			VK_SUBPASS_EXTERNAL,
			0,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
			0));
		subPasses.push_back(subpass);

		uint32_t viewMask = 0;
		VkRenderPassMultiviewCreateInfo renderPassMultiviewCI{};
		std::vector<int32_t> viewOffsets(dependencies.size(), 0);
		void* next = nullptr;
		viewMask |= 1u << mMultiViewCount;
		viewMask -= 1;
		renderPassMultiviewCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO;
		renderPassMultiviewCI.subpassCount = 1;
		renderPassMultiviewCI.pViewMasks = &viewMask;
		renderPassMultiviewCI.correlationMaskCount = 0;
		renderPassMultiviewCI.pViewOffsets = viewOffsets.data();
		if (mMultiViewCount > 0) {
			next = &renderPassMultiviewCI;
		}

		mRenderPass = VulkanRenderer::GetRenderer()->CreateRenderPass(attachmentDescs.data(), attachmentDescs.size(), subPasses.data(), subPasses.size(), dependencies.data(), dependencies.size(), next);

		// Frame buffer
		mFrameBuffer = VulkanRenderer::GetRenderer()->CreateFramebuffer(mRenderPass, biggestSize, frameBufferAttachments.data(), frameBufferAttachments.size(), 1);

		// Descriptor sets
		std::vector<VkDescriptorSetLayoutBinding> descriptorSets{};

		VkDescriptorBindingFlags bindlessFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
		std::vector<VkDescriptorBindingFlagsEXT> bindingFlags = std::vector<VkDescriptorBindingFlagsEXT>();
		for (const auto& value : mInputBindings) {
			switch (value->mBindingType) {
			case PL_BINDING_BUFFER:
				descriptorSets.push_back(static_cast<VulkanBufferBinding*>(value.get())->GetDescriptorLayout());
				bindingFlags.push_back(value->mMaxBindlessResources > 0 ? bindlessFlags : 0);
				break;
			case PL_BINDING_TEXTURE:
				descriptorSets.push_back(static_cast<VulkanTextureBinding*>(value.get())->GetDescriptorLayout());
				bindingFlags.push_back(value->mMaxBindlessResources > 0 ? bindlessFlags : 0);
				break;
			}
		}

		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT,	nullptr };
		extendedInfo.pBindingFlags = bindingFlags.data();
		extendedInfo.bindingCount = static_cast<uint32_t>(descriptorSets.size());

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = plvk::descriptorSetLayoutCreateInfo(descriptorSets, VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT, &extendedInfo);

		vkCreateDescriptorSetLayout(VulkanRenderer::GetRenderer()->mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorSetLayout);

		std::vector<VkDescriptorSetLayout> layouts(Application->mRenderer->mMaxFramesInFlight,
			mDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO
		};
		allocInfo.descriptorPool = VulkanRenderer::GetRenderer()->mDescriptorPool;
		allocInfo.descriptorSetCount = layouts.size();
		allocInfo.pSetLayouts = layouts.data();

		VkDescriptorSetVariableDescriptorCountAllocateInfoEXT countInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT
		};
		static const uint32_t maxBindlessResources = 16536;
		std::vector<uint32_t> maxBinding(Application->mRenderer->mMaxFramesInFlight,
			Application->mRenderer->mMaxBindlessTextures - 1);
		countInfo.descriptorSetCount = Application->mRenderer->mMaxFramesInFlight;
		countInfo.pDescriptorCounts = maxBinding.data();
		//allocInfo.pNext = &countInfo;
		mDescriptorSets.resize(Application->mRenderer->mMaxFramesInFlight);
		VkResult res = vkAllocateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, &allocInfo, mDescriptorSets.data());
		if (res != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
		for (unsigned int i = 0; i < Application->mRenderer->mMaxFramesInFlight; ++i) {
			std::vector<VkWriteDescriptorSet> descriptorWrites{};

			std::vector<VkDescriptorBufferInfo*> bufferInfos{};
			std::vector<VkDescriptorImageInfo*> imageInfos{};
			for (const auto& value : mInputBindings) {
				switch (value->mBindingType) {
				case PL_BINDING_BUFFER: {
					VulkanBufferBinding* bufferBinding = static_cast<VulkanBufferBinding*>(value.get());
					VkDescriptorBufferInfo bufferInfo = bufferBinding->GetBufferInfo(i);
					bufferInfos.push_back(new VkDescriptorBufferInfo(bufferInfo));
					descriptorWrites.push_back(bufferBinding->GetDescriptorWrite(mDescriptorSets[i], bufferInfos[bufferInfos.size() - 1]));
					break;
				}
				case PL_BINDING_TEXTURE: {
					VulkanTextureBinding* textureBinding = static_cast<VulkanTextureBinding*>(value.get());
					VkDescriptorImageInfo imageInfo = textureBinding->GetImageInfo();
					if (value->mMaxBindlessResources > 0) {
						imageInfo = plvk::descriptorImageInfo(VK_IMAGE_LAYOUT_UNDEFINED, VK_NULL_HANDLE, VK_NULL_HANDLE);
						break;
					}
					imageInfo.imageLayout = PlImageLayoutToVkImageLayout(textureBinding->mInitialLayout);
					imageInfos.push_back(new VkDescriptorImageInfo(imageInfo));
					descriptorWrites.push_back(textureBinding->GetDescriptorWrite(mDescriptorSets[i], imageInfos[imageInfos.size() - 1]));
					break;
				}
				}
			}

			vkUpdateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice,
				static_cast<uint32_t>(descriptorWrites.size()),
				descriptorWrites.data(),
				0,
				nullptr);
		}

		for (PlPipelineCreateInfo& createInfo : mPipelinesCreateInfo) {
			this->CompilePipeline(createInfo);
		}
	}

	void VulkanRenderPass::BindMainBuffers() {
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, &VulkanRenderer::GetRenderer()->mMainVertexBuffer->GetBuffer(), offsets);
		vkCmdBindVertexBuffers(mCommandBuffer, 1, 1, &VulkanRenderer::GetRenderer()->mMainInstanceMatrixBuffers[VulkanRenderer::GetRenderer()->mCurrentFrame], offsets);
		vkCmdBindIndexBuffer(mCommandBuffer, VulkanRenderer::GetRenderer()->mMainIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanRenderPass::BindRenderPass() {
		VkRenderPassBeginInfo renderPassInfo = plvk::renderPassBeginInfo(this->mRenderPass, this->mFrameBuffer,
			mRenderSize.x, mRenderSize.y, 0, 0, mClearValues.size(), mClearValues.data());
		vkCmdBeginRenderPass(mCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = plvk::viewport(0.0f, mFlipViewPort ? mRenderSize.y : 0.0f, mRenderSize.x, mFlipViewPort ? -static_cast<float>(mRenderSize.y) : mRenderSize.y);
		vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);
		VkRect2D scissor = plvk::rect2D(0, 0, mRenderSize.x, mRenderSize.y);
		vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);
	}

	void VulkanRenderPass::EndRenderPass() {
		vkCmdEndRenderPass(mCommandBuffer);
	}

	bool VulkanRenderGraph::BindPass(std::string passName) {
		if (mPasses.find(passName) == mPasses.end())
			return false;
		VulkanRenderPass* renderPass = this->GetRenderPass(passName);

		renderPass->BindRenderPass();
	}

	void VulkanRenderGraph::Execute(uint8_t imageIndex, uint8_t currentFrame) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		VkCommandBuffer commandBuffer = *mCommandBuffer;


		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		VulkanRenderer::GetRenderer()->mActiveCommandBuffer = &commandBuffer;
		//VulkanRenderer::GetRenderer()->TransitionImageLayout(VulkanRenderer::GetRenderer()->mSwapChainImages[currentFrame], VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		VulkanRenderer::GetRenderer()->UpdateInstancesData();

		for (const auto& value : mOrderedPasses) {
			this->GetRenderPass(value->mName)->UpdateCommandBuffer(commandBuffer);
			this->GetRenderPass(value->mName)->Execute(this);
		}

		/* Render ImGui if in Editor build */
#ifdef EDITOR_MODE
		std::array<VkClearValue, 1> clearValues{};
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		VkRenderPassBeginInfo renderPassInfo = plvk::renderPassBeginInfo(VulkanRenderer::GetRenderer()->mSwapchainRenderPass, VulkanRenderer::GetRenderer()->mSwapChainFramebuffers[imageIndex],
			VulkanRenderer::GetRenderer()->mSwapChainExtent.width, VulkanRenderer::GetRenderer()->mSwapChainExtent.height, 0, 0, 1, clearValues.data());

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		//vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanRenderer::GetRenderer()->mGraphicsPipeline);

		VkViewport viewport = plvk::viewport(0.0f, 0.0f, VulkanRenderer::GetRenderer()->mSwapChainExtent.width, VulkanRenderer::GetRenderer()->mSwapChainExtent.height);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		VkRect2D scissor = plvk::rect2D(0, 0, VulkanRenderer::GetRenderer()->mSwapChainExtent.width, VulkanRenderer::GetRenderer()->mSwapChainExtent.height);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		//vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanRenderer::GetRenderer()->mPipelineLayout, 0, 1, &VulkanRenderer::GetRenderer()->mFinalSceneDescriptorSet, 0, nullptr);

		{
			PLAZA_PROFILE_SECTION("Render ImGui");
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
		}

		vkCmdEndRenderPass(commandBuffer);
#endif

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void VulkanRenderPass::RenderIndirectBuffer(PlazaPipeline* pipeline) {
		VulkanPlazaPipeline* vulkanPipeline = static_cast<VulkanPlazaPipeline*>(pipeline);
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipeline);
		vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipelineLayout, 0, 1, &mDescriptorSets[VulkanRenderer::GetRenderer()->mCurrentFrame], 0, nullptr);
		for (const PlPushConstants& pushConstant : vulkanPipeline->mPushConstants) {
			vkCmdPushConstants(mCommandBuffer, vulkanPipeline->mShaders->mPipelineLayout, PlRenderStageToVkShaderStage(pushConstant.mStage), pushConstant.mOffset, pushConstant.mStride, pushConstant.mData);
		}
		vkCmdDrawIndexedIndirect(mCommandBuffer, VulkanRenderer::GetRenderer()->mIndirectBuffers[VulkanRenderer::GetRenderer()->mCurrentFrame], pipeline->mIndirectBufferOffset, VulkanRenderer::GetRenderer()->mIndirectDrawCount, sizeof(VkDrawIndexedIndirectCommand));
	}

	void VulkanRenderPass::RenderIndirectBufferShadowMap(PlazaPipeline* pipeline) {
		VulkanPlazaPipeline* vulkanPipeline = static_cast<VulkanPlazaPipeline*>(pipeline);
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipeline);
		vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipelineLayout, 0, 1, &mDescriptorSets[VulkanRenderer::GetRenderer()->mCurrentFrame], 0, nullptr);
		for (const PlPushConstants& pushConstant : vulkanPipeline->mPushConstants) {
			vkCmdPushConstants(mCommandBuffer, vulkanPipeline->mShaders->mPipelineLayout, PlRenderStageToVkShaderStage(pushConstant.mStage), pushConstant.mOffset, pushConstant.mStride, pushConstant.mData);
		}
		vkCmdDrawIndexedIndirect(mCommandBuffer, VulkanRenderer::GetRenderer()->mIndirectBuffers[VulkanRenderer::GetRenderer()->mCurrentFrame], pipeline->mIndirectBufferOffset, VulkanRenderer::GetRenderer()->mIndirectDrawCount, sizeof(VkDrawIndexedIndirectCommand));
	}

	void VulkanRenderPass::RenderIndirectBufferSpecificMesh(PlazaPipeline* pipeline) {
		VulkanPlazaPipeline* vulkanPipeline = static_cast<VulkanPlazaPipeline*>(pipeline);
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipeline);
		vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipelineLayout, 0, 1, &mDescriptorSets[VulkanRenderer::GetRenderer()->mCurrentFrame], 0, nullptr);
		for (const PlPushConstants& pushConstant : pipeline->mPushConstants) {
			vkCmdPushConstants(mCommandBuffer, vulkanPipeline->mShaders->mPipelineLayout, PlRenderStageToVkShaderStage(pushConstant.mStage), pushConstant.mOffset, pushConstant.mStride, pushConstant.mData);
		}
		for (auto& meshUuid : pipeline->mCreateInfo.staticMeshesUuid) {
			Mesh* mesh = AssetsManager::GetMesh(meshUuid);
			if (!mesh) continue;
			vkCmdDrawIndexed(mCommandBuffer, static_cast<uint32_t>(mesh->indices.size()), 1, mesh->indicesOffset, mesh->verticesOffset, mesh->instanceOffset);
		}
	}

	void VulkanRenderPass::RenderFullScreenQuad(PlazaPipeline* pipeline) {
		VulkanPlazaPipeline* vulkanPipeline = static_cast<VulkanPlazaPipeline*>(pipeline);
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipeline);
		vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipelineLayout, 0, 1, &mDescriptorSets[VulkanRenderer::GetRenderer()->mCurrentFrame], 0, nullptr);
		for (const PlPushConstants& pushConstant : vulkanPipeline->mPushConstants) {
			vkCmdPushConstants(mCommandBuffer, vulkanPipeline->mShaders->mPipelineLayout, PlRenderStageToVkShaderStage(pushConstant.mStage), pushConstant.mOffset, pushConstant.mStride, pushConstant.mData);
		}
		vkCmdDraw(mCommandBuffer, 3, 1, 0, 0);
	}

	void VulkanRenderPass::RenderCube(PlazaPipeline* pipeline) {
		VulkanPlazaPipeline* vulkanPipeline = static_cast<VulkanPlazaPipeline*>(pipeline);
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipeline);
		vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipelineLayout, 0, 1, &mDescriptorSets[VulkanRenderer::GetRenderer()->mCurrentFrame], 0, nullptr);
		for (const PlPushConstants& pushConstant : vulkanPipeline->mPushConstants) {
			vkCmdPushConstants(mCommandBuffer, vulkanPipeline->mShaders->mPipelineLayout, PlRenderStageToVkShaderStage(pushConstant.mStage), pushConstant.mOffset, pushConstant.mStride, pushConstant.mData);
		}
		vkCmdDraw(mCommandBuffer, 36, 1, 0, 0);
	}
}