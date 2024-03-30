#include <Engine/Core/PreCompiledHeaders.h>
#include "VulkanPicking.h"
#include "VulkanPostEffects.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"
#include "VulkanShaders.h"

namespace Plaza {
	void VulkanPicking::Init() {
		this->mResolution = Application->appSizes->sceneSize;

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(VulkanPicking::PushConstants);

		this->InitializeDescriptorSets();

		this->mPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		this->mPipelineLayoutInfo.setLayoutCount = 1;
		this->mPipelineLayoutInfo.pSetLayouts = &this->mDescriptorSetLayout;
		this->mPipelineLayoutInfo.pushConstantRangeCount = 1;
		this->mPipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		this->InitializePicking();
		this->InitializeOutline();

		this->InitializeImageView();
		this->InitializeFramebuffer();
	}

	void VulkanPicking::InitializeImageView() {
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.extent.width = Application->appSizes->sceneSize.x;
		imageCreateInfo.extent.height = Application->appSizes->sceneSize.y;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		if (vkCreateImage(VulkanRenderer::GetRenderer()->mDevice, &imageCreateInfo, nullptr, &this->mPickingTextureImage) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(VulkanRenderer::GetRenderer()->mDevice, this->mPickingTextureImage, &memoryRequirements);

		VkMemoryAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = VulkanRenderer::GetRenderer()->FindMemoryType(
			memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VkDeviceMemory imageMemory;
		if (vkAllocateMemory(VulkanRenderer::GetRenderer()->mDevice, &allocateInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}
		vkBindImageMemory(VulkanRenderer::GetRenderer()->mDevice, this->mPickingTextureImage, imageMemory, 0);

		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = this->mPickingTextureImage;

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(VulkanRenderer::GetRenderer()->mDevice, &createInfo, nullptr, &this->mPickingTextureImageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}

	void VulkanPicking::InitializeFramebuffer() {
		std::array<VkImageView, 1> attachments = {
			mPickingTextureImageView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = this->mRenderPickingTexturePostEffects->mRenderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = Application->appSizes->sceneSize.x;
		framebufferInfo.height = Application->appSizes->sceneSize.y;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(VulkanRenderer::GetRenderer()->mDevice, &framebufferInfo, nullptr, &mFramebuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}

	}

	void VulkanPicking::InitializeDescriptorSets() {
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 0;//static_cast<uint32_t>(bindings.size());
		//layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(VulkanRenderer::GetRenderer()->mDevice, &layoutInfo, nullptr, &this->mDescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void VulkanPicking::InitializeRenderPass() {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = VulkanRenderer::GetRenderer()->FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		//subpass.pDepthStencilAttachment = &depthAttachmentRef;

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

		std::array<VkAttachmentDescription, 1> attachments = { colorAttachment };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(VulkanRenderer::GetRenderer()->mDevice, &renderPassInfo, nullptr, &this->mRenderPickingTexturePostEffects->mRenderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void VulkanPicking::InitializePicking() {
		const std::string pickingVertexPath = VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\picking\\picking.vert");
		const std::string pickingFragmentPath = VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\picking\\picking.frag");

		this->mRenderPickingTexturePostEffects = new VulkanPostEffects();

		this->InitializeRenderPass();

		this->mRenderPickingTexturePostEffects->mShaders = new VulkanShaders(pickingVertexPath, pickingFragmentPath, "");

		this->mRenderPickingTexturePostEffects->mShaders->InitializeFull(
			VulkanRenderer::GetRenderer()->mDevice,
			this->mPipelineLayoutInfo,
			true,
			this->mResolution.x,
			this->mResolution.y,
			{}, {}, {}, {}, {}, {}, {}, {},
			this->mRenderPickingTexturePostEffects->mRenderPass,
			{});
	}

	void VulkanPicking::InitializeOutline() {
		//const std::string outlineVertexPath = Application->enginePath + "\\Shaders\\Vulkan\\picking\\outline.vert";
		//const std::string outlineFragmentPath = Application->enginePath + "\\Shaders\\Vulkan\\picking\\outline.frag";
		//
		//this->mRenderPickingTexturePostEffects = new VulkanPostEffects();
		//
		//this->mOutlinePostEffects->mRenderPass = VulkanRenderer::GetRenderer()->mRenderPass;
		//this->mOutlinePostEffects->mShaders = new VulkanShaders(outlineVertexPath, outlineFragmentPath, "");
		//
		//this->mOutlinePostEffects->mShaders->InitializeFull(
		//	VulkanRenderer::GetRenderer()->mDevice,
		//	this->mPipelineLayoutInfo,
		//	true,
		//	Application->appSizes->sceneSize.x,
		//	Application->appSizes->sceneSize.y,
		//	{}, {}, {}, {}, {}, {}, {}, {},
		//	this->mRenderPickingTexturePostEffects->mRenderPass,
		//	{});
	}

	void VulkanPicking::DrawMeshToPickingTexture(const MeshRenderer& meshRenderer, VkCommandBuffer& commandBuffer) {
		VulkanMesh* mesh = (VulkanMesh*)meshRenderer.mesh;

		VkDeviceSize offsets[] = { 0 };
		VkCommandBuffer activeCommandBuffer = commandBuffer;

		VulkanPicking::PushConstants pushData{};
		pushData.projection = Application->activeCamera->GetProjectionMatrix();
		pushData.view = Application->activeCamera->GetViewMatrix();
		pushData.model = Application->activeScene->GetComponent<Transform>(meshRenderer.uuid)->modelMatrix;
		pushData.uuid1 = (uint32_t)(meshRenderer.uuid >> 32);
		pushData.uuid2 = (uint32_t)(meshRenderer.uuid & 0xFFFFFFFF);

		vkCmdPushConstants(commandBuffer, this->mRenderPickingTexturePostEffects->mShaders->mPipelineLayout, VK_SHADER_STAGE_ALL, 0, sizeof(VulkanPicking::PushConstants), &pushData);

		std::array<VkBuffer, 1> buffers = { mesh->mVertexBuffer };
		vkCmdBindVertexBuffers(activeCommandBuffer, 0, 1, buffers.data(), offsets);
		vkCmdBindIndexBuffer(activeCommandBuffer, mesh->mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(activeCommandBuffer, static_cast<uint32_t>(mesh->indices.size()), 1, 0, 0, 0);
	}

	void VulkanPicking::DrawSelectedObjectsUuid() {
		PLAZA_PROFILE_SECTION("DrawSelectedObjectsUuid");

		VkCommandBuffer& commandBuffer = *VulkanRenderer::GetRenderer()->mActiveCommandBuffer;
		//VkCommandBuffer commandBuffer = VulkanRenderer::GetRenderer()->BeginSingleTimeCommands();

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();
		renderPassInfo.renderPass = this->mRenderPickingTexturePostEffects->mRenderPass;
		renderPassInfo.framebuffer = this->mFramebuffer;//VulkanRenderer::GetRenderer()->mFinalSceneFramebuffer;
		renderPassInfo.renderArea.extent.width = this->mResolution.x;
		renderPassInfo.renderArea.extent.height = this->mResolution.y;
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->mRenderPickingTexturePostEffects->mShaders->mPipeline);

		for (const auto& [key, value] : Application->activeScene->meshRendererComponents) {
			this->DrawMeshToPickingTexture(value, commandBuffer);
		}

		vkCmdEndRenderPass(commandBuffer);
		//VulkanRenderer::GetRenderer()->EndSingleTimeCommands(commandBuffer);
	}

	void VulkanPicking::DrawOutline() {

	}

	uint64_t VulkanPicking::ReadPickingTexture(glm::vec2 pos) {
		return 0;
	}

	uint64_t VulkanPicking::DrawAndRead(glm::vec2 pos) {
		return 0;
	}

	void VulkanPicking::UpdateAndPushConstants(VkCommandBuffer commandBuffer) {

	}

	void VulkanPicking::Terminate() {

	}

}