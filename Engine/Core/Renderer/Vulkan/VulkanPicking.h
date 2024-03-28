#pragma once
#include <Engine/Core/Renderer/Picking.h>
#include "VulkanPostEffects.h"

namespace Plaza {
	class VulkanPicking : public Picking {
	public:
		struct PushConstants {
			glm::mat4 projection;
			glm::mat4 view;
			unsigned int uuid1;
			unsigned int uuid2;
		} pushData;
		VulkanMesh* mSkyboxMesh = nullptr;
		std::vector<std::string> mSkyboxPaths = std::vector<std::string>(6);
		VkFormat mSkyboxFormat = VK_FORMAT_R8G8B8A8_UNORM;

		VulkanPostEffects* mRenderPickingTexturePostEffects = nullptr;
		VulkanPostEffects* mOutlinePostEffects = nullptr;
		std::vector<VkFramebuffer> mFramebuffers = std::vector<VkFramebuffer>();
		std::vector<VkDescriptorSet> mDescriptorSets = std::vector<VkDescriptorSet>();
		VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayoutCreateInfo mPipelineLayoutInfo{};
		VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
		void Init() override;
		void DrawSelectedObjectsUuid() override;
		void DrawOutline() override;
		uint64_t ReadPickingTexture(glm::vec2 position) override;
		uint64_t DrawAndRead(glm::vec2 position) override;
		void Terminate() override;

	private:
		VkBuffer mStagingBuffer;
		VkDeviceMemory mStagingBufferMemory;

		VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
		VkSampler mSkyboxSampler = VK_NULL_HANDLE;
		VkImage mSkyboxImage = VK_NULL_HANDLE;
		std::vector<VkImageView> mSkyboxImageViews = std::vector<VkImageView>();
		void InitializePicking();
		void InitializeOutline();
		void InitializeImageSampler();
		void InitializeImageView();
		void InitializeDescriptorPool();
		void InitializeDescriptorSets();
		void InitializeRenderPass();
		void UpdateAndPushConstants(VkCommandBuffer commandBuffer);
		void DrawMeshToPickingTexture(const MeshRenderer& meshRenderer, VkCommandBuffer& commandBuffer);
	};
}