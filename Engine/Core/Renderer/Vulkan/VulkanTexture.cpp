#include "Engine/Core/PreCompiledHeaders.h"
#include "VulkanTexture.h"



/*
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(mDevice, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(mDevice, stagingBufferMemory);

		stbi_image_free(pixels);

		CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mTextureImage, mTextureImageMemory);

		TransitionImageLayout(mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(stagingBuffer, mTextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		TransitionImageLayout(mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
		vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
*/

namespace Plaza {
	int VulkanTexture::mLastBindingIndex = 0;
	void VulkanTexture::CreateTextureImage(VkDevice device, std::string path) {
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}
		VulkanRenderer::GetRenderer()->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mStagingBuffer, mStagingBufferMemory);

		void* data;
		vkMapMemory(device, mStagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(device, mStagingBufferMemory);

		stbi_image_free(pixels);

		//VulkanRenderer::GetRenderer()->CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mTextureImage, mTextureImageMemory);
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = texWidth;
		imageInfo.extent.height = texHeight;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(device, &imageInfo, nullptr, &mImage) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, mImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VulkanRenderer::GetRenderer()->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &mImageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, mImage, mImageMemory, 0);
		// VkImageCreateInfo imageInfo{};
		// imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		// imageInfo.imageType = VK_IMAGE_TYPE_2D;
		// imageInfo.extent.width = static_cast<uint32_t>(texWidth);
		// imageInfo.extent.height = static_cast<uint32_t>(texHeight);
		// imageInfo.extent.depth = 1;
		// imageInfo.mipLevels = 1;
		// imageInfo.arrayLayers = 1;
		// imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		// imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		// imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		// imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		// if (vkCreateImage(device, &imageInfo, nullptr, &mImage) != VK_SUCCESS) {
		// 	throw std::runtime_error("failed to create image!");
		// }
		// 
		// VkMemoryRequirements memRequirements;
		// vkGetImageMemoryRequirements(device, mImage, &memRequirements);
		// 
		// VkMemoryAllocateInfo allocInfo{};
		// allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		// allocInfo.allocationSize = memRequirements.size;
		// VulkanRenderer* renderer = (VulkanRenderer*)(Application->mRenderer);
		// allocInfo.memoryTypeIndex = renderer->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		// 
		// if (vkAllocateMemory(device, &allocInfo, nullptr, &mTextureImageMemory) != VK_SUCCESS) {
		// 	throw std::runtime_error("failed to allocate image memory!");
		// }
		// 
		// vkBindImageMemory(device, mImage, mTextureImageMemory, 0);

		VulkanRenderer::GetRenderer()->TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VulkanRenderer::GetRenderer()->CopyBufferToImage(mStagingBuffer, mImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		VulkanRenderer::GetRenderer()->TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device, mStagingBuffer, nullptr);
		vkFreeMemory(device, mStagingBufferMemory, nullptr);



	}

	void VulkanTexture::Load(std::string path) {

	}

	unsigned int VulkanTexture::GetTextureID() {
		return (unsigned int)this->mDescriptorSet;
	}

	ImTextureID VulkanTexture::GetImGuiTextureID() {
		ImTextureID id = (ImTextureID)this->mDescriptorSet;
		return id;
	}

	VkImageView VulkanTexture::CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = this->mImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.aspectMask = aspectFlags;

		if (vkCreateImageView(VulkanRenderer::GetRenderer()->mDevice, &viewInfo, nullptr, &mImageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		return mImageView;
	}

	VkDescriptorSet VulkanTexture::GetDescriptorSet() {
		return this->mDescriptorSet;
	}

#define ArraySize(a) \
  ((sizeof(a) / sizeof(*(a))) / \
  static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

	void VulkanTexture::InitDescriptorSet() {
		VkDescriptorPool descriptorPool;

		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; // Adjust based on your descriptor type
		poolSize.descriptorCount = 1;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = 1;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;

		static const uint32_t maxBindlessResources = 16536;
		VkDescriptorPoolSize poolSizesBindless[] =
		{
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxBindlessResources }
		};
		poolInfo.maxSets = maxBindlessResources * ArraySize(poolSizesBindless);
		poolInfo.poolSizeCount = uint32_t(ArraySize(poolSizesBindless));
		poolInfo.pPoolSizes = poolSizesBindless;

		if (vkCreateDescriptorPool(VulkanRenderer::GetRenderer()->mDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			// Handle pool creation failure
			throw std::runtime_error("Failed to create descriptor pool!");
		}


		std::vector<VkDescriptorSetLayout> layouts = { mDescriptorSetLayout };
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &mDescriptorSetLayout;

		VkDescriptorSetVariableDescriptorCountAllocateInfoEXT countInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT };
		uint32_t maxBinding = 16536 - 1;
		countInfo.descriptorSetCount = 1;
		countInfo.pDescriptorCounts = &maxBinding;
		//allocInfo.pNext = &countInfo;

		if (vkAllocateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, &allocInfo, &mDescriptorSet) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = VulkanRenderer::GetRenderer()->mUniformBuffers[0];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(VulkanRenderer::UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = mImageView;
		imageInfo.sampler = mSampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		VkDescriptorBufferInfo bufferInfo2{};
		bufferInfo2.buffer = VulkanRenderer::GetRenderer()->mUniformBuffers[VulkanRenderer::GetRenderer()->mCurrentFrame];
		bufferInfo2.offset = 0;
		bufferInfo2.range = sizeof(VulkanRenderer::UniformBufferObject);

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = VulkanRenderer::GetRenderer()->mDescriptorSets[VulkanRenderer::GetRenderer()->mCurrentFrame];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo2 ;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = VulkanRenderer::GetRenderer()->mDescriptorSets[VulkanRenderer::GetRenderer()->mCurrentFrame];
		descriptorWrites[1].dstBinding = 10;
		descriptorWrites[1].dstArrayElement = VulkanTexture::mLastBindingIndex;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;
		this->mIndexHandle = mLastBindingIndex;

		vkUpdateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, 2, descriptorWrites.data(), 0, nullptr);
		VulkanTexture::mLastBindingIndex++;
	}

	void VulkanTexture::InitDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = &mSampler; // Optional

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 0;
		samplerLayoutBinding.descriptorCount = 16536;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		samplerLayoutBinding.pImmutableSamplers = &mSampler;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;


		std::array<VkDescriptorSetLayoutBinding, 1> bindings = { samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(VulkanRenderer::GetRenderer()->mDevice, &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

		InitDescriptorSet();
	}

	void VulkanTexture::CreateTextureSampler() {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 8;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(VulkanRenderer::GetRenderer()->mPhysicalDevice, &properties);

		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		if (vkCreateSampler(VulkanRenderer::GetRenderer()->mDevice, &samplerInfo, nullptr, &mSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}
}