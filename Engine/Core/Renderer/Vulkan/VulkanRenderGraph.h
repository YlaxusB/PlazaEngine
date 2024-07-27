#pragma once
#include "Renderer.h"
#include "Engine/Core/Renderer/RenderGraph.h"
namespace Plaza {
	class VulkanBufferBinding : public PlazaBufferBinding {
	public:
		void Compile() override {};
		void Destroy() override {};

	private:
	};

	class VulkanTextureBinding : public PlazaTextureBinding {
	public:
		void Compile() override {};
		void Destroy() override {};

	private:
	};

	class VulkanRenderPass : public PlazaRenderPass {
	public:
		void Execute() override;

		VulkanRenderPass* AddInputResource(std::shared_ptr<PlazaShadersBinding> resource) {
			mInputBindings.emplace(resource->mName, resource);
			return this;
		}

		VulkanRenderPass* AddOutputResource(std::shared_ptr<PlazaShadersBinding> resource) {
			mOutputBindings.emplace(resource->mName, resource);
			return this;
		}
	};

	class VulkanRenderGraph : public PlazaRenderGraph {
	public:
		void Execute(uint8_t currentFrame) override;
		//void Compile() override;

		std::map<std::string, std::shared_ptr<VulkanRenderPass>> mPasses;

		void UpdateCommandBuffer(VkCommandBuffer commandBuffer) {
			mCommandBuffer = commandBuffer;
		}
	private:
		VkCommandBuffer mCommandBuffer;
	};
}