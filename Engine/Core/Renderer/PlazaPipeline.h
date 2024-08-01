#pragma once

namespace Plaza {
	class PlazaPipeline {
	public:
		PlazaPipeline() = default;
		virtual void Init(std::string vertexPath, std::string fragmentPath, std::string geometryPath, VkDevice device, glm::vec2 size, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayoutCreateInfo pipelineLayoutInfo) {};
		virtual void InitializeShaders(std::string vertexPath, std::string fragmentPath, std::string geometryPath, VkDevice device, glm::vec2 size, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayoutCreateInfo pipelineLayoutInfo) {};
		virtual void Update() {};
		virtual void DrawFullScreenRectangle() {};
		virtual void Terminate() {};
	};
}