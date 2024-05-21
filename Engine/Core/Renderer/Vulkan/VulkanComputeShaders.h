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
			glm::vec4 u_threshold;
			glm::vec2 u_texel_size;
			int u_mip_level;
			bool u_use_threshold;
		};

		void CreateComputeDescriptorSetLayout();

		void Init(std::string shadersPath, unsigned int pushConstantsCount = 0, VkPushConstantRange pushConstantRange = VkPushConstantRange{});
		void RunCompute();
		void Draw();
		void Dispatch(int x, int y, int z, void* pushConstantData = nullptr, unsigned int pushConstantSize = 0, VkDescriptorSet descriptorSet = VK_NULL_HANDLE);
		void Terminate();

		std::vector<VkBuffer> mShaderStorageBuffers;
		std::vector<VkDeviceMemory> mShaderStorageBuffersMemory;
		VkDescriptorSetLayout mComputeDescriptorSetLayout;
		std::vector<VkWriteDescriptorSet> mDescriptorWrites{};
		std::vector<VkDescriptorSet> mComputeDescriptorSets;
	private:
		VkPipelineLayout mComputePipelineLayout;
		VkPipeline mComputePipeline;

		std::vector<VkBuffer> mUniformBuffers;
		std::vector<VkDeviceMemory> mUniformBuffersMemory;
		std::vector<void*> mUniformBuffersMapped;
	};
}