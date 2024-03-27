#include "Engine/Core/PreCompiledHeaders.h"
#include "VulkanTexture.h"
#include "ThirdParty/dds_image/dds.hpp"
#include "ThirdParty/DirectXTex/DirectXTex/DirectXTex.h"

namespace Plaza {
	int VulkanTexture::mLastBindingIndex = 0;

	void VulkanTexture::GenerateMipmaps(VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, VkFormat format) {
		// Check if image format supports linear blitting
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(VulkanRenderer::GetRenderer()->mPhysicalDevice, format, &formatProperties);

		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("texture image format does not support linear blitting!");
		}

		VkCommandBuffer commandBuffer = VulkanRenderer::GetRenderer()->BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = texWidth;
		int32_t mipHeight = texHeight;

		for (uint32_t i = 1; i < mipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VulkanRenderer::GetRenderer()->EndSingleTimeCommands(commandBuffer);
	}

	void VulkanTexture::CreateTextureImage(VkDevice device, std::string path, VkFormat& format, bool generateMipMaps) {
		bool isDDS = std::filesystem::path{ path }.extension() == ".dds";
		dds::Image image;

		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = nullptr;
		uint32_t pix = 0;
		VkFormat imageFormat = format;
		DirectX::TexMetadata metadata;
		DirectX::ScratchImage scratchImage;
		VkDeviceSize imageSize;
		if (!isDDS)
		{
			pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			imageSize = texWidth * texHeight * 4;
		}
		else {
			dds::ReadResult res = dds::readFile(path, &image);
			texWidth = image.width;
			texHeight = image.height;
			VkImageViewCreateInfo asd = dds::getVulkanImageViewCreateInfo(&image);
			VkImageCreateInfo asd2 = dds::getVulkanImageCreateInfo(&image);
			pix = dds::getBitsPerPixel(image.format);
			pix = dds::getBlockSize(image.format);
			imageFormat = dds::getVulkanFormat(image.format, false);
			format = imageFormat;
			texChannels = 4;

			// Convert std::string to std::wstring
			std::wstring widestr = std::wstring(path.begin(), path.end());
			const wchar_t* widecstr = widestr.c_str();
			DirectX::LoadFromDDSFile(widecstr, DirectX::DDS_FLAGS_FORCE_RGB, &metadata, scratchImage);

			texWidth = metadata.width;
			texHeight = metadata.height;
			texChannels = 4;

			// Access image data
			
			const DirectX::Image* image2 = scratchImage.GetImage(0, 0, 0);
			pixels = static_cast<stbi_uc*>(image2->pixels);

			imageSize = image2->slicePitch;
		}

		this->mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
		if (!generateMipMaps)
			this->mMipLevels = 1;

		if (!isDDS && !pixels) {
			throw std::runtime_error("failed to load texture image!");
		}
		VulkanRenderer::GetRenderer()->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mStagingBuffer, mStagingBufferMemory);

		void* data;
		vkMapMemory(device, mStagingBufferMemory, 0, imageSize, 0, &data);
		if (!isDDS)
			memcpy(data, pixels, static_cast<size_t>(imageSize));
		else
			memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(device, mStagingBufferMemory);

		if (!isDDS)
			stbi_image_free(pixels);


		//VulkanRenderer::GetRenderer()->CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mTextureImage, mTextureImageMemory);
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = texWidth;
		imageInfo.extent.height = texHeight;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = this->mMipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = imageFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;

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

		VulkanRenderer::GetRenderer()->TransitionImageLayout(mImage, imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VulkanRenderer::GetRenderer()->CopyBufferToImage(mStagingBuffer, mImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		VulkanRenderer::GetRenderer()->TransitionImageLayout(mImage, imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


		vkDestroyBuffer(device, mStagingBuffer, nullptr);
		vkFreeMemory(device, mStagingBufferMemory, nullptr);


		//TODO: FIX VALIDATION ERROR WHEN GENERATING MIP MAP
		if (generateMipMaps)
			GenerateMipmaps(this->mImage, texWidth, texHeight, this->mMipLevels, imageFormat);

	}

	void VulkanTexture::CreateTextureImage(VkDevice device, VkFormat format, int width, int height, bool generateMipMaps) {
		if (!generateMipMaps)
			this->mMipLevels = 0;

		VkImageCreateInfo imageInfo;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = this->mMipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.format = format;
		imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		if (vkCreateImage(device, &imageInfo, nullptr, &this->mImage) != VK_SUCCESS) {
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

		if (generateMipMaps)
			GenerateMipmaps(this->mImage, width, height, this->mMipLevels, format);

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

	VkImageView VulkanTexture::CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType, unsigned int layerCount) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = this->mImage;
		viewInfo.viewType = viewType;
		viewInfo.format = format;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = this->mMipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = layerCount;
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

		static const uint32_t maxBindlessResources = 16536 * 4;
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

		for (size_t i = 0; i < Application->mRenderer->mMaxFramesInFlight; i++) {
			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

			VkDescriptorBufferInfo bufferInfo2{};
			bufferInfo2.buffer = VulkanRenderer::GetRenderer()->mUniformBuffers[i];
			bufferInfo2.offset = 0;
			bufferInfo2.range = sizeof(VulkanRenderer::UniformBufferObject);

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = VulkanRenderer::GetRenderer()->mDescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo2;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = VulkanRenderer::GetRenderer()->mDescriptorSets[i];
			descriptorWrites[1].dstBinding = 20;
			descriptorWrites[1].dstArrayElement = VulkanTexture::mLastBindingIndex;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;
			this->mIndexHandle = mLastBindingIndex;

			vkUpdateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, 2, descriptorWrites.data(), 0, nullptr);
		}
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
		//samplerInfo.maxAnisotropy = 0;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(VulkanRenderer::GetRenderer()->mPhysicalDevice, &properties);
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		//samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(this->mMipLevels);
		if (vkCreateSampler(VulkanRenderer::GetRenderer()->mDevice, &samplerInfo, nullptr, &mSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}
}