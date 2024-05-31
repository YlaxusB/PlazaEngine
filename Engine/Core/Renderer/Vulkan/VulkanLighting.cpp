#include <Engine/Core/PreCompiledHeaders.h>
#include "VulkanLighting.h"
#include "VulkanPostEffects.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"
#include "VulkanShaders.h"

namespace Plaza {
	void VulkanLighting::InitializeBuffers() {
		this->mLightsBuffer.resize(Application->mRenderer->mMaxFramesInFlight);
		this->mLightsBufferMemory.resize(Application->mRenderer->mMaxFramesInFlight);
		this->mTilesBuffer.resize(Application->mRenderer->mMaxFramesInFlight);
		this->mTilesBufferMemory.resize(Application->mRenderer->mMaxFramesInFlight);
		this->mDepthValuesBuffer.resize(Application->mRenderer->mMaxFramesInFlight);
		this->mDepthValuesBufferMemory.resize(Application->mRenderer->mMaxFramesInFlight);

		for (int i = 0; i < Application->mRenderer->mMaxFramesInFlight; ++i) {
			VulkanRenderer::GetRenderer()->CreateBuffer(
				sizeof(LightStruct) * 1024,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				this->mLightsBuffer[i],
				this->mLightsBufferMemory[i]);

			VulkanRenderer::GetRenderer()->CreateBuffer(sizeof(Tile) * 1024,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				this->mTilesBuffer[i],
				this->mTilesBufferMemory[i]);


			VulkanRenderer::GetRenderer()->CreateBuffer(sizeof(glm::vec2) * 1024,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				this->mDepthValuesBuffer[i],
				this->mDepthValuesBufferMemory[i]);
		}
	}
	/*

layout(binding = 1) buffer ClusterBuffer {
	Cluster clusters[];
};

layout(binding = 2) buffer OutputBuffer {
	vec3 result[];
};

layout(binding = 3) buffer DepthTileBuffer {
	vec2 tileDepthStats[];
};

layout(binding = 4) uniform sampler2D depthMap;

	*/
	void VulkanLighting::InitializeDescriptorSets() {
		/* Descriptor Sets Layout */
		std::array<VkDescriptorSetLayoutBinding, 4> layoutBindings{};
		layoutBindings[0].binding = 0;
		layoutBindings[0].descriptorCount = 1;
		layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		layoutBindings[0].pImmutableSamplers = nullptr;
		layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		layoutBindings[0].stageFlags |= VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;

		layoutBindings[1].binding = 1;
		layoutBindings[1].descriptorCount = 1;
		layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		layoutBindings[1].pImmutableSamplers = nullptr;
		layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		layoutBindings[1].stageFlags |= VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;

		layoutBindings[2].binding = 2;
		layoutBindings[2].descriptorCount = 1;
		layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		layoutBindings[2].pImmutableSamplers = nullptr;
		layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		layoutBindings[2].stageFlags |= VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;

		layoutBindings[3].binding = 3;
		layoutBindings[3].descriptorCount = 1;
		layoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		layoutBindings[3].pImmutableSamplers = nullptr;
		layoutBindings[3].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		layoutBindings[3].stageFlags |= VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = layoutBindings.size();
		layoutInfo.pBindings = layoutBindings.data();

		if (vkCreateDescriptorSetLayout(VulkanRenderer::GetRenderer()->mDevice, &layoutInfo, nullptr, &this->mLightSorterComputeShaders.mComputeDescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute descriptor set layout!");
		}

		/* Descriptor sets */
		std::vector<VkDescriptorSetLayout> layouts(Application->mRenderer->mMaxFramesInFlight, this->mLightSorterComputeShaders.mComputeDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = VulkanRenderer::GetRenderer()->mDescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(Application->mRenderer->mMaxFramesInFlight);
		allocInfo.pSetLayouts = layouts.data();

		this->mLightSorterComputeShaders.mComputeDescriptorSets.resize(Application->mRenderer->mMaxFramesInFlight);
		if (vkAllocateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, &allocInfo, this->mLightSorterComputeShaders.mComputeDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (int i = 0; i < Application->mRenderer->mMaxFramesInFlight; ++i) {
			VkDescriptorBufferInfo lightsBufferInfo{};
			lightsBufferInfo.buffer = this->mLightsBuffer[i];
			lightsBufferInfo.range = VK_WHOLE_SIZE;
			this->mLightSorterComputeShaders.mDescriptorWrites.resize(4);
			this->mLightSorterComputeShaders.mDescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			this->mLightSorterComputeShaders.mDescriptorWrites[0].dstSet = this->mLightSorterComputeShaders.mComputeDescriptorSets[i];
			this->mLightSorterComputeShaders.mDescriptorWrites[0].dstBinding = 0;
			this->mLightSorterComputeShaders.mDescriptorWrites[0].dstArrayElement = 0;
			this->mLightSorterComputeShaders.mDescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			this->mLightSorterComputeShaders.mDescriptorWrites[0].descriptorCount = 1;
			this->mLightSorterComputeShaders.mDescriptorWrites[0].pBufferInfo = &lightsBufferInfo;

			VkDescriptorBufferInfo tilesBufferInfo{};
			tilesBufferInfo.buffer = this->mTilesBuffer[i];
			tilesBufferInfo.range = VK_WHOLE_SIZE;
			this->mLightSorterComputeShaders.mDescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			this->mLightSorterComputeShaders.mDescriptorWrites[1].dstSet = this->mLightSorterComputeShaders.mComputeDescriptorSets[i];
			this->mLightSorterComputeShaders.mDescriptorWrites[1].dstBinding = 1;
			this->mLightSorterComputeShaders.mDescriptorWrites[1].dstArrayElement = 0;
			this->mLightSorterComputeShaders.mDescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			this->mLightSorterComputeShaders.mDescriptorWrites[1].descriptorCount = 1;
			this->mLightSorterComputeShaders.mDescriptorWrites[1].pBufferInfo = &tilesBufferInfo;

			VkDescriptorBufferInfo depthValuesBufferInfo{};
			depthValuesBufferInfo.buffer = this->mDepthValuesBuffer[i];
			depthValuesBufferInfo.range = VK_WHOLE_SIZE;
			this->mLightSorterComputeShaders.mDescriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			this->mLightSorterComputeShaders.mDescriptorWrites[2].dstSet = this->mLightSorterComputeShaders.mComputeDescriptorSets[i];
			this->mLightSorterComputeShaders.mDescriptorWrites[2].dstBinding = 2;
			this->mLightSorterComputeShaders.mDescriptorWrites[2].dstArrayElement = 0;
			this->mLightSorterComputeShaders.mDescriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			this->mLightSorterComputeShaders.mDescriptorWrites[2].descriptorCount = 1;
			this->mLightSorterComputeShaders.mDescriptorWrites[2].pBufferInfo = &depthValuesBufferInfo;

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			imageInfo.imageView = VulkanRenderer::GetRenderer()->mDepthImageView;
			imageInfo.sampler = VulkanRenderer::GetRenderer()->mTextureSampler;
			this->mLightSorterComputeShaders.mDescriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			this->mLightSorterComputeShaders.mDescriptorWrites[3].dstSet = this->mLightSorterComputeShaders.mComputeDescriptorSets[i];
			this->mLightSorterComputeShaders.mDescriptorWrites[3].dstBinding = 3;
			this->mLightSorterComputeShaders.mDescriptorWrites[3].dstArrayElement = 0;
			this->mLightSorterComputeShaders.mDescriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			this->mLightSorterComputeShaders.mDescriptorWrites[3].descriptorCount = 1;
			this->mLightSorterComputeShaders.mDescriptorWrites[3].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, this->mLightSorterComputeShaders.mDescriptorWrites.size(), this->mLightSorterComputeShaders.mDescriptorWrites.data(), 0, nullptr);
		}
	}

	void VulkanLighting::Init() {
		std::cerr << "Initializing Tiled Deferred Shading \n";
		/* Initialize Light Sorter Compute Shaders */
		this->InitializeBuffers();
		this->InitializeDescriptorSets();

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.size = sizeof(LightSorterPushConstants);
		pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		this->mLightSorterComputeShaders.Init(Application->enginePath + "\\Shaders\\Vulkan\\lighting\\lightSorter.comp", 1, pushConstantRange);

		/* Initialize Deferred Pass */
		mDeferredPassRenderer = new VulkanPostEffects();
		mDeferredPassRenderer->Init();
	}

	void VulkanLighting::GetLights() {
		this->mLights.clear();
		for (const auto& [key, value] : Application->activeScene->lightComponents) {
			glm::vec3 position = Application->activeScene->transformComponents.at(key).GetWorldPosition();
			this->mLights.push_back(LightStruct{ value.color, value.radius, position, value.intensity });
		}

		void* data;
		size_t bufferSize = sizeof(LightStruct) * this->mLights.size();
		vkMapMemory(VulkanRenderer::GetRenderer()->mDevice, this->mLightsBufferMemory[Application->mRenderer->mCurrentFrame], 0, VK_WHOLE_SIZE, 0, &data);
		memcpy(data, this->mLights.data(), (size_t)bufferSize);
		vkUnmapMemory(VulkanRenderer::GetRenderer()->mDevice, this->mLightsBufferMemory[Application->mRenderer->mCurrentFrame]);
	}

	void VulkanLighting::UpdateTiles() {
		this->sceneSize = Application->appSizes->sceneSize;

		glm::vec2 clusterSize = glm::vec2(32.0f);
		glm::vec2 clusterCount = glm::ceil(this->sceneSize / clusterSize);
		int extraX = int(Application->appSizes->sceneSize.x) % 32 != 0 ? 1 : 0;
		LightSorterPushConstants lightSorterPushConstants{};
		lightSorterPushConstants.clusterSize = glm::vec2(32);
		lightSorterPushConstants.first = true;
		lightSorterPushConstants.lightCount = this->mLights.size();
		lightSorterPushConstants.projection = Application->activeCamera->GetProjectionMatrix();
		lightSorterPushConstants.view = Application->activeCamera->GetViewMatrix();
		lightSorterPushConstants.screenSize = this->sceneSize;
		vkCmdPushConstants(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer, this->mLightSorterComputeShaders.mComputePipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, { 0 }, sizeof(LightSorterPushConstants), &lightSorterPushConstants);
		this->mLightSorterComputeShaders.Dispatch(clusterCount.x, clusterCount.y, 1);

		
	}

	void VulkanLighting::DrawDeferredPass() {
		mDeferredPassRenderer->DrawFullScreenRectangle(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer);
	}

	void VulkanLighting::Terminate() {

	}
}