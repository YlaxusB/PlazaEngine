#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include "ThirdParty/vulkan/vulkan/vulkan.h"

namespace Plaza {
	class PlDescriptorSetLayout : public VkDescriptorSetLayoutBinding {
	public:
		PlDescriptorSetLayout(
			uint32_t binding,
			uint32_t descriptorCount,
			VkDescriptorType descriptorType,
			VkSampler* pImmutableSamplers,
			VkShaderStageFlags shaderStageFlags) {

			this->binding = binding;
			this->descriptorCount = descriptorCount;
			this->descriptorType = descriptorType;
			this->pImmutableSamplers = pImmutableSamplers;
			this->stageFlags = shaderStageFlags;
		}
	};

	class PlDescriptorSetLayoutCreateInfo : public VkDescriptorSetLayoutCreateInfo {
	public:
		PlDescriptorSetLayoutCreateInfo(
			std::vector<VkDescriptorSetLayoutBinding>& bindings,
			VkDescriptorSetLayoutCreateFlags flags
		) {
			this->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			this->bindingCount = static_cast<uint32_t>(bindings.size());
			this->pBindings = bindings.data();
			this->flags = flags;
			this->pNext = nullptr;
		}
	};

	class PlWriteDescriptorSet : public VkWriteDescriptorSet {
	public:
		PlWriteDescriptorSet(
			VkDescriptorSet dstSet,
			uint32_t dstBinding = 0,
			uint32_t dstArrayElement = 0,
			VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			uint32_t descriptorCount = 1,
			VkDescriptorImageInfo* imageInfo = nullptr,
			VkDescriptorBufferInfo* bufferInfo = nullptr
			) {
			this->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			this->dstSet = dstSet;
			this->dstBinding = dstBinding;
			this->dstArrayElement = dstArrayElement;
			this->descriptorType = descriptorType;
			this->descriptorCount = descriptorCount;
			if (imageInfo)
				this->pImageInfo = imageInfo;
			if (bufferInfo)
				this->pBufferInfo = bufferInfo;
		}
	};

	class PlDescriptorImageInfo : public VkDescriptorImageInfo {
	public:
		PlDescriptorImageInfo(
			VkImageLayout layout,
			VkImageView imageView,
			VkSampler sampler
		) {
			this->imageLayout = layout;
			this->imageView = imageView;
			this->sampler = sampler;
		}
	};
}