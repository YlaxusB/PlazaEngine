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


		VulkanPostEffects* mSkyboxPostEffect = nullptr;
		VkFramebuffer mFramebuffer = VK_NULL_HANDLE;
		std::vector<VkDescriptorSet> mDescriptorSets = std::vector<VkDescriptorSet>();
		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts = std::vector<VkDescriptorSetLayout>();
		VkPipelineLayoutCreateInfo mPipelineLayoutInfo{};
		VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
		void Init() override;
		void DrawSkybox() override;
		void Termiante() override;

	private:
		void UpdateAndPushConstants(VkCommandBuffer commandBuffer);
	};
}