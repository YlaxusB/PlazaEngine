#pragma once
#include <Engine/Core/Renderer/ComputeShaders.h>

namespace Plaza {
	class VulkanComputeShaders {
	public:
		struct Particle {
			glm::vec2 position;
			glm::vec2 velocity;
			glm::vec4 color;

			static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Particle, position);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Particle, color);

				return attributeDescriptions;
			}
		};

		struct UniformBufferObject {
			float deltaTime = 1.0f;
		};

		void CreateComputeDescriptorSetLayout();

		void Init(std::string shadersPath);
		void RunCompute();
		void Draw();
		void Terminate();

		std::vector<VkBuffer> mShaderStorageBuffers;
		std::vector<VkDeviceMemory> mShaderStorageBuffersMemory;
	private:
		std::vector<VkDescriptorSet> mComputeDescriptorSets;
		VkDescriptorSetLayout mComputeDescriptorSetLayout;
		VkPipelineLayout mComputePipelineLayout;
		VkPipeline mComputePipeline;

		std::vector<VkBuffer> mUniformBuffers;
		std::vector<VkDeviceMemory> mUniformBuffersMemory;
		std::vector<void*> mUniformBuffersMapped;
	};
}