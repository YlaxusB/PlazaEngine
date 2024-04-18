#include "Engine/Core/PreCompiledHeaders.h"
#include "VulkanPostEffects.h"

namespace Plaza {
	void VulkanPostEffects::InitializeShaders(std::string vertexPath, std::string fragmentPath, std::string geometryPath, VkDevice device, glm::vec2 size, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayoutCreateInfo pipelineLayoutInfo) {
		this->mShaders = new VulkanShaders(vertexPath, fragmentPath, geometryPath);
		this->mShaders->Init(device, this->mRenderPass, size.x, size.y, descriptorSetLayout, pipelineLayoutInfo, std::vector<VkPushConstantRange>(), true);
	}

	void VulkanPostEffects::Init(std::string vertexPath, std::string fragmentPath, std::string geometryPath, VkDevice device, glm::vec2 size, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayoutCreateInfo pipelineLayoutInfo) {
		this->InitializeShaders(vertexPath, fragmentPath, geometryPath, device, size, descriptorSetLayout, pipelineLayoutInfo);
	}

	void VulkanPostEffects::Update() {

	}

	void VulkanPostEffects::DrawFullScreenRectangle() {

	}

	void VulkanPostEffects::Terminate() {

	}
}