#pragma once
namespace Plaza {
	class VulkanShaders {
	public:
		std::string mVertexShaderPath;
		std::string mFragmentShaderPath;
		std::string mGeometryShaderPath;

		VulkanShaders(std::string vertexPath, std::string fragmentPath, std::string geometryPath) : mVertexShaderPath(vertexPath), mFragmentShaderPath(fragmentPath), mGeometryShaderPath(geometryPath){};

		VkPipeline mPipeline;
		VkPipelineLayout mPipelineLayout;
		VkDescriptorSet mDescriptorSet;
		VkDescriptorSetLayout mDescriptorSetLayout;

		void Init(VkDevice device, VkRenderPass renderPass, int width, int height, VkDescriptorSetLayout descriptorSetLayout, std::vector<VkPushConstantRange> pushConstantRanges = std::vector<VkPushConstantRange>());
		void Terminate();
	};
}