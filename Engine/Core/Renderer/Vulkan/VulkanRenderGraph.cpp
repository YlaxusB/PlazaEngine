#include "Engine/Core/PreCompiledHeaders.h"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_vulkan.h"

namespace Plaza {
	void VulkanRenderGraph::BuildDefaultRenderGraph() {
		PlImageUsage inImageUsageFlags = static_cast<PlImageUsage>(PL_IMAGE_USAGE_COLOR_ATTACHMENT | PL_IMAGE_USAGE_SAMPLED | PL_IMAGE_USAGE_TRANSFER_DST | PL_IMAGE_USAGE_TRANSFER_SRC);
		PlImageUsage outImageUsageFlags = static_cast<PlImageUsage>(PL_IMAGE_USAGE_TRANSFER_DST | PL_IMAGE_USAGE_TRANSFER_SRC | PL_IMAGE_USAGE_SAMPLED | PL_IMAGE_USAGE_COLOR_ATTACHMENT);
		PlImageUsage depthTextureUsageFlags = static_cast<PlImageUsage>(PL_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT | PL_IMAGE_USAGE_SAMPLED);
		const unsigned int bufferCount = Application->mRenderer->mMaxFramesInFlight;
		const unsigned int shadowMapResolution = 2048;

		this->AddTexture(make_shared<VulkanTexture>(VulkanRenderer::GetRenderer()->mMaxBindlessTextures, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(1, 1, 1), 1, 1, "TexturesBuffer"));
		this->AddTexture(make_shared<VulkanTexture>(1, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "samplerBRDFLUT"));
		this->AddTexture(make_shared<VulkanTexture>(1, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_CUBE, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(512, 512, 1), 1, 6, "prefilterMap"));
		this->AddTexture(make_shared<VulkanTexture>(1, inImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_CUBE, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(64, 64, 1), 1, 6, "irradianceMap"));
		this->AddTexture(make_shared<VulkanTexture>(1, depthTextureUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D_ARRAY, PL_FORMAT_D32_SFLOAT_S8_UINT, glm::vec3(shadowMapResolution, shadowMapResolution, 1), 1, VulkanRenderer::GetRenderer()->mShadows->mCascadeCount, "ShadowsDepthMap"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "GPosition"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "GDiffuse"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "GNormal"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "GOthers"));
		this->AddTexture(make_shared<VulkanTexture>(1, depthTextureUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_D32_SFLOAT_S8_UINT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "SceneDepth"));
		this->AddTexture(make_shared<VulkanTexture>(1, outImageUsageFlags, PL_TYPE_2D, PL_VIEW_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(Application->appSizes->sceneSize, 1), 1, 1, "SceneTexture"));


		//PlBufferType type, uint64_t maxItems, uint16_t stride, uint8_t bufferCount, PlBufferUsage bufferUsage, PlMemoryUsage memoryUsage, const std::string& name
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

		//uint64_t descriptorCount, PlazaImageUsage imageUsage, PlazaTextureType imageType, PlazaViewType viewType, PlazaTextureFormat format, glm::vec3 resolution, uint8_t mipCount, uint16_t layersCount, const std::string& name

		this->AddRenderPass(std::make_shared<VulkanRenderPass>("Shadow Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_INDIRECT_BUFFER_SHADOW_MAP, glm::vec2(shadowMapResolution, shadowMapResolution), false))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 0, PlBufferType::PL_BUFFER_UNIFORM_BUFFER, PL_STAGE_VERTEX, this->GetSharedBuffer("ShadowPassUBO")))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 1, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, this->GetSharedBuffer("BoneMatricesBuffer")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, this->GetSharedTexture("ShadowsDepthMap")));

		/*
		layout(location = 0) in vec3 aPos;
		layout(location = 5) in vec4 instanceMatrix[4];
		attributeDescriptions.push_back(pl::vertexInputAttributeDescription(0, 0, PL_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)));
					attributeDescriptions.push_back(pl::vertexInputAttributeDescription(4, 1, PL_FORMAT_R32G32B32A32_SFLOAT, 0));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(5, 1, PL_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 4));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(6, 1, PL_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 8));
			attributeDescriptions.push_back(pl::vertexInputAttributeDescription(7, 1, PL_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 12));
		*/
		this->GetRenderPass("Shadow Pass")->mMultiViewCount = VulkanRenderer::GetRenderer()->mShadows->mCascadeCount;
		this->GetRenderPass("Shadow Pass")->AddPipeline(pl::pipelineCreateInfo(
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
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 6, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("samplerBRDFLUT")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 7, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("prefilterMap")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 8, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("irradianceMap")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 9, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, this->GetSharedTexture("ShadowsDepthMap")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("GPosition")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 1, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("GNormal")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 2, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("GDiffuse")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 3, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("GOthers")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 4, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, this->GetSharedTexture("SceneDepth")));

		this->GetRenderPass("Deferred Geometry Pass")->AddPipeline(pl::pipelineCreateInfo(
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
			});


		//this->GetRenderPass("Deferred Geometry Pass")->AddPipeline();


		//uint64_t descriptorCount, uint8_t location, uint8_t binding, PlazaBufferType bufferType, PlazaRenderStage renderStage, PlazaImageLayout initialLayout,

		//this->GetRenderPass("Deferred Geometry Pass")->GetInputResource<VulkanBufferBinding>("UniformBufferObject")
		//	->SetBufferUsage<VulkanBufferBinding>(PL_BUFFER_USAGE_UNIFORM_BUFFER)
		//	->SetMemoryUsage<VulkanBufferBinding>(PL_MEMORY_USAGE_CPU_TO_GPU);
		//this->GetRenderPass("Deferred Geometry Pass")->GetInputResource<VulkanBufferBinding>("materialsBuffer")
		//	->SetBufferUsage<VulkanBufferBinding>(PL_BUFFER_USAGE_STORAGE_BUFFER)
		//	->SetMemoryUsage<VulkanBufferBinding>(PL_MEMORY_USAGE_CPU_TO_GPU);
		//this->GetRenderPass("Deferred Geometry Pass")->GetInputResource<VulkanBufferBinding>("renderGroupOffsetsBuffer")
		//	->SetBufferUsage<VulkanBufferBinding>(PL_BUFFER_USAGE_STORAGE_BUFFER)
		//	->SetMemoryUsage<VulkanBufferBinding>(PL_MEMORY_USAGE_CPU_TO_GPU);
		//this->GetRenderPass("Deferred Geometry Pass")->GetInputResource<VulkanBufferBinding>("renderGroupMaterialsOffsetsBuffer")
		//	->SetBufferUsage<VulkanBufferBinding>(PL_BUFFER_USAGE_STORAGE_BUFFER)
		//	->SetMemoryUsage<VulkanBufferBinding>(PL_MEMORY_USAGE_CPU_TO_GPU);
		//this->GetRenderPass("Deferred Geometry Pass")->GetInputResource<VulkanBufferBinding>("boneMatrices")
		//	->SetBufferUsage<VulkanBufferBinding>(PL_BUFFER_USAGE_STORAGE_BUFFER)
		//	->SetMemoryUsage<VulkanBufferBinding>(PL_MEMORY_USAGE_CPU_TO_GPU);

		// --------------- TEMPORARY
		//std::shared_ptr<VulkanTextureBinding> preFilterMap = std::make_shared<VulkanTextureBinding>(1, 0, 7, PL_BUFFER_COMBINED_IMAGE_SAMPLER, static_cast<PlazaImageUsage>(inImageUsageFlags), PL_STAGE_FRAGMENT, PL_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(512, 512, 1), 1, 1, "prefilterMap");
		//glm::vec2 size = glm::vec2(512, 512);
		//
		//preFilterMap->mTexture = std::make_shared<VulkanTexture>();
		//preFilterMap->GetTexture()->CreateTextureImage(VulkanRenderer::GetRenderer()->mDevice, VK_FORMAT_R32G32B32A32_SFLOAT, size.x, size.y, false, PlImageUsageToVkImageUsage(static_cast<PlazaImageUsage>(inImageUsageFlags)),
		//	VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, false, VK_SHARING_MODE_EXCLUSIVE, true);
		//preFilterMap->GetTexture()->CreateImageView(VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE, 6);
		//preFilterMap->GetTexture()->CreateTextureSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE);

		//std::shared_ptr<VulkanTextureBinding> irradianceMap = std::make_shared<VulkanTextureBinding>(1, 0, 8, PL_BUFFER_COMBINED_IMAGE_SAMPLER, static_cast<PlazaImageUsage>(inImageUsageFlags), PL_STAGE_FRAGMENT, PL_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(64, 64, 1), 1, 1, "irradianceMap");
		//irradianceMap->mTexture = std::make_shared<VulkanTexture>();
		//irradianceMap->GetTexture()->CreateTextureImage(VulkanRenderer::GetRenderer()->mDevice, VK_FORMAT_R32G32B32A32_SFLOAT, 64, 64, false, PlImageUsageToVkImageUsage(static_cast<PlazaImageUsage>(inImageUsageFlags)),
		//	VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, false, VK_SHARING_MODE_EXCLUSIVE, false);
		//irradianceMap->GetTexture()->CreateImageView(VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE, 6);
		//irradianceMap->GetTexture()->CreateTextureSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE);


		//this->GetRenderPass("Deferred Geometry Pass")->AddInputResource(preFilterMap);
		//this->GetRenderPass("Deferred Geometry Pass")->AddInputResource(irradianceMap);
		// --------------- TEMPORARY

		this->GetRenderPass("Deferred Geometry Pass")->GetInputResource<PlazaShadersBinding>("TexturesBuffer")->mMaxBindlessResources = VulkanRenderer::GetRenderer()->mMaxBindlessTextures;

		/*
		layout (std430, binding = 4) buffer LightsBuffer {
	LightStruct lights[];
};

layout (std430, binding = 5) buffer ClusterBuffer {
	Cluster[] clusters;
};
		*/

		//this->AddRenderPass(std::make_shared<VulkanRenderPass>("Mid Second Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_FULL_SCREEN_QUAD))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("MidTexture")))
		//	->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("UselessTexture")));
		//
		//this->AddRenderPass(std::make_shared<VulkanRenderPass>("Biggest Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_FULL_SCREEN_QUAD))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GPosition")))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 1, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("UselessTexture")))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 2, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("DependedTexture")))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 3, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("MidTexture")))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 4, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GNormal")))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 5, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GDiffuse")))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 6, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GOthers")))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 6, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("SceneTexture")))
		//	->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("BigTexture")));
		//
		//this->AddRenderPass(std::make_shared<VulkanRenderPass>("Depends on Both Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_FULL_SCREEN_QUAD))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GPosition")))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 1, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("MidTexture")))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 2, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("UselessTexture")))
		//	->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("DependedTexture")));
		//
		//this->AddRenderPass(std::make_shared<VulkanRenderPass>("Mid First Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_FULL_SCREEN_QUAD))
		//	->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GPosition")))
		//	->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("MidTexture")));

		this->AddRenderPass(std::make_shared<VulkanRenderPass>("Deferred Lighting Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT, PL_RENDER_FULL_SCREEN_QUAD, gPassSize, false))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GPosition")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 1, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GNormal")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 2, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GDiffuse")))
			->AddInputResource(std::make_shared<VulkanTextureBinding>(1, 0, 3, PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->GetSharedTexture("GOthers")))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 4, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, this->GetSharedBuffer("LightsBuffer")))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 5, PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, this->GetSharedBuffer("ClustersBuffer")))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, PL_STAGE_FRAGMENT, PL_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, this->GetSharedTexture("SceneTexture")));

		//this->GetRenderPass("Deferred Lighting Pass")->GetInputResource<VulkanBufferBinding>("LightsBuffer")
		//	->SetBufferUsage<VulkanBufferBinding>(PL_BUFFER_USAGE_STORAGE_BUFFER | PL_BUFFER_USAGE_TRANSFER_DST)
		//	->SetMemoryUsage<VulkanBufferBinding>(PL_MEMORY_USAGE_CPU_TO_GPU);
		//this->GetRenderPass("Deferred Lighting Pass")->GetInputResource<VulkanBufferBinding>("ClusterBuffer")
		//	->SetBufferUsage<VulkanBufferBinding>(PL_BUFFER_USAGE_STORAGE_BUFFER | PL_BUFFER_USAGE_TRANSFER_DST)
		//	->SetMemoryUsage<VulkanBufferBinding>(PL_MEMORY_USAGE_CPU_TO_GPU);


		struct DeferredLightingPassConstants {
			glm::vec3 viewPos;
			float time;
			glm::mat4 view;
			glm::mat4 projection;
			int lightCount;
			glm::vec4 ambientLightColor;
		};
		this->GetRenderPass("Deferred Lighting Pass")->AddPipeline(pl::pipelineCreateInfo(
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
		//->AddOutputResource(std::make_shared<VulkanTextureBinding>(1, 0, 0, PL_BUFFER_SAMPLER, static_cast<PlazaImageUsage>(inImageUsageFlags), PL_STAGE_FRAGMENT, PL_TYPE_2D, PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1, "SceneTexture"));

		RunSkyboxRenderGraph(BuildSkyboxRenderGraph());
	}

	VulkanRenderGraph* VulkanRenderGraph::BuildSkyboxRenderGraph() {
		VulkanRenderGraph* skyboxRenderGraph = new VulkanRenderGraph();



		return skyboxRenderGraph;
	}

	void VulkanRenderGraph::RunSkyboxRenderGraph(VulkanRenderGraph* renderGraph) {

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

		auto bindingsArray = PlazaRenderGraph::VertexGetBindingDescription();
		auto attributesArray = PlazaRenderGraph::VertexGetAttributeDescriptions();
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
			true,
			Application->appSizes->sceneSize.x,
			Application->appSizes->sceneSize.y,
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
		mPipelines.push_back(pipeline);
	}

	void VulkanRenderGraph::CreatePipeline(PlPipelineCreateInfo createInfo) {

	}

	void VulkanRenderGraph::AddPipeline() {

	}

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

		//for (int i = mOrderedPasses.size() - 1; i >= 0; i--) {
		//	int16_t currentIndex = i;
		//	if (alreadyOrderedPasses.find(mOrderedPasses[i]->mName) != alreadyOrderedPasses.end())
		//		continue;
		//	mOrderedPasses[currentIndex]->mExecutionIndex = i == mOrderedPasses.size() - 1 ? i : mOrderedPasses[i + 1]->mExecutionIndex - 1;
		//	OrderPassDependencies(mOrderedPasses[currentIndex]->mExecutionIndex, mOrderedPasses[mOrderedPasses[currentIndex]->mExecutionIndex], alreadyOrderedPasses);
		//}
		for (int i = 0; i < mOrderedPasses.size(); ++i) {
			if (alreadyOrderedPasses.find(mOrderedPasses[i]->mName) != alreadyOrderedPasses.end() || mOrderedPasses[i]->mDependents.size() == 0)
				continue;
			mOrderedPasses[i]->mExecutionIndex = 0; //== mOrderedPasses.size() - 1 ? i : mOrderedPasses[i + 1]->mExecutionIndex - 1;
			OrderPassDependencies(0, mOrderedPasses[mOrderedPasses[i]->mExecutionIndex], alreadyOrderedPasses);
		}

		//std::sort(mOrderedPasses.begin(), mOrderedPasses.end(), compareByAge);

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
		this->GetTexture()->CreateTextureImage(VulkanRenderer::GetRenderer()->mDevice, PlImageFormatToVkFormat(mTexture->mFormat), mTexture->mResolution.x, mTexture->mResolution.y,
			false, PlImageUsageToVkImageUsage(mTexture->mImageUsage), PlTextureTypeToVkImageType(mTexture->mTextureType), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, mTexture->mLayersCount, flags, false);
		VulkanRenderer::GetRenderer()->TransitionTextureLayout(*this->GetTexture(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, aspect, this->GetTexture()->mLayersCount, this->GetTexture()->mMipCount);


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

		for (PlPipelineCreateInfo createInfo : mPipelinesCreateInfo) {
			this->CompilePipeline(createInfo);
		}
	}

	void VulkanRenderPass::BindRenderPass() {
		VkRenderPassBeginInfo renderPassInfo = plvk::renderPassBeginInfo(this->mRenderPass, this->mFrameBuffer,
			mRenderSize.x, mRenderSize.y, 0, 0, mClearValues.size(), mClearValues.data());
		vkCmdBeginRenderPass(mCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
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

	void VulkanRenderPass::RenderIndirectBuffer(PlazaRenderGraph* plazaRenderGraph) {
		VulkanRenderGraph* renderGraph = static_cast<VulkanRenderGraph*>(plazaRenderGraph);
		VkCommandBuffer commandBuffer = *VulkanRenderer::GetRenderer()->mActiveCommandBuffer;

		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &VulkanRenderer::GetRenderer()->mMainVertexBuffer->GetBuffer(), offsets);
		vkCmdBindVertexBuffers(commandBuffer, 1, 1, &VulkanRenderer::GetRenderer()->mMainInstanceMatrixBuffers[VulkanRenderer::GetRenderer()->mCurrentFrame], offsets);
		vkCmdBindIndexBuffer(commandBuffer, VulkanRenderer::GetRenderer()->mMainIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		renderGraph->BindPass(this->mName);

		VkViewport viewport = plvk::viewport(0.0f, mFlipViewPort ? mRenderSize.y : 0.0f, mRenderSize.x, mFlipViewPort ? -static_cast<float>(mRenderSize.y) : mRenderSize.y);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		VkRect2D scissor = plvk::rect2D(0, 0, mRenderSize.x, mRenderSize.y);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		for (const auto& pipeline : mPipelines) {
			VulkanPlazaPipeline* vulkanPipeline = static_cast<VulkanPlazaPipeline*>(pipeline.get());
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipeline);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipelineLayout, 0, 1, &mDescriptorSets[VulkanRenderer::GetRenderer()->mCurrentFrame], 0, nullptr);
			vkCmdDrawIndexedIndirect(commandBuffer, VulkanRenderer::GetRenderer()->mIndirectBuffers[VulkanRenderer::GetRenderer()->mCurrentFrame], 0, VulkanRenderer::GetRenderer()->mIndirectDrawCount, sizeof(VkDrawIndexedIndirectCommand));
		}

		vkCmdEndRenderPass(commandBuffer);
	}

	void VulkanRenderPass::RenderIndirectBufferShadowMap(PlazaRenderGraph* plazaRenderGraph) {
		VulkanRenderGraph* renderGraph = static_cast<VulkanRenderGraph*>(plazaRenderGraph);
		VkCommandBuffer commandBuffer = *VulkanRenderer::GetRenderer()->mActiveCommandBuffer;

		renderGraph->BindPass(this->mName);
		//std::array<VkClearValue, 1> clearValues{};
		//clearValues[0].depthStencil = { 1.0f, 0 };
		//VkRenderPassBeginInfo renderPassInfo = plvk::renderPassBeginInfo(this->mRenderPass, this->mFrameBuffer,
		//	Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 0, 0, clearValues.size(), clearValues.data());
		//vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &VulkanRenderer::GetRenderer()->mMainVertexBuffer->GetBuffer(), offsets);
		vkCmdBindVertexBuffers(commandBuffer, 1, 1, &VulkanRenderer::GetRenderer()->mMainInstanceMatrixBuffers[VulkanRenderer::GetRenderer()->mCurrentFrame], offsets);
		vkCmdBindIndexBuffer(commandBuffer, VulkanRenderer::GetRenderer()->mMainIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);


		VkViewport viewport = plvk::viewport(0.0f, mFlipViewPort ? mRenderSize.y : 0.0f, mRenderSize.x, mFlipViewPort ? -static_cast<float>(mRenderSize.y) : mRenderSize.y);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		VkRect2D scissor = plvk::rect2D(0, 0, mRenderSize.x, mRenderSize.y);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		for (const auto& pipeline : mPipelines) {
			VulkanPlazaPipeline* vulkanPipeline = static_cast<VulkanPlazaPipeline*>(pipeline.get());
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipeline);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipelineLayout, 0, 1, &mDescriptorSets[VulkanRenderer::GetRenderer()->mCurrentFrame], 0, nullptr);
			vkCmdDrawIndexedIndirect(commandBuffer, VulkanRenderer::GetRenderer()->mIndirectBuffers[VulkanRenderer::GetRenderer()->mCurrentFrame], 0, VulkanRenderer::GetRenderer()->mIndirectDrawCount, sizeof(VkDrawIndexedIndirectCommand));
		}

		vkCmdEndRenderPass(commandBuffer);
	}

	void VulkanRenderPass::RenderFullScreenQuad(PlazaRenderGraph* plazaRenderGraph) {
		VulkanRenderGraph* renderGraph = static_cast<VulkanRenderGraph*>(plazaRenderGraph);
		VkCommandBuffer commandBuffer = *VulkanRenderer::GetRenderer()->mActiveCommandBuffer;

		renderGraph->BindPass(this->mName);

		VkViewport viewport = plvk::viewport(0.0f, mFlipViewPort ? mRenderSize.y : 0.0f, mRenderSize.x, mFlipViewPort ? -static_cast<float>(mRenderSize.y) : mRenderSize.y);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		VkRect2D scissor = plvk::rect2D(0, 0, mRenderSize.x, mRenderSize.y);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		for (const auto& pipeline : mPipelines) {
			VulkanPlazaPipeline* vulkanPipeline = static_cast<VulkanPlazaPipeline*>(pipeline.get());
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipeline);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->mShaders->mPipelineLayout, 0, 1, &mDescriptorSets[VulkanRenderer::GetRenderer()->mCurrentFrame], 0, nullptr);

			for (const PlPushConstants& pushConstant : vulkanPipeline->mPushConstants) {
				vkCmdPushConstants(commandBuffer, vulkanPipeline->mShaders->mPipelineLayout, PlRenderStageToVkShaderStage(pushConstant.mStage), pushConstant.mOffset, pushConstant.mStride, pushConstant.mData);
			}

			vkCmdDraw(commandBuffer, 3, 1, 0, 0);
		}

		vkCmdEndRenderPass(commandBuffer);
	}
}