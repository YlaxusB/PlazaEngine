#pragma once
#include "Renderer.h"
#include "Engine/Core/Renderer/RenderGraph.h"
#include "VulkanPlazaPipeline.h"
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
		std::vector<std::shared_ptr<VulkanPlazaPipeline>> mPipelines;

		void Execute() override;

		VulkanRenderPass* AddInputResource(std::shared_ptr<PlazaShadersBinding> resource) {
			mInputBindings.emplace(resource->mName, resource);
			return this;
		}

		VulkanRenderPass* AddOutputResource(std::shared_ptr<PlazaShadersBinding> resource) {
			mOutputBindings.emplace(resource->mName, resource);
			return this;
		}

		VkRenderPass mRenderPass;
		VkFramebuffer mFrameBuffer;
	};

	class VulkanRenderGraph : public PlazaRenderGraph {
	public:
		void Execute(uint8_t imageIndex, uint8_t currentFrame) override;
		bool BindPass(std::string passName) override;
		//void Compile() override;

		std::map<std::string, std::shared_ptr<VulkanRenderPass>> mPasses;

		void UpdateCommandBuffer(VkCommandBuffer& commandBuffer) {
			mCommandBuffer = &commandBuffer;
		}
	private:
		VkCommandBuffer* mCommandBuffer;
	};
}