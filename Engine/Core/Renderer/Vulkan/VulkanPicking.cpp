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

		VkDeviceSize offsets[] = { 0, 0 };
		VkCommandBuffer activeCommandBuffer = commandBuffer;

		VulkanPicking::PushConstants pushData{};
		pushData.projection = Application->activeCamera->GetProjectionMatrix();
		pushData.view = Application->activeCamera->GetViewMatrix();
		pushData.uuid1 = (uint32_t)(meshRenderer.uuid >> 32);
		pushData.uuid2 = (uint32_t)(meshRenderer.uuid & 0xFFFFFFFF);

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(glm::mat4) * 1;
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		std::vector<glm::mat4> instanceModelMatrices = { glm::mat4(1.0f) };
		void* data;
		vkMapMemory(VulkanRenderer::GetRenderer()->mDevice, mesh->mInstanceBufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy(data, instanceModelMatrices.data(), static_cast<size_t>(bufferInfo.size));
		vkUnmapMemory(VulkanRenderer::GetRenderer()->mDevice, mesh->mInstanceBufferMemory);


		vkCmdPushConstants(commandBuffer, this->mRenderPickingTexturePostEffects->mShaders->mPipelineLayout, VK_SHADER_STAGE_ALL, 0, sizeof(VulkanPicking::PushConstants), &pushData);

		std::array<VkBuffer, 2> buffers = { mesh->mVertexBuffer, meshRenderer.renderGroup->mInstanceBuffers[VulkanRenderer::GetRenderer()->mCurrentFrame] };
		vkCmdBindVertexBuffers(activeCommandBuffer, 0, 2, buffers.data(), offsets);
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
		renderPassInfo.framebuffer = VulkanRenderer::GetRenderer()->mFinalSceneFramebuffer;
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