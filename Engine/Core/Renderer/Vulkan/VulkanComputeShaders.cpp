#include <Engine/Core/PreCompiledHeaders.h>
#include "VulkanComputeShaders.h"
#include "VulkanShaders.h"

namespace Plaza {
	void VulkanComputeShaders::CreateComputeDescriptorSetLayout() {
		std::array<VkDescriptorSetLayoutBinding, 3> layoutBindings{};
		layoutBindings[0].binding = 0;
		layoutBindings[0].descriptorCount = 1;
		layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		layoutBindings[0].pImmutableSamplers = nullptr;
		layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		layoutBindings[1].binding = 1;
		layoutBindings[1].descriptorCount = 1;
		layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		layoutBindings[1].pImmutableSamplers = nullptr;
		layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		layoutBindings[2].binding = 2;
		layoutBindings[2].descriptorCount = 1;
		layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		layoutBindings[2].pImmutableSamplers = nullptr;
		layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 3;
		layoutInfo.pBindings = layoutBindings.data();

		if (vkCreateDescriptorSetLayout(VulkanRenderer::GetRenderer()->mDevice, &layoutInfo, nullptr, &mComputeDescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute descriptor set layout!");
		}
	}
	void VulkanComputeShaders::Init(std::string shadersPath) {
		if (mComputeDescriptorSetLayout == VK_NULL_HANDLE)
			CreateComputeDescriptorSetLayout();
		VkShaderModule computeShaderModule = VulkanShaders::CreateShaderModule(VulkanShaders::ReadFile(VulkanShadersCompiler::Compile(shadersPath)), VulkanRenderer::GetRenderer()->mDevice);

		VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
		computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		computeShaderStageInfo.module = computeShaderModule;
		computeShaderStageInfo.pName = "main";

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &mComputeDescriptorSetLayout;

		if (vkCreatePipelineLayout(VulkanRenderer::GetRenderer()->mDevice, &pipelineLayoutInfo, nullptr, &mComputePipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute pipeline layout!");
		}

		mShaderStorageBuffers.resize(Application->mRenderer->mMaxFramesInFlight);
		mShaderStorageBuffersMemory.resize(Application->mRenderer->mMaxFramesInFlight);

		// Initialize particles
		std::default_random_engine rndEngine((unsigned)time(nullptr));
		std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

#define PARTICLE_COUNT 3000
		// Initial particle positions on a circle
		std::vector<Particle> particles(PARTICLE_COUNT);
		for (auto& particle : particles) {
			float r = 0.25f * sqrt(rndDist(rndEngine));
			float theta = rndDist(rndEngine) * 2 * 3.14159265358979323846;
			float x = r * cos(theta) * Application->appSizes->sceneSize.x / Application->appSizes->sceneSize.y;
			float y = r * sin(theta);
			particle.position = glm::vec2(x, y);
			particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.00025f;
			particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
		}

		VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_COUNT;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VulkanRenderer::GetRenderer()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(VulkanRenderer::GetRenderer()->mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, particles.data(), (size_t)bufferSize);
		vkUnmapMemory(VulkanRenderer::GetRenderer()->mDevice, stagingBufferMemory);

		for (size_t i = 0; i < Application->mRenderer->mMaxFramesInFlight; i++) {
			VulkanRenderer::GetRenderer()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mShaderStorageBuffers[i], mShaderStorageBuffersMemory[i]);
			// Copy data from the staging buffer (host) to the shader storage buffer (GPU)
			VulkanRenderer::GetRenderer()->CopyBuffer(stagingBuffer, mShaderStorageBuffers[i], bufferSize, 0);
		}

		std::array<VkDescriptorSetLayoutBinding, 3> layoutBindings{};
		layoutBindings[0].binding = 0;
		layoutBindings[0].descriptorCount = 1;
		layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		layoutBindings[0].pImmutableSamplers = nullptr;
		layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		layoutBindings[1].binding = 1;
		layoutBindings[1].descriptorCount = 1;
		layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		layoutBindings[1].pImmutableSamplers = nullptr;
		layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		layoutBindings[2].binding = 2;
		layoutBindings[2].descriptorCount = 1;
		layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		layoutBindings[2].pImmutableSamplers = nullptr;
		layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 3;
		layoutInfo.pBindings = layoutBindings.data();

		//if (vkCreateDescriptorSetLayout(VulkanRenderer::GetRenderer()->mDevice, &layoutInfo, nullptr, &mComputeDescriptorSetLayout) != VK_SUCCESS) {
		//	throw std::runtime_error("failed to create compute descriptor set layout!");
		//}

		//bufferSize = sizeof(UniformBufferObject);
		//
		//mUniformBuffers.resize(VulkanRenderer::GetRenderer()->mMaxFramesInFlight);
		//mUniformBuffersMemory.resize(VulkanRenderer::GetRenderer()->mMaxFramesInFlight);
		//mUniformBuffersMapped.resize(VulkanRenderer::GetRenderer()->mMaxFramesInFlight);
		//
		//for (size_t i = 0; i < VulkanRenderer::GetRenderer()->mMaxFramesInFlight; i++) {
		//	VulkanRenderer::GetRenderer()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mUniformBuffers[i], mUniformBuffersMemory[i]);
		//
		//	vkMapMemory(VulkanRenderer::GetRenderer()->mDevice, mUniformBuffersMemory[i], 0, bufferSize, 0, &mUniformBuffersMapped[i]);
		//}

		//std::array<VkDescriptorPoolSize, 2> poolSizes{};
		//poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//poolSizes[0].descriptorCount = static_cast<uint32_t>(Application->mRenderer->mMaxFramesInFlight);
		//
		//poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		//poolSizes[1].descriptorCount = static_cast<uint32_t>(Application->mRenderer->mMaxFramesInFlight) * 2;
		//
		//VkDescriptorPoolCreateInfo poolInfo{};
		//poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		//poolInfo.poolSizeCount = 2;
		//poolInfo.pPoolSizes = poolSizes.data();
		//poolInfo.maxSets = static_cast<uint32_t>(Application->mRenderer->mMaxFramesInFlight);
		//
		//VkDescriptorPool descriptorPool;
		//if (vkCreateDescriptorPool(VulkanRenderer::GetRenderer()->mDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		//	throw std::runtime_error("failed to create descriptor pool!");
		//}

		//mComputeDescriptorSets.resize(Application->mRenderer->mMaxFramesInFlight);
		//std::vector<VkDescriptorSetLayout> layouts(Application->mRenderer->mMaxFramesInFlight, mComputeDescriptorSetLayout);
		//VkDescriptorSetAllocateInfo allocInfo{};
		//allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		//allocInfo.descriptorPool = descriptorPool;
		//allocInfo.descriptorSetCount = static_cast<uint32_t>(Application->mRenderer->mMaxFramesInFlight);
		//allocInfo.pSetLayouts = layouts.data();
		//
		//if (vkAllocateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, &allocInfo, mComputeDescriptorSets.data()) != VK_SUCCESS) {
		//	throw std::runtime_error("failed to allocate descriptor sets!");
		//}
		//for (size_t i = 0; i < VulkanRenderer::GetRenderer()->mMaxFramesInFlight; i++) {		
		//	vkUpdateDescriptorSets(VulkanRenderer::GetRenderer()->mDevice, mDescriptorWrites.size(), mDescriptorWrites.data(), 0, nullptr);
		//}

		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = mComputePipelineLayout;
		pipelineInfo.stage = computeShaderStageInfo;

		if (vkCreateComputePipelines(VulkanRenderer::GetRenderer()->mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mComputePipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create compute pipeline!");
		}

		vkDestroyShaderModule(VulkanRenderer::GetRenderer()->mDevice, computeShaderModule, nullptr);
	}

	void VulkanComputeShaders::RunCompute() {


		UniformBufferObject ubo{};
		//ubo.deltaTime = Time::deltaTime * 1000.0f;

		memcpy(mUniformBuffersMapped[Application->mRenderer->mCurrentFrame], &ubo, sizeof(ubo));

		vkCmdBindPipeline(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipeline);

		vkCmdBindDescriptorSets(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipelineLayout, 0, 1, &mComputeDescriptorSets[VulkanRenderer::GetRenderer()->mCurrentFrame], 0, nullptr);

		vkCmdDispatch(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer, PARTICLE_COUNT / 256, 1, 1);
	}

	void VulkanComputeShaders::Dispatch(int x, int y, int z) {
		vkCmdBindPipeline(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipeline);

		uint32_t offsets[1] = { 0 };
		vkCmdBindDescriptorSets(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipelineLayout, 0, 1, &mComputeDescriptorSets[VulkanRenderer::GetRenderer()->mCurrentFrame], 1, offsets);

		vkCmdDispatch(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer, x, y, z);
	}

	void VulkanComputeShaders::Draw() {
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer, 0, 1, &mShaderStorageBuffers[VulkanRenderer::GetRenderer()->mCurrentFrame], offsets);

		vkCmdDraw(*VulkanRenderer::GetRenderer()->mActiveCommandBuffer, PARTICLE_COUNT, 1, 0, 0);
	}

	void VulkanComputeShaders::Terminate() {

	}

}