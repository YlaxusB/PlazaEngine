#pragma once
#include "Renderer.h"
#include "Engine/Core/Renderer/RenderGraph.h"
#include "VulkanPlazaPipeline.h"
#include "VulkanPlazaInitializator.h"
#include "VulkanTexture.h"
#include "VulkanPlazaWrapper.h"
namespace Plaza {
	static VkDescriptorType PlBufferTypeToVkDescriptorType(PlazaBufferType type) {
		VkDescriptorType convertedType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		switch (type) {
		case PL_BUFFER_UNIFORM_BUFFER:
			convertedType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
		case PL_BUFFER_STORAGE_BUFFER:
			convertedType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		case PL_BUFFER_STORAGE_BUFFER_DYNAMIC:
			convertedType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			break;
		case PL_BUFFER_SAMPLER:
			convertedType = VK_DESCRIPTOR_TYPE_SAMPLER;
			break;
		case PL_BUFFER_COMBINED_IMAGE_SAMPLER:
			convertedType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			break;
		}
		return convertedType;
	}

	static VkImageType PlTextureTypeToVkImageType(PlazaTextureType type) {
		VkImageType convertedType = VK_IMAGE_TYPE_2D;
		switch (type) {
		case PL_TYPE_1D:
			convertedType = VK_IMAGE_TYPE_1D;
			break;
		case PL_TYPE_2D:
			convertedType = VK_IMAGE_TYPE_2D;
			break;
		case PL_TYPE_3D:
			convertedType = VK_IMAGE_TYPE_3D;
			break;
		}
		return convertedType;
	}

	static VkFormat PlImageFormatToVkFormat(PlazaTextureFormat format) {
		VkFormat convertedFormat = VK_FORMAT_R8G8B8_UNORM;
		switch (format) {
		case PL_FORMAT_R32G32B32A32_SFLOAT:
			convertedFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		case PL_FORMAT_R32G32B32_SFLOAT:
			convertedFormat = VK_FORMAT_R32G32B32_SFLOAT;
			break;
		case PL_FORMAT_R8G8B8A8_UNORM:
			convertedFormat = VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case PL_FORMAT_R8G8B8_UNORM:
			convertedFormat = VK_FORMAT_R8G8B8_UNORM;
			break;
		case PL_FORMAT_D32_SFLOAT_S8_UINT:
			convertedFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
			break;
		default:
			assert(false && "Unhandled PlazaTextureFormat case");
			break;
		}
		return convertedFormat;
	}

	static VkImageUsageFlags PlImageUsageToVkImageUsage(PlazaImageUsage imageUsage) {
		VkImageUsageFlags convertedUsage = 0;
		if (imageUsage & PL_IMAGE_USAGE_TRANSFER_SRC) {
			convertedUsage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}
		if (imageUsage & PL_IMAGE_USAGE_TRANSFER_DST) {
			convertedUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}
		if (imageUsage & PL_IMAGE_USAGE_SAMPLED) {
			convertedUsage = VK_IMAGE_USAGE_SAMPLED_BIT;
		}
		if (imageUsage & PL_IMAGE_USAGE_STORAGE) {
			convertedUsage = VK_IMAGE_USAGE_STORAGE_BIT;
		}
		if (imageUsage & PL_IMAGE_USAGE_COLOR_ATTACHMENT) {
			convertedUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}
		if (imageUsage & PL_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT) {
			convertedUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		if (imageUsage & PL_IMAGE_USAGE_INPUT_ATTACHMENT) {
			convertedUsage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		}
		return convertedUsage;
	}

	static VkShaderStageFlags PlRenderStageToVkShaderStage(PlazaRenderStage stage) {
		VkShaderStageFlags newStage{};
		if (stage & PL_STAGE_ALL) {
			newStage = VK_SHADER_STAGE_ALL;
		}
		if (stage & PL_STAGE_VERTEX) {
			newStage |= VK_SHADER_STAGE_VERTEX_BIT;
		}
		if (stage & PL_STAGE_FRAGMENT) {
			newStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		if (stage & PL_STAGE_COMPUTE) {
			newStage |= VK_SHADER_STAGE_COMPUTE_BIT;
		}
		return newStage;
	}

	static VkMemoryPropertyFlags PlMemoryPropertyToVkMemoryProperty(PlazaMemoryProperty memoryProperties) {
		VkMemoryPropertyFlags convertedPropertyFlags = 0;

		if (memoryProperties & PL_MEMORY_PROPERTY_DEVICE_LOCAL) {
			convertedPropertyFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}
		if (memoryProperties & PL_MEMORY_PROPERTY_HOST_VISIBLE) {
			convertedPropertyFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		}
		if (memoryProperties & PL_MEMORY_PROPERTY_HOST_COHERENT) {
			convertedPropertyFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}
		if (memoryProperties & PL_MEMORY_PROPERTY_HOST_CACHED) {
			convertedPropertyFlags |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		}
		if (memoryProperties & PL_MEMORY_PROPERTY_LAZILY_ALLOCATED) {
			convertedPropertyFlags |= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
		}
		if (memoryProperties & PL_MEMORY_PROPERTY_PROTECTED) {
			convertedPropertyFlags |= VK_MEMORY_PROPERTY_PROTECTED_BIT;
		}

		return convertedPropertyFlags;
	}

	static VkBufferUsageFlags PlBufferUsageToVkBufferUsage(PlazaBufferUsage usage) {
		VkBufferUsageFlags convertedUsage{};
		if (usage & PL_BUFFER_USAGE_TRANSFER_SRC) {
			convertedUsage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		}
		if (usage & PL_BUFFER_USAGE_TRANSFER_DST) {
			convertedUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}
		if (usage & PL_BUFFER_USAGE_UNIFORM_BUFFER) {
			convertedUsage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		if (usage & PL_BUFFER_USAGE_STORAGE_BUFFER) {
			convertedUsage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		if (usage & PL_BUFFER_USAGE_INDEX_BUFFER) {
			convertedUsage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (usage & PL_BUFFER_USAGE_VERTEX_BUFFER) {
			convertedUsage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		if (usage & PL_BUFFER_USAGE_INDIRECT_BUFFER) {
			convertedUsage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}
		return convertedUsage;
	}

	static VmaMemoryUsage PlMemoryUsageToVmaMemoryUsage(PlazaMemoryUsage usage) {
		VmaMemoryUsage convertedUsage = VMA_MEMORY_USAGE_UNKNOWN;
		switch (usage) {
		case PL_MEMORY_USAGE_GPU_ONLY:
			convertedUsage = VMA_MEMORY_USAGE_GPU_ONLY;
			break;
		case PL_MEMORY_USAGE_CPU_ONLY:
			convertedUsage = VMA_MEMORY_USAGE_CPU_ONLY;
			break;
		case PL_MEMORY_USAGE_CPU_TO_GPU:
			convertedUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
			break;
		case PL_MEMORY_USAGE_GPU_TO_CPU:
			convertedUsage = VMA_MEMORY_USAGE_GPU_TO_CPU;
			break;
		case PL_MEMORY_USAGE_CPU_COPY:
			convertedUsage = VMA_MEMORY_USAGE_CPU_COPY;
			break;
		case PL_MEMORY_USAGE_GPU_LAZILY_ALLOCATED:
			convertedUsage = VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;
			break;
		case PL_MEMORY_USAGE_AUTO:
			convertedUsage = VMA_MEMORY_USAGE_AUTO;
			break;
		case PL_MEMORY_USAGE_AUTO_PREFER_DEVICE:
			convertedUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			break;
		case PL_MEMORY_USAGE_AUTO_PREFER_HOST:
			convertedUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
			break;
		}
		return convertedUsage;
	}

	class VulkanBufferBinding : public PlazaBufferBinding {
	public:
		VulkanBufferBinding(uint64_t descriptorCount, uint8_t binding, PlazaBufferType type, PlazaRenderStage stage, uint64_t maxItems, uint16_t stride, uint8_t bufferCount, const std::string& name)
			: PlazaBufferBinding(descriptorCount, binding, type, stage, maxItems, stride, bufferCount, name) {};
		virtual void Compile() override;
		virtual void Destroy() override;

		VkDescriptorSetLayoutBinding GetDescriptorLayout() {
			return plvk::descriptorSetLayoutBinding(mBinding, mDescriptorCount, PlBufferTypeToVkDescriptorType(mType), nullptr, PlRenderStageToVkShaderStage(mStage));
		}

		VkDescriptorBufferInfo GetBufferInfo(unsigned int currentFrame) {
			return  plvk::descriptorBufferInfo(mBuffer->GetBuffer(currentFrame), 0, mMaxItems * mStride);
		}

		VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet& descriptorSet, VkDescriptorBufferInfo* bufferInfo) {
			if (mType == PL_BUFFER_PUSH_CONSTANTS)
				return {};

			return plvk::writeDescriptorSet(descriptorSet, this->mBinding, 0, PlBufferTypeToVkDescriptorType(mType), mDescriptorCount, nullptr, bufferInfo);
		}

		std::shared_ptr<PlVkBuffer> mBuffer = nullptr;
	private:
	};

	class VulkanTextureBinding : public PlazaTextureBinding {
	public:
		VulkanTextureBinding(uint64_t descriptorCount, uint8_t location, uint8_t binding, PlazaBufferType bufferType, PlazaImageUsage imageUsage, PlazaRenderStage renderStage, PlazaTextureType type, PlazaTextureFormat format, glm::vec3 resolution, uint8_t mipCount, uint16_t layersCount, const std::string& name)
			: PlazaTextureBinding(descriptorCount, location, binding, bufferType, imageUsage, renderStage, type, format, resolution, mipCount, layersCount, name) {};
		virtual void Compile() override;
		virtual void Destroy() override;

		VkDescriptorSetLayoutBinding GetDescriptorLayout() {
			return plvk::descriptorSetLayoutBinding(mBinding, mDescriptorCount, PlBufferTypeToVkDescriptorType(mBufferType), nullptr, PlRenderStageToVkShaderStage(mStage));
		}

		VkDescriptorImageInfo GetImageInfo() {
			if (mTexture == nullptr)
				return VkDescriptorImageInfo{};

			return plvk::descriptorImageInfo(mTexture.get()->GetLayout(), mTexture.get()->mImageView, mTexture.get()->mSampler);
		}

		VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet& descriptorSet, VkDescriptorImageInfo* imageInfo) {
			return plvk::writeDescriptorSet(descriptorSet, this->mBinding, 0, PlBufferTypeToVkDescriptorType(mBufferType), mMaxBindlessResources > 0 ? 1 : mDescriptorCount, imageInfo, nullptr);
		}

		std::shared_ptr<VulkanTexture> mTexture = nullptr;
	private:
	};

	class VulkanRenderPass : public PlazaRenderPass {
	public:
		VulkanRenderPass(std::string name, int stage) : PlazaRenderPass(name, stage) {}
		//std::vector<std::shared_ptr<VulkanPlazaPipeline>> mPipelines = std::vector<std::shared_ptr<VulkanPlazaPipeline>>();

		virtual void Compile() override;
		virtual void BindRenderPass() override;

		VulkanRenderPass* AddInputResource(std::shared_ptr<PlazaShadersBinding> resource) {
			mInputBindings.emplace(resource->mName, resource);
			return this;
		}

		VulkanRenderPass* AddOutputResource(std::shared_ptr<PlazaShadersBinding> resource) {
			mOutputBindings.emplace(resource->mName, resource);
			return this;
		}

		inline void UpdateCommandBuffer(VkCommandBuffer& commandBuffer) {
			mCommandBuffer = commandBuffer;
		}

		VkRenderPass mRenderPass = VK_NULL_HANDLE;
		VkFramebuffer mFrameBuffer = VK_NULL_HANDLE;

		std::vector<VkDescriptorSet> mDescriptorSets = std::vector<VkDescriptorSet>();
		VkDescriptorSetLayout mDescriptorSetLayout;
	private:
		VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
	};

	class VulkanRenderGraph : public PlazaRenderGraph {
	public:
		void Execute(uint8_t imageIndex, uint8_t currentFrame) override;
		bool BindPass(std::string passName) override;
		//void Compile() override;

		void UpdateCommandBuffer(VkCommandBuffer& commandBuffer) {
			mCommandBuffer = &commandBuffer;
		}

		VulkanRenderPass* GetRenderPass(std::string name) {
			if (mPasses.find(name) != mPasses.end())
				return (VulkanRenderPass*)mPasses.find(name)->second.get();
			return nullptr;
		}

		void BuildDefaultRenderGraph() override;

	private:
		VkCommandBuffer* mCommandBuffer = VK_NULL_HANDLE;
	};
}