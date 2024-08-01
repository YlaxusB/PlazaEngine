#pragma once
#include "Renderer.h"
#include "Engine/Core/Renderer/RenderGraph.h"
#include "VulkanPlazaPipeline.h"
namespace Plaza {
	class VulkanBufferBinding : public PlazaBufferBinding {
	public:
		VulkanBufferBinding(const std::string& name, uint8_t location, uint8_t binding, PlazaBufferType type)
			: PlazaBufferBinding(name, location, binding, type) {};
		void Compile() override {};
		void Destroy() override {};

	private:
	};

	class VulkanTextureBinding : public PlazaTextureBinding {
	public:
		VulkanTextureBinding(const std::string& name, uint8_t location, uint8_t binding, PlazaTextureType type, PlazaTextureFormat format, glm::vec3 resolution, uint8_t mipCount, uint16_t layersCount)
			: PlazaTextureBinding(name, location, binding, type, format, resolution, mipCount, layersCount) {};
		void Compile() override {};
		void Destroy() override {};

	private:
	};

	class VulkanRenderPass : public PlazaRenderPass {
	public:
		VulkanRenderPass(std::string name, int stage) : PlazaRenderPass(name, stage) {}
		//std::vector<std::shared_ptr<VulkanPlazaPipeline>> mPipelines = std::vector<std::shared_ptr<VulkanPlazaPipeline>>();

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
		VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
	private:
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

		void BuildDefaultRenderGraph() override {
			this->AddRenderPass(std::make_shared<VulkanRenderPass>("Deferred Geometry Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT))
				->AddInputResource(std::make_shared<VulkanBufferBinding>("UniformBufferObject", 0, 0, PlazaBufferType::PL_BUFFER_UNIFORM_BUFFER))
				->AddOutputResource(std::make_shared<VulkanTextureBinding>("GPosition", 0, 0, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddOutputResource(std::make_shared<VulkanTextureBinding>("GDiffuse", 1, 0, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddOutputResource(std::make_shared<VulkanTextureBinding>("GNormal", 2, 0, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddOutputResource(std::make_shared<VulkanTextureBinding>("GOthers", 3, 0, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1));

			this->AddRenderPass(std::make_shared<VulkanRenderPass>("Deferred Lighting Pass", PL_STAGE_VERTEX | PL_STAGE_FRAGMENT))
				->AddInputResource(std::make_shared<VulkanTextureBinding>("GPosition", 0, 0, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddInputResource(std::make_shared<VulkanTextureBinding>("GDiffuse", 0, 1, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32A32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddInputResource(std::make_shared<VulkanTextureBinding>("GNormal", 0, 2, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddInputResource(std::make_shared<VulkanTextureBinding>("GOthers", 0, 3, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1))
				->AddOutputResource(std::make_shared<VulkanTextureBinding>("SceneTexture", 0, 0, PlazaTextureType::PL_TYPE_2D, PlazaTextureFormat::PL_FORMAT_R32G32B32_SFLOAT, glm::vec3(2560, 1080, 1), 1, 1));
		}
	private:
		VkCommandBuffer* mCommandBuffer = VK_NULL_HANDLE;
	};
}