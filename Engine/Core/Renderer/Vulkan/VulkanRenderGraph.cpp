#include "Engine/Core/PreCompiledHeaders.h"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_vulkan.h"

namespace Plaza {
	void VulkanRenderGraph::BuildDefaultRenderGraph() {
		this->AddRenderPass(std::make_shared<VulkanRenderPass>("Deferred Geometry Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 0, PlazaBufferType::PL_BUFFER_UNIFORM_BUFFER, PL_STAGE_ALL, "UniformBufferObject"))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(VulkanRenderer::GetRenderer()->mMaxBindlessTextures, 20, PlazaBufferType::PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, "TexturesBuffer"))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 6, PlazaBufferType::PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, "samplerBRDFLUT"))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 7, PlazaBufferType::PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, "prefilterMap"))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 8, PlazaBufferType::PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, "irradianceMap"))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 9, PlazaBufferType::PL_BUFFER_COMBINED_IMAGE_SAMPLER, PL_STAGE_FRAGMENT, "shadowsDepthMap"))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 19, PlazaBufferType::PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, "materialsBuffer"))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 1, PlazaBufferType::PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, "boneMatrices"))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 2, PlazaBufferType::PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, "renderGroupOffsetsBuffer"))
			->AddInputResource(std::make_shared<VulkanBufferBinding>(1, 3, PlazaBufferType::PL_BUFFER_STORAGE_BUFFER, PL_STAGE_VERTEX, "renderGroupMaterialsOffsetsBuffer"))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>("GPosition", 1, 0, 0, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>("GDiffuse", 1, 1, 0, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>("GNormal", 1, 2, 0, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>("GOthers", 1, 3, 0, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1));

		this->AddRenderPass(std::make_shared<VulkanRenderPass>("Deferred Lighting Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT))
			->AddInputResource(std::make_shared<VulkanTextureBinding>("GPosition", 1, 0, 0, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
			->AddInputResource(std::make_shared<VulkanTextureBinding>("GDiffuse", 1, 0, 1, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
			->AddInputResource(std::make_shared<VulkanTextureBinding>("GNormal", 1, 0, 2, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
			->AddInputResource(std::make_shared<VulkanTextureBinding>("GOthers", 1, 0, 3, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
			->AddOutputResource(std::make_shared<VulkanTextureBinding>("SceneTexture", 1, 0, 0, PlazaBufferType::PL_BUFFER_SAMPLER, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1));
	}

	void VulkanBufferBinding::Compile() {
		mBuffer = std::make_shared<PlVkBuffer>();
		mBuffer->CreateBuffer(mMaxItems * sizeof(mStride), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, 0);
	}
	void VulkanBufferBinding::Destroy() {

	}

	void VulkanTextureBinding::Compile() {
		mTexture = std::make_shared<VulkanTexture>();
		mTexture->CreateTextureImage(VulkanRenderer::GetRenderer()->mDevice, PlImageFormatToVkFormat(mFormat), mResolution.x, mResolution.y, false, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, 1, 0, false);
		VulkanRenderer::GetRenderer()->TransitionImageLayout(mTexture->mImage, PlImageFormatToVkFormat(mFormat), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		mTexture->CreateTextureSampler();
		mTexture->CreateImageView(PlImageFormatToVkFormat(mFormat), VK_IMAGE_ASPECT_COLOR_BIT);
	}
	void VulkanTextureBinding::Destroy() {

	}

	void VulkanRenderPass::Compile() {
		// Render Pass
		// Frame buffer
		// Descriptor sets
		// Descriptor set
		std::vector<VkDescriptorSetLayoutBinding> descriptorSets{};

		for (const auto& [key, value] : mInputBindings) {
			switch (value->mBindingType) {
			case PL_BINDING_BUFFER:
				descriptorSets.push_back(static_cast<VulkanBufferBinding*>(value.get())->GetDescriptorLayout());
				break;
			case PL_BINDING_TEXTURE:
				descriptorSets.push_back(static_cast<VulkanTextureBinding*>(value.get())->GetDescriptorLayout());
				break;
			}
		}
		//descriptorSets.push_back(plvk::descriptorSetLayoutBinding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, VK_SHADER_STAGE_ALL));
		//static const uint32_t maxBindlessResources = 16536;
		//descriptorSets.push_back(plvk::descriptorSetLayoutBinding(20, maxBindlessResources, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nullptr, VK_SHADER_STAGE_FRAGMENT_BIT));
		//descriptorSets.push_back(plvk::descriptorSetLayoutBinding(6, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nullptr, VK_SHADER_STAGE_FRAGMENT_BIT));
		//descriptorSets.push_back(plvk::descriptorSetLayoutBinding(7, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nullptr, VK_SHADER_STAGE_FRAGMENT_BIT));
		//descriptorSets.push_back(plvk::descriptorSetLayoutBinding(8, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nullptr, VK_SHADER_STAGE_FRAGMENT_BIT));
		//descriptorSets.push_back(plvk::descriptorSetLayoutBinding(9, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nullptr, VK_SHADER_STAGE_FRAGMENT_BIT));
		//descriptorSets.push_back(plvk::descriptorSetLayoutBinding(19, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, nullptr, VK_SHADER_STAGE_VERTEX_BIT));
		//descriptorSets.push_back(plvk::descriptorSetLayoutBinding(1, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, nullptr, VK_SHADER_STAGE_VERTEX_BIT));
		//descriptorSets.push_back(plvk::descriptorSetLayoutBinding(2, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, nullptr, VK_SHADER_STAGE_VERTEX_BIT));
		//descriptorSets.push_back(plvk::descriptorSetLayoutBinding(3, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, nullptr, VK_SHADER_STAGE_VERTEX_BIT));

		//VkDescriptorBindingFlags bindlessFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT,	nullptr };
		//VkDescriptorBindingFlagsEXT bindingFlags[] = { 0, bindlessFlags, 0, 0, 0, 0, 0, 0, 0, 0 };
		//extendedInfo.pBindingFlags = bindingFlags
		//extendedInfo.bindingCount = static_cast<uint32_t>(descriptorSets.size());

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
			maxBindlessResources - 1);
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

			/*
						if (mType == PL_BUFFER_PUSH_CONSTANTS)
				return {};

			VkDescriptorBufferInfo bufferInfo = plvk::descriptorBufferInfo(mBuffer, 0, mStride);
			return plvk::writeDescriptorSet(descriptorSet, this->mBinding, 0, PlBufferTypeToVkDescriptorType(mType), mDescriptorCount, nullptr, &bufferInfo);
			VkDescriptorImageInfo imageInfo = plvk::descriptorImageInfo(mTexture.get()->GetLayout(), mTexture.get()->mImageView, mTexture.get()->mSampler);
			return plvk::writeDescriptorSet(descriptorSet, this->mBinding, 0, PlBufferTypeToVkDescriptorType(mBufferType), mDescriptorCount, &imageInfo, nullptr);
			*/

			for (const auto& [key, value] : mInputBindings) {
				switch (value->mBindingType) {
				case PL_BINDING_BUFFER: {
					VulkanBufferBinding* bufferBinding = static_cast<VulkanBufferBinding*>(value.get());
					VkDescriptorBufferInfo bufferInfo = bufferBinding->GetBufferInfo();
					descriptorWrites.push_back(bufferBinding->GetDescriptorWrite(mDescriptorSets[i], bufferInfo));
					break;
				}
				case PL_BINDING_TEXTURE: {
					VulkanTextureBinding* textureBinding = static_cast<VulkanTextureBinding*>(value.get());
					VkDescriptorImageInfo imageInfo = textureBinding->GetImageInfo();
					descriptorWrites.push_back(textureBinding->GetDescriptorWrite(mDescriptorSets[i], imageInfo));
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
	}

	void VulkanRenderPass::BindRenderPass() {
		std::array<VkClearValue, 5> clearValues{};
		clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[1].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[2].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[3].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[4].depthStencil = { 1.0f, 0 };
		VkRenderPassBeginInfo renderPassInfo = plvk::renderPassBeginInfo(this->mRenderPass, this->mFrameBuffer,
			Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 0, 0, clearValues.size(), clearValues.data());
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

		for (auto& [key, value] : mPasses) {
			this->GetRenderPass(key)->UpdateCommandBuffer(commandBuffer);
			this->GetRenderPass(key)->Execute(this);
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
}