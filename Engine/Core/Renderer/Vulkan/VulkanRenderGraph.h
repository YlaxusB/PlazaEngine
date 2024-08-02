#pragma once
#include "Renderer.h"
#include "Engine/Core/Renderer/RenderGraph.h"
#include "VulkanPlazaPipeline.h"
#include "VulkanPlazaInitializator.h"
#include "VulkanTexture.h"
#include "VulkanPlazaWrapper.h"
namespace Plaza {
	static VkDescriptorType PlBufferTypeToVkDescriptorType(PlazaBufferType type) {
		switch (type) {
		case PL_BUFFER_UNIFORM_BUFFER:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case PL_BUFFER_STORAGE_BUFFER:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case PL_BUFFER_STORAGE_BUFFER_DYNAMIC:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		case PL_BUFFER_SAMPLER:
			return VK_DESCRIPTOR_TYPE_SAMPLER;
		case PL_BUFFER_COMBINED_IMAGE_SAMPLER:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		}
	}

	static VkImageType PlTextureTypeToVkImageType(PlazaTextureType type) {
		switch (type) {
		case PL_TYPE_1D:
			return VK_IMAGE_TYPE_1D;
		case PL_TYPE_2D:
			return VK_IMAGE_TYPE_2D;
		case PL_TYPE_3D:
			return VK_IMAGE_TYPE_3D;
		}
	}

	static VkFormat PlImageFormatToVkFormat(PlazaTextureFormat format) {
		switch (format) {
		case PL_FORMAT_R32G32B32A32_SFLOAT:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case PL_FORMAT_R32G32B32_SFLOAT:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case PL_FORMAT_R8G8B8A8_UNORM:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case PL_FORMAT_R8G8B8_UNORM:
			return VK_FORMAT_R8G8B8_UNORM;
		default:
			assert(false && "Unhandled PlazaTextureFormat case");
			break;
		}
	}

	static VkShaderStageFlags PlRenderStageToVkShaderStage(PlazaRenderStage stage) {
		VkShaderStageFlags newStage{};
		if (stage & PL_STAGE_ALL) {
			newStage |= VK_SHADER_STAGE_ALL;
		}
		if (stage & PL_STAGE_VERTEX) {
			newStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		if (stage & PL_STAGE_FRAGMENT) {
			newStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		if (stage & PL_STAGE_COMPUTE) {
			newStage |= VK_SHADER_STAGE_COMPUTE_BIT;
		}
		return newStage;
	}

	class VulkanBufferBinding : public PlazaBufferBinding {
	public:
		VulkanBufferBinding(uint64_t descriptorCount, uint8_t binding, PlazaBufferType type, PlazaRenderStage stage, const std::string& name)
			: PlazaBufferBinding(descriptorCount, binding, type, stage, name) {};
		virtual void Compile() override;
		virtual void Destroy() override;

		VkDescriptorSetLayoutBinding GetDescriptorLayout() {
			return plvk::descriptorSetLayoutBinding(mBinding, mDescriptorCount, PlBufferTypeToVkDescriptorType(mType), nullptr, PlRenderStageToVkShaderStage(mStage));
		}

		VkDescriptorBufferInfo GetBufferInfo() {
			return  plvk::descriptorBufferInfo(mBuffer->GetBuffer(), 0, mStride);
		}

		VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet& descriptorSet, VkDescriptorBufferInfo& bufferInfo) {
			if (mType == PL_BUFFER_PUSH_CONSTANTS)
				return {};

			return plvk::writeDescriptorSet(descriptorSet, this->mBinding, 0, PlBufferTypeToVkDescriptorType(mType), mDescriptorCount, nullptr, &bufferInfo);
		}

	private:
		std::shared_ptr<PlVkBuffer> mBuffer = nullptr;
	};

	class VulkanTextureBinding : public PlazaTextureBinding {
	public:
		VulkanTextureBinding(const std::string& name, uint64_t descriptorCount, uint8_t location, uint8_t binding, PlazaBufferType bufferType, PlazaTextureType type, PlazaTextureFormat format, glm::vec3 resolution, uint8_t mipCount, uint16_t layersCount)
			: PlazaTextureBinding(name, descriptorCount, location, binding, bufferType, type, format, resolution, mipCount, layersCount) {};
		virtual void Compile() override;
		virtual void Destroy() override;

		VkDescriptorSetLayoutBinding GetDescriptorLayout() {
			return plvk::descriptorSetLayoutBinding(mBinding, mDescriptorCount, PlBufferTypeToVkDescriptorType(mBufferType), nullptr, PlRenderStageToVkShaderStage(mStage));
		}

		VkDescriptorImageInfo GetImageInfo() {
			return plvk::descriptorImageInfo(mTexture.get()->GetLayout(), mTexture.get()->mImageView, mTexture.get()->mSampler);;
		}

		VkWriteDescriptorSet GetDescriptorWrite(VkDescriptorSet& descriptorSet, VkDescriptorImageInfo& imageInfo) {
			return plvk::writeDescriptorSet(descriptorSet, this->mBinding, 0, PlBufferTypeToVkDescriptorType(mBufferType), mDescriptorCount, &imageInfo, nullptr);
		}

	private:
		std::shared_ptr<VulkanTexture> mTexture = nullptr;
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