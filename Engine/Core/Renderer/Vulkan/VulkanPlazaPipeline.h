#pragma once
#include "Engine/Core/Renderer/PlazaPipeline.h"
#include "VulkanShaders.h"

namespace Plaza {
	class VulkanPlazaPipeline : public PlazaPipeline {
	public:
		VkRenderPass mRenderPass = VK_NULL_HANDLE;
		VkFramebuffer mFramebuffer = VK_NULL_HANDLE;
		VulkanShaders* mShaders = new VulkanShaders();
		void Init(std::string vertexPath, std::string fragmentPath, std::string geometryPath, VkDevice device, glm::vec2 size, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayoutCreateInfo pipelineLayoutInfo) override;
		void InitializeShaders(std::string vertexPath, std::string fragmentPath, std::string geometryPath, VkDevice device, glm::vec2 size, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayoutCreateInfo pipelineLayoutInfo) override;
		void InitializeFramebuffer(VkImageView* pAttachmentsData, uint32_t attachmentsCount, glm::vec2 size, uint32_t layers = 1);
		void Update() override;
		void DrawFullScreenRectangle() override;
		void Terminate() override;

		void UpdateCommandBuffer(VkCommandBuffer commandBuffer);
	private:
		VkCommandBuffer mCommandBuffer;
	};
}