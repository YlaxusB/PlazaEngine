#include <Engine/Core/PreCompiledHeaders.h>
#include "VulkanSkybox.h"
#include "VulkanPlazaPipeline.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"
#include "VulkanPlazaInitializator.h"

namespace Plaza {
	void VulkanSkybox::InitializeImageSampler() {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(VulkanRenderer::GetRenderer()->mPhysicalDevice, &properties);
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
		if (vkCreateSampler(VulkanRenderer::GetRenderer()->mDevice, &samplerInfo, nullptr, &this->mSkyboxSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	void VulkanSkybox::InitializeImageView() {
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = this->mResolution.x;
		imageInfo.extent.height = this->mResolution.y;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 6;
		imageInfo.format = mSkyboxFormat;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		//imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(VulkanRenderer::GetRenderer()->mDevice, &imageInfo, nullptr, &this->mSkyboxImage) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create depth image!");
		}

		// Allocate memory and bind image
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(VulkanRenderer::GetRenderer()->mDevice, this->mSkyboxImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size * 6;
		allocInfo.memoryTypeIndex = VulkanRenderer::GetRenderer()->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VkDeviceMemory mSkyboxMemory;
		if (vkAllocateMemory(VulkanRenderer::GetRenderer()->mDevice, &allocInfo, nullptr, &mSkyboxMemory) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate depth image memory!");
		}

		vkBindImageMemory(VulkanRenderer::GetRenderer()->mDevice, this->mSkyboxImage, mSkyboxMemory, 0);


		std::vector<stbi_uc*> allPixels = std::vector<stbi_uc*>();
		for (unsigned int i = 0; i < 6; ++i) {
			int texWidth, texHeight, texChannels;
			stbi_uc* pixels = stbi_load(mSkyboxPaths[i].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			VkDeviceSize imageSize = texWidth * texHeight * 4;

			if (!pixels) {
				throw std::runtime_error("failed to load texture image!");
			}

			allPixels.push_back(pixels);
			//stbi_image_free(pixels);
		}

		uint32_t imageSize = 2048 * 2048 * 4;

		VulkanRenderer::GetRenderer()->CreateBuffer(imageSize * 6, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mStagingBuffer, mStagingBufferMemory);

		for (unsigned int i = 0; i < 6; ++i) {

			void* data;
			vkMapMemory(VulkanRenderer::GetRenderer()->mDevice, mStagingBufferMemory, i * (imageSize), imageSize, 0, &data);
			//int texWidth, texHeight, texChannels;
			//stbi_uc* pixel = stbi_load(mSkyboxPaths[i].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			//if (!pixel) {
			//	throw std::runtime_error("failed to load texture image!");
			//}
			memcpy(data, allPixels[i], static_cast<size_t>(imageSize));
			vkUnmapMemory(VulkanRenderer::GetRenderer()->mDevice, mStagingBufferMemory);


		}

		VulkanRenderer::GetRenderer()->TransitionImageLayout(this->mSkyboxImage, this->mSkyboxFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 6);
		VulkanRenderer::GetRenderer()->CopyBufferToImage(mStagingBuffer, this->mSkyboxImage, static_cast<uint32_t>(mResolution.x), static_cast<uint32_t>(mResolution.y), 0, 6);
		VulkanRenderer::GetRenderer()->TransitionImageLayout(this->mSkyboxImage, this->mSkyboxFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 6);


		vkDestroyBuffer(VulkanRenderer::GetRenderer()->mDevice, mStagingBuffer, nullptr);
		vkFreeMemory(VulkanRenderer::GetRenderer()->mDevice, mStagingBufferMemory, nullptr);

		// Transition image layout 
		//renderer.TransitionImageLayout(this->mShadowDepthImage, mDepthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT, 9);
		//renderer.TransitionImageLayout(this->mShadowDepthImage, mDepthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_ASPECT_DEPTH_BIT, 9);

		this->mSkyboxImageViews.resize(Application->mRenderer->mMaxFramesInFlight);
		for (unsigned int i = 0; i < Application->mRenderer->mMaxFramesInFlight; ++i) {
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = this->mSkyboxImage;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
			viewInfo.format = this->mSkyboxFormat;
			viewInfo.subresourceRange = {};
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 6;

			if (vkCreateImageView(VulkanRenderer::GetRenderer()->mDevice, &viewInfo, nullptr, &this->mSkyboxImageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create texture image view!");
			}
		}
		for (unsigned int i = 0; i < 6; ++i) {
			stbi_image_free(allPixels[i]);
		}
	}

	void VulkanSkybox::InitializeDescriptorPool() {
		std::array<VkDescriptorPoolSize, 1> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[0].descriptorCount = 6;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 6;

		if (vkCreateDescriptorPool(VulkanRenderer::GetRenderer()->mDevice, &poolInfo, nullptr, &this->mDescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
	void VulkanSkybox::InitializeDescriptorSets() {
		// Layout
		VkDescriptorSetLayoutBinding skyboxLayoutBinding = plvk::descriptorSetLayoutBinding(5, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nullptr, VK_SHADER_STAGE_FRAGMENT_BIT);

		std::array<VkDescriptorSetLayoutBinding, 1> bindings = { skyboxLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(VulkanRenderer::GetRenderer()->mDevice, &layoutInfo, nullptr, &this->mDescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		// Descriptor set
		VkDescriptorSetAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = this->mDescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &mDescriptorSetLayout;

		this->mDescriptorSets.resize(Application->mRenderer->mMaxFramesInFlight);
		for (unsigned int i = 0; i < Application->mRenderer->mMaxFramesInFlight; ++i) {
			if (vkAllocateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, &allocInfo, &this->mDescriptorSets[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate descriptor sets!");
			}
			std::vector<VkWriteDescriptorSet> descriptorWrites = std::vector<VkWriteDescriptorSet>();

			VkDescriptorImageInfo skyboxInfo = plvk::descriptorImageInfo(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, this->mSkyboxImageViews[i], this->mSkyboxSampler);
			descriptorWrites.push_back(plvk::writeDescriptorSet(mDescriptorSets[i], 5, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &skyboxInfo));

			vkUpdateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	void VulkanSkybox::InitializeRenderPass() {
		//VkAttachmentDescription colorAttachment{};
		//colorAttachment.format = this->mSkyboxFormat;
		//colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		//colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		//colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		//colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		//colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		//colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		//
		//VkAttachmentReference colorAttachmentRef{};
		//colorAttachmentRef.attachment = 0;
		//colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		//
		//VkAttachmentDescription depthAttachment{};
		//depthAttachment.format = VulkanRenderer::GetRenderer()->FindDepthFormat();
		//depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		//depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		//depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		//depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		//
		//VkAttachmentReference depthAttachmentRef{};
		//depthAttachmentRef.attachment = 1;
		//depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		//
		//VkSubpassDescription subpass{};
		//subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		//subpass.colorAttachmentCount = 1;
		//subpass.pColorAttachments = &colorAttachmentRef;
		//subpass.pDepthStencilAttachment = &depthAttachmentRef;
		//
		//VkSubpassDependency dependency{};
		//dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		//dependency.dstSubpass = 0;
		//dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		//dependency.srcAccessMask = 0;
		//dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		//dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		//dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		//dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		//dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		//
		//std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		//VkRenderPassCreateInfo renderPassInfo{};
		//renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		//renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		//renderPassInfo.pAttachments = attachments.data();
		//renderPassInfo.subpassCount = 1;
		//renderPassInfo.pSubpasses = &subpass;
		//renderPassInfo.dependencyCount = 1;
		//renderPassInfo.pDependencies = &dependency;
		//
		//if (vkCreateRenderPass(VulkanRenderer::GetRenderer()->mDevice, &renderPassInfo, nullptr, &this->mSkyboxPostEffect->mRenderPass) != VK_SUCCESS) {
		//	throw std::runtime_error("failed to create render pass!");
		//}
		/* Render pass */
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = VulkanRenderer::GetRenderer()->mLighting->mDeferredEndTextureFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(VulkanRenderer::GetRenderer()->mDevice, &renderPassInfo, nullptr, &this->mSkyboxPostEffect->mRenderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void VulkanSkybox::Init() {
		this->mSkyboxPostEffect = new VulkanPlazaPipeline();


		//this->mSkyboxPostEffect->mRenderPass = VulkanRenderer::GetRenderer()->mDeferredRenderPass;

		std::string shadersPath;
#ifdef EDITOR_MODE
		shadersPath = Application->enginePath + "\\Editor\\DefaultAssets\\Skybox\\oldskybox\\";
#else
		shadersPath = Application->projectPath + "\\";
#endif
		this->mSkyboxPaths[0] = shadersPath + "right.jpg";
		this->mSkyboxPaths[1] = shadersPath + "left.jpg";
		this->mSkyboxPaths[2] = shadersPath + "top.jpg";
		this->mSkyboxPaths[3] = shadersPath + "bottom.jpg";
		this->mSkyboxPaths[4] = shadersPath + "front.jpg";
		this->mSkyboxPaths[5] = shadersPath + "back.jpg";

		this->mResolution = glm::vec2(2048);//Application->appSizes->sceneSize;
		this->InitializeImageSampler();
		this->InitializeImageView();
		this->InitializeDescriptorPool();
		this->InitializeDescriptorSets();
		//this->InitializeRenderPass();

		//this->mFramebuffers.resize(Application->mRenderer->mMaxFramesInFlight);
		//for (unsigned int i = 0; i < Application->mRenderer->mMaxFramesInFlight; ++i) {
		//     VkFramebufferCreateInfo framebufferInfo{};
		//     framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		//     framebufferInfo.renderPass = this->mSkyboxPostEffect->mRenderPass;
		//     framebufferInfo.attachmentCount = 1;
		//     std::array<VkImageView, 1> imageViews{ this->mSkyboxImageViews[i] };
		//
		//     framebufferInfo.pAttachments = imageViews.data();
		//     framebufferInfo.width = this->mResolution.x;
		//     framebufferInfo.height = this->mResolution.y;
		//     framebufferInfo.layers = 1;
		//     if (vkCreateFramebuffer(VulkanRenderer::GetRenderer()->mDevice, &framebufferInfo, nullptr, &this->mFramebuffers[i]) != VK_SUCCESS) {
		//          throw std::runtime_error("Failed to create framebuffer!");
		//     }
		//}

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(VulkanSkybox::PushConstants);

		this->mPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		this->mPipelineLayoutInfo.setLayoutCount = 1;
		this->mPipelineLayoutInfo.pSetLayouts = &this->mDescriptorSetLayout;
		this->mPipelineLayoutInfo.pushConstantRangeCount = 1;
		this->mPipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		VkDescriptorSetLayoutBinding skyboxLayoutBinding = plvk::descriptorSetLayoutBinding(0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nullptr, VK_SHADER_STAGE_FRAGMENT_BIT);

		std::vector<VkDescriptorSetLayoutBinding> bindings = { skyboxLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo = plvk::descriptorSetLayoutCreateInfo(bindings, VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT);
		mSkyboxPostEffect->mRenderPass = VulkanRenderer::GetRenderer()->mGeometryPassRenderer.mRenderPass;
		if (vkCreateDescriptorSetLayout(VulkanRenderer::GetRenderer()->mDevice, &layoutInfo, nullptr, &mSkyboxPostEffect->mShaders->mDescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		//this->mSkyboxPostEffect->mShaders = new VulkanShaders(vertexPath, fragmentPath, "");
		mScreenSize = Application->appSizes->sceneSize;
		std::vector<VkImageView> attachments{ VulkanRenderer::GetRenderer()->mLighting->mDeferredEndTexture.mImageView, VulkanRenderer::GetRenderer()->mDepthImageView };
		mSkyboxPostEffect->InitializeFramebuffer(attachments.data(), attachments.size(), this->mScreenSize, 1);
		//mSkyboxPostEffect->Init(
		//	VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\skybox.vert"),
		//	VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\skybox.frag"),
		//	"",
		//	VulkanRenderer::GetRenderer()->mDevice,
		//	mScreenSize,
		//	mSkyboxPostEffect->mShaders->mDescriptorSetLayout,
		//	this->mPipelineLayoutInfo);

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_FALSE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_NOT_EQUAL;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		this->mSkyboxPostEffect->mShaders->mVertexShaderPath = VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\skybox.vert");
		this->mSkyboxPostEffect->mShaders->mFragmentShaderPath = VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\skybox.frag");
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = plvk::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
		VkPipelineRasterizationStateCreateInfo rasterizationState = plvk::pipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, 1.0f, VK_FALSE, 0.0f, 0.0f, 0.0f, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
		VkPipelineColorBlendAttachmentState blendAttachmentState = plvk::pipelineColorBlendAttachmentState(VK_TRUE);
		std::vector<VkPipelineColorBlendAttachmentState> blendAttachments{ blendAttachmentState,blendAttachmentState,blendAttachmentState,blendAttachmentState };
		VkPipelineColorBlendStateCreateInfo colorBlendState = plvk::pipelineColorBlendStateCreateInfo(4, blendAttachments.data());
		VkPipelineDepthStencilStateCreateInfo depthStencilState = plvk::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS);
		VkPipelineViewportStateCreateInfo viewportState = plvk::pipelineViewportStateCreateInfo(1, 1);
		VkPipelineMultisampleStateCreateInfo multisampleState = plvk::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);
		std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicState = plvk::pipelineDynamicStateCreateInfo(dynamicStateEnables);
		this->mSkyboxPostEffect->mShaders->InitializeFull(VulkanRenderer::GetRenderer()->mDevice, this->mPipelineLayoutInfo, true, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, {},
			{},
			inputAssemblyState,
			viewportState,
			rasterizationState,
			multisampleState,
			colorBlendState,
			dynamicState,
			mSkyboxPostEffect->mRenderPass,
			depthStencilState);
	}

	void VulkanSkybox::DrawSkybox() {
		if (!mSkyboxMesh)
			mSkyboxMesh = (VulkanMesh*)Editor::DefaultModels::Cube();
		this->mCommandBuffer = *VulkanRenderer::GetRenderer()->mActiveCommandBuffer;
		PLAZA_PROFILE_SECTION("Draw Skybox");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		//renderPassInfo.renderPass = this->mSkyboxPostEffect->mRenderPass;
		//renderPassInfo.framebuffer = this->mFramebuffers[Application->mRenderer->mCurrentFrame];//mSwapChainFramebuffers[0];//mSwapChainFramebuffers[imageIndex];
		renderPassInfo.renderPass = mSkyboxPostEffect->mRenderPass;
		renderPassInfo.framebuffer = mSkyboxPostEffect->mFramebuffer;

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = VulkanRenderer::GetRenderer()->mSwapChainExtent;




		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 0.0f} };
		clearValues[1].depthStencil = { 0.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		viewport.width = static_cast<float>(VulkanRenderer::GetRenderer()->mSwapChainExtent.width);
		viewport.height = static_cast<float>(VulkanRenderer::GetRenderer()->mSwapChainExtent.height);
		//viewport.y = this->mResolution.y;

		renderPassInfo.renderArea.extent.width = Application->appSizes->sceneSize.x;
		renderPassInfo.renderArea.extent.height = Application->appSizes->sceneSize.y;

		viewport.width = Application->appSizes->sceneSize.x;
		viewport.height = -Application->appSizes->sceneSize.y;
		viewport.y = Application->appSizes->sceneSize.y;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = VulkanRenderer::GetRenderer()->mSwapChainExtent;
		scissor.extent.width = Application->appSizes->sceneSize.x;
		scissor.extent.height = Application->appSizes->sceneSize.y;

		//vkCmdBeginRenderPass(this->mCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		//vkCmdSetViewport(this->mCommandBuffer, 0, 1, &viewport);
		//vkCmdSetScissor(this->mCommandBuffer, 0, 1, &scissor);

		vkCmdBindPipeline(this->mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->mSkyboxPostEffect->mShaders->mPipeline);

		//this->UpdateAndPushConstants(mCommandBuffer);
		vkCmdBindDescriptorSets(this->mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->mSkyboxPostEffect->mShaders->mPipelineLayout, 0, 1, &this->mDescriptorSets[Application->mRenderer->mCurrentFrame], 0, nullptr);

		VkDeviceSize offsets[] = { 0, 0 };

		pushData.skyboxIntensity = VulkanRenderer::GetRenderer()->mSkyboxIntensity;
		pushData.gamma = VulkanRenderer::GetRenderer()->gamma;
		pushData.exposure = VulkanRenderer::GetRenderer()->exposure;
		pushData.projection = Application->activeCamera->GetProjectionMatrix();
		pushData.view = Application->activeCamera->GetViewMatrix();
		vkCmdPushConstants(this->mCommandBuffer, this->mSkyboxPostEffect->mShaders->mPipelineLayout, VK_SHADER_STAGE_ALL, 0, sizeof(VulkanSkybox::PushConstants), &pushData);

		//VkDeviceSize offsets2[1] = { 0 };
		//vkCmdBindIndexBuffer(mCommandBuffer, VulkanRenderer::GetRenderer()->mMainIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		//vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, &VulkanRenderer::GetRenderer()->mMainVertexBuffer, offsets2);
		//vkCmdBindVertexBuffers(mCommandBuffer, 1, 1, &VulkanRenderer::GetRenderer()->mMainInstanceMatrixBuffers[VulkanRenderer::GetRenderer()->mCurrentFrame], offsets2);

		vkCmdDrawIndexed(mCommandBuffer, static_cast<uint32_t>(mSkyboxMesh->indices.size()), 1, mSkyboxMesh->indicesOffset, mSkyboxMesh->verticesOffset, mSkyboxMesh->instanceOffset);
		//vkCmdEndRenderPass(mCommandBuffer);
	}

	void VulkanSkybox::UpdateAndPushConstants(VkCommandBuffer commandBuffer) {

	}

	void VulkanSkybox::Terminate() {

	}

}