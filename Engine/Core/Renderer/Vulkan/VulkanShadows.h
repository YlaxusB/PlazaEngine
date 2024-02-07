#pragma once 
#include "Engine/Core/Renderer/Shadows.h"
#include "Engine/Core/Renderer/Vulkan/VulkanShaders.h"
namespace Plaza {
	class VulkanShadows : Shadows {
	public:
		struct ShadowsUniformBuffer {
			glm::mat4 lightSpaceMatrices[32];
		};

		struct ShadowDepthBuffer {
			VulkanTexture texture;
			VkFramebuffer mFramebuffer;
		};

		unsigned int mShadowResolution = 2048;
		std::vector<ShadowDepthBuffer> shadowsBuffers = std::vector<ShadowDepthBuffer>();
		std::vector<float> shadowCascadeLevels;

		unsigned int shadowBufferCount = 5;
		VulkanShaders* mShadowsShader;
		VkBuffer mUniformBuffer;
		VkDeviceMemory mUniformBufferMemory;
		void* mUniformBufferMapped;

		VkRenderPass mRenderPass;
		VkImage mShadowDepthImage;
		std::vector<VkImageView> mShadowDepthImageViews = std::vector<VkImageView>();
		VkFramebuffer mFramebuffer;

		VkDescriptorSet mDescriptorSet;
		VkDescriptorSetLayout mDescriptorSetLayout;
		VkDescriptorPool mDescriptorPool;
		void Init() override;
		void RenderToShadowMap() override;
		void Terminate() override;
		void UpdateUniformBuffer();

		void InitializeBuffers(VulkanRenderer& renderer);
		void InitializeRenderPass(VulkanRenderer& renderer);
		void CreateDescriptorPool(VkDevice device);
		void CreateDescriptorSetLayout(VkDevice device);
		void CreateDescriptorSet(VkDevice device);
	};
}