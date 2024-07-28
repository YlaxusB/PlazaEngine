#pragma once

namespace Plaza {
	class PlazaPipeline {
	public:
		PlazaPipeline() = default;
		virtual void Init(std::string vertexPath, std::string fragmentPath, std::string geometryPath, VkDevice device, glm::vec2 size, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayoutCreateInfo pipelineLayoutInfo) = 0;
		virtual void InitializeShaders(std::string vertexPath, std::string fragmentPath, std::string geometryPath, VkDevice device, glm::vec2 size, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayoutCreateInfo pipelineLayoutInfo) = 0;
		virtual void Update() = 0;
		virtual void DrawFullScreenRectangle() = 0;
		virtual void Terminate() = 0;
	};
}