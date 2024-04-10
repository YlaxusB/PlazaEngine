#pragma once
#include <Engine/Core/Renderer/Skybox.h>
#include "VulkanPostEffects.h"

namespace Plaza {
	class VulkanSkybox : public Skybox {
	public:
		struct PushConstants {
			glm::mat4 projection;
			glm::mat4 view;
		} pushData;
		VulkanMesh* mSkyboxMesh = nullptr;
		std::vector<std::string> mSkyboxPaths = std::vector<std::string>(6);
		VkFormat mSkyboxFormat = VK_FORMAT_B8G8R8A8_UNORM;

		VulkanPostEffects* mSkyboxPostEffect = nullptr;
		std::vector<VkFramebuffer> mFramebuffers = std::vector<VkFramebuffer>();
		std::vector<VkDescriptorSet> mDescriptorSets = std::vector<VkDescriptorSet>();
		VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayoutCreateInfo mPipelineLayoutInfo{};
		VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
		void Init() override;
		void DrawSkybox() override;
		void Terminate() override;

	private:
		VkBuffer mStagingBuffer;
		VkDeviceMemory mStagingBufferMemory;

		VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
		VkSampler mSkyboxSampler = VK_NULL_HANDLE;
		VkImage mSkyboxImage = VK_NULL_HANDLE;
		std::vector<VkImageView> mSkyboxImageViews = std::vector<VkImageView>();
		void InitializeImageSampler();
		void InitializeImageView();
		void InitializeDescriptorPool();
		void InitializeDescriptorSets();
		void InitializeRenderPass();
		void UpdateAndPushConstants(VkCommandBuffer commandBuffer);
	};
}