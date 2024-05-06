#include "Engine/Core/PreCompiledHeaders.h"
#include "VulkanBloom.h"
namespace Plaza {
	void VulkanBloom::UpdateDescriptorSet(VkImageLayout inputLayout, VkImageView inputView, VkSampler inputSmapler, VkImageView outputImageView, unsigned int frame) {
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = inputLayout; // Assuming this layout is appropriate
		imageInfo.imageView = inputView;//this->mTexture1->mImageView;
		imageInfo.sampler = inputSmapler;//this->mTexture1->mSampler;

		mComputeShadersScaleDown.mComputeDescriptorSets.resize(Application->mRenderer->mMaxFramesInFlight);
		mUniformBuffers.resize(Application->mRenderer->mMaxFramesInFlight);

		mComputeShadersScaleDown.mDescriptorWrites.resize(3);
		mComputeShadersScaleDown.mDescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		mComputeShadersScaleDown.mDescriptorWrites[0].dstSet = mComputeShadersScaleDown.mComputeDescriptorSets[frame];
		mComputeShadersScaleDown.mDescriptorWrites[0].dstBinding = 0;
		mComputeShadersScaleDown.mDescriptorWrites[0].dstArrayElement = 0;
		mComputeShadersScaleDown.mDescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		mComputeShadersScaleDown.mDescriptorWrites[0].descriptorCount = 1;
		mComputeShadersScaleDown.mDescriptorWrites[0].pImageInfo = &imageInfo;

		VkDescriptorImageInfo storageImageInfo{};
		storageImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL; // Assuming this layout is appropriate
		storageImageInfo.imageView = outputImageView;

		mComputeShadersScaleDown.mDescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		mComputeShadersScaleDown.mDescriptorWrites[1].dstSet = mComputeShadersScaleDown.mComputeDescriptorSets[frame];
		mComputeShadersScaleDown.mDescriptorWrites[1].dstBinding = 1;
		mComputeShadersScaleDown.mDescriptorWrites[1].dstArrayElement = 0;
		mComputeShadersScaleDown.mDescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		mComputeShadersScaleDown.mDescriptorWrites[1].descriptorCount = 1;
		mComputeShadersScaleDown.mDescriptorWrites[1].pImageInfo = &storageImageInfo;

		VkDescriptorBufferInfo uniformBufferInfo{};
		uniformBufferInfo.buffer = mUniformBuffers[frame];
		uniformBufferInfo.offset = 0;
		uniformBufferInfo.range = sizeof(UniformBufferObject);

		mComputeShadersScaleDown.mDescriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		mComputeShadersScaleDown.mDescriptorWrites[2].dstSet = mComputeShadersScaleDown.mComputeDescriptorSets[frame];
		mComputeShadersScaleDown.mDescriptorWrites[2].dstBinding = 2;
		mComputeShadersScaleDown.mDescriptorWrites[2].dstArrayElement = 0;
		mComputeShadersScaleDown.mDescriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		mComputeShadersScaleDown.mDescriptorWrites[2].descriptorCount = 1;
		mComputeShadersScaleDown.mDescriptorWrites[2].pBufferInfo = &uniformBufferInfo;

		vkUpdateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, mComputeShadersScaleDown.mDescriptorWrites.size(), mComputeShadersScaleDown.mDescriptorWrites.data(), 0, nullptr);
	}

	void VulkanBloom::UpdateDescriptorSets() {
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		mUniformBuffers.resize(VulkanRenderer::GetRenderer()->mMaxFramesInFlight);
		mUniformBuffersMemory.resize(VulkanRenderer::GetRenderer()->mMaxFramesInFlight);
		mUniformBuffersMapped.resize(VulkanRenderer::GetRenderer()->mMaxFramesInFlight);
		
		for (size_t i = 0; i < VulkanRenderer::GetRenderer()->mMaxFramesInFlight; i++) {
			VulkanRenderer::GetRenderer()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mUniformBuffers[i], mUniformBuffersMemory[i]);

			vkMapMemory(VulkanRenderer::GetRenderer()->mDevice, mUniformBuffersMemory[i], 0, bufferSize, 0, &mUniformBuffersMapped[i]);
		}


		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(Application->mRenderer->mMaxFramesInFlight);

		poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(Application->mRenderer->mMaxFramesInFlight) * 2;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 2;
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(Application->mRenderer->mMaxFramesInFlight);

		VkDescriptorPool descriptorPool;
		if (vkCreateDescriptorPool(VulkanRenderer::GetRenderer()->mDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}

		mComputeShadersScaleDown.mComputeDescriptorSets.resize(Application->mRenderer->mMaxFramesInFlight);
		std::vector<VkDescriptorSetLayout> layouts(Application->mRenderer->mMaxFramesInFlight, mComputeShadersScaleDown.mComputeDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(Application->mRenderer->mMaxFramesInFlight);
		allocInfo.pSetLayouts = layouts.data();

		if (vkAllocateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, &allocInfo, mComputeShadersScaleDown.mComputeDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < VulkanRenderer::GetRenderer()->mMaxFramesInFlight; i++) {
			UpdateDescriptorSet(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VulkanRenderer::GetRenderer()->mFinalSceneImageView, VulkanRenderer::GetRenderer()->mTextureSampler, this->mTexture2->mImageView, i);
		}
	}

	void VulkanBloom::Init() {
		this->mTexture1 = new VulkanTexture();
		this->mTexture1->mMipLevels = this->mMipCount;
		this->mTexture1->CreateTextureImage(VulkanRenderer::GetRenderer()->mDevice, VK_FORMAT_R32G32B32A32_SFLOAT, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, true);
		this->mTexture1->CreateTextureSampler();
		this->mTexture1->CreateImageView(VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);
		this->mTexture1->InitDescriptorSetLayout();

		this->mTexture2 = new VulkanTexture();
		this->mTexture2->mMipLevels = this->mMipCount;
		this->mTexture2->CreateTextureImage(VulkanRenderer::GetRenderer()->mDevice, VK_FORMAT_R32G32B32A32_SFLOAT, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, true);
		this->mTexture2->CreateTextureSampler();
		this->mTexture2->CreateImageView(VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);
		this->mTexture2->InitDescriptorSetLayout();

		VulkanRenderer::GetRenderer()->AddTrackerToImage(this->mTexture1->mImageView, "Bloom Downscale", this->mTexture1->mSampler, VK_IMAGE_LAYOUT_GENERAL);
		VulkanRenderer::GetRenderer()->AddTrackerToImage(this->mTexture2->mImageView, "Bloom Upscale", this->mTexture2->mSampler, VK_IMAGE_LAYOUT_GENERAL);

		std::array<VkDescriptorSetLayoutBinding, 3> layoutBindings{};
		layoutBindings[0].binding = 0;
		layoutBindings[0].descriptorCount = 1;
		layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		layoutBindings[0].pImmutableSamplers = nullptr;
		layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		layoutBindings[1].binding = 1;
		layoutBindings[1].descriptorCount = 1;
		layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		layoutBindings[1].pImmutableSamplers = nullptr;
		layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		layoutBindings[2].binding = 2;
		layoutBindings[2].descriptorCount = 1;
		layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		layoutBindings[2].pImmutableSamplers = nullptr;
		layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 3;
		layoutInfo.pBindings = layoutBindings.data();

		if (vkCreateDescriptorSetLayout(VulkanRenderer::GetRenderer()->mDevice, &layoutInfo, nullptr, &this->mComputeShadersScaleDown.mComputeDescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute descriptor set layout!");
		}

		UpdateDescriptorSets();

		this->mComputeShadersScaleDown.Init(Application->enginePath + "\\Shaders\\Vulkan\\bloom\\bloomDownScale.comp");
		this->mComputeShadersScaleUp.Init(Application->enginePath + "\\Shaders\\Vulkan\\bloom\\bloomUpScale.comp");
	}

	void VulkanBloom::UpdateUniformBuffers(glm::vec2 texelSize, unsigned int mipLevel, bool useThreshold) {
		float mThreshold = 1.5f;
		float mKnee = 0.1f;
		float m_bloom_intensity = 16.0f;
		float m_bloom_dirt_intensity = 1.0f;


		UniformBufferObject uniformBuffer{};
		uniformBuffer.u_texel_size = texelSize;
		uniformBuffer.u_mip_level = mipLevel;
		uniformBuffer.u_threshold = glm::vec4(mThreshold, mThreshold - mKnee, 2.0f * mKnee, 0.25f * mKnee);
		uniformBuffer.u_use_threshold = useThreshold;
		unsigned int frameIndex = Application->mRenderer->mCurrentFrame;
		memcpy(this->mUniformBuffersMapped[frameIndex], &uniformBuffer, sizeof(uniformBuffer));
	}

	void VulkanBloom::Draw() {
		/* Bloom: downscale */
		glm::uvec2 mipSize = glm::uvec2(Application->appSizes->sceneSize.x / 2, Application->appSizes->sceneSize.y / 2);
		UpdateDescriptorSet(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VulkanRenderer::GetRenderer()->mFinalSceneImageView, VulkanRenderer::GetRenderer()->mTextureSampler, this->mTexture2->mImageView, Application->mRenderer->mCurrentFrame);
		for (uint8_t i = 0; i < mMipCount; ++i)
		{
			//glBindImageTexture(0, mFinalTexturePair->texture1.id, i + 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			UpdateUniformBuffers(1.0f / glm::vec2(mipSize), i, i == 0);

			this->mComputeShadersScaleDown.Dispatch(glm::ceil(float(mipSize.x) / 8), glm::ceil(float(mipSize.y) / 8), 1);
			mipSize = mipSize / 2u;

			VkImageMemoryBarrier imageMemoryBarrier = {};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL; // Previous layout used by the image
			imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL; // New layout to transition the image to
			imageMemoryBarrier.image = this->mTexture2->mImage; // The image to transition
			imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // Aspect of the image to transition
			imageMemoryBarrier.subresourceRange.baseMipLevel = 0; // Base mip level
			imageMemoryBarrier.subresourceRange.levelCount = this->mTexture2->mMipLevels; // Number of mip levels
			imageMemoryBarrier.subresourceRange.baseArrayLayer = 0; // Base array layer
			imageMemoryBarrier.subresourceRange.layerCount = 1; // Number of array layers

			// Specify the access masks and pipeline stages for the transition
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT; // Access mask for the compute shader write
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT; // Access mask for subsequent shader read
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // Queue family indices
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			// Insert pipeline barrier
			vkCmdPipelineBarrier(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer,
				VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, // Compute shader stage
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, // Subsequent shader stage
				0, // Dependency flags
				0, nullptr, // Memory barriers
				0, nullptr, // Buffer memory barriers
				1, &imageMemoryBarrier); // Image memory barriers

			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL; // Previous layout used by the image
			imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL; // New layout to transition the image to
			imageMemoryBarrier.image = this->mTexture1->mImage; // The image to transition
			imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // Aspect of the image to transition
			imageMemoryBarrier.subresourceRange.baseMipLevel = 0; // Base mip level
			imageMemoryBarrier.subresourceRange.levelCount = this->mTexture1->mMipLevels; // Number of mip levels
			imageMemoryBarrier.subresourceRange.baseArrayLayer = 0; // Base array layer
			imageMemoryBarrier.subresourceRange.layerCount = 1; // Number of array layers

			// Specify the access masks and pipeline stages for the transition
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT; // Access mask for the compute shader write
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT; // Access mask for subsequent shader read
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // Queue family indices
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			// Insert pipeline barrier
			vkCmdPipelineBarrier(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer,
				VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, // Compute shader stage
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, // Subsequent shader stage
				0, // Dependency flags
				0, nullptr, // Memory barriers
				0, nullptr, // Buffer memory barriers
				1, &imageMemoryBarrier); // Image memory barriers

			UpdateDescriptorSet(VK_IMAGE_LAYOUT_GENERAL, this->mTexture1->mImageView, this->mTexture1->mSampler, this->mTexture2->mImageView, Application->mRenderer->mCurrentFrame);

			//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
		}

		/* Bloom: upscale */
		//mBloomUpScaleShader->use();
		//mBloomUpScaleShader->setFloat("u_bloom_intensity", m_bloom_intensity);
		//glBindTextureUnit(0, mFinalTexturePair->texture1.id);

		for (uint8_t i = mMipCount; i >= 1; --i)
		{
			mipSize.x = glm::max(1.0, glm::floor(float(Application->appSizes->sceneSize.x) / glm::pow(2.0, i - 1)));
			mipSize.y = glm::max(1.0, glm::floor(float(Application->appSizes->sceneSize.y) / glm::pow(2.0, i - 1)));

			//mBloomUpScaleShader->setVec2("u_texel_size", 1.0f / glm::vec2(mipSize));
			//mBloomUpScaleShader->setInt("u_mip_level", i);
			//
			//glBindImageTexture(0, mFinalTexturePair->texture1.id, i - 1, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

			//((**    this->mComputeShadersScaleDown.Dispatch(glm::ceil(float(mipSize.x) / 8), glm::ceil(float(mipSize.y) / 8), 1);

			//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
		}
	}
	void VulkanBloom::BlendBloomWithScene() {

	}
	void VulkanBloom::Terminate() {

	}
}