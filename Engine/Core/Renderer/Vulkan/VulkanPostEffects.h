#pragma once
#include "Engine/Core/Renderer/PostEffects.h"
#include "VulkanShaders.h"

namespace Plaza {
	class VulkanPostEffects : public PostEffects {
	public:
		VkRenderPass mRenderPass = VK_NULL_HANDLE;
		VulkanShaders* mShaders = nullptr;
		void Init(std::string vertexPath, std::string fragmentPath, std::string geometryPath, VkDevice device, glm::vec2 size, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayoutCreateInfo pipelineLayoutInfo) override;
		void InitializeShaders(std::string vertexPath, std::string fragmentPath, std::string geometryPath, VkDevice device, glm::vec2 size, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayoutCreateInfo pipelineLayoutInfo) override;
		void Update() override;
		void DrawFullScreenRectangle(VkCommandBuffer commandBuffer) override;
		void Terminate() override;

	};
}