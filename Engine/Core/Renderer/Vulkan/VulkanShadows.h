#pragma once 
#include "Engine/Core/Renderer/Shadows.h"
#include "Engine/Core/Renderer/Vulkan/VulkanShaders.h"
namespace Plaza {
	class VulkanShadows : Shadows {
	public:
		unsigned int mCascadeCount = 9;
		glm::vec3 mLightDirection = glm::normalize(glm::vec3(20.0f, 50, 20.0f));

		struct PushConstants {
			glm::vec4 position;
			unsigned int cascadeIndex;
		};
		VulkanShadows::PushConstants pushConstants{};

		struct Cascade {
			VkDescriptorSet mDescriptorSet;
			VkImageView mImageView;
			VkFramebuffer mFramebuffer;
		};
		std::vector<Cascade> mCascades = std::vector<Cascade>();
		struct ShadowsUniformBuffer {
			glm::mat4 lightSpaceMatrices[32];
		};
		ShadowsUniformBuffer mUbo;

		struct ShadowDepthBuffer {
			VulkanTexture texture;
			VkFramebuffer mFramebuffer;
		};

		VkFormat mDepthFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;

		unsigned int mShadowResolution = 4096;
		std::vector<ShadowDepthBuffer> shadowsBuffers = std::vector<ShadowDepthBuffer>();
		std::vector<float> shadowCascadeLevels;

		unsigned int mDepthDebugLayer = 0;
		VulkanShaders* mDepthDebugShaders = nullptr;
		VkRenderPass mDepthDebugRenderPass = VK_NULL_HANDLE;
		VkFramebuffer mDepthDebugFramebuffer = VK_NULL_HANDLE;
		VkPipelineLayout mDebugDepthPipelineLayout = VK_NULL_HANDLE;
		VkPipeline mDebugDepthPipeline = VK_NULL_HANDLE;
		VkImageView mDebugDepthImageView = VK_NULL_HANDLE;
		VkDescriptorSetLayout mDebugDepthDescriptorLayout = VK_NULL_HANDLE;
		VkDescriptorSet mDebugDepthDescriptorSet = VK_NULL_HANDLE;
		VkImage mDebugDepthImage = VK_NULL_HANDLE;


		unsigned int shadowBufferCount = 5;
		VulkanShaders* mShadowsShader = nullptr;;
		VkBuffer mUniformBuffer = VK_NULL_HANDLE;
		VkDeviceMemory mUniformBufferMemory = VK_NULL_HANDLE;
		void* mUniformBufferMapped = VK_NULL_HANDLE;

		VkRenderPass mRenderPass = VK_NULL_HANDLE;
		VkSampler mShadowsSampler = VK_NULL_HANDLE;
		VkImage mShadowDepthImage = VK_NULL_HANDLE;
		std::vector<VkImageView> mShadowDepthImageViews = std::vector<VkImageView>();
		VkFramebuffer mFramebuffer = VK_NULL_HANDLE;

		VkDescriptorSet mDescriptorSet = VK_NULL_HANDLE;
		VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
		void Init() override;
		void RenderToShadowMap() override;
		void Terminate() override;
		void UpdateUniformBuffer();

		void InitializeBuffers(VulkanRenderer& renderer);
		void InitializeRenderPass(VulkanRenderer& renderer);
		void CreateDescriptorPool(VkDevice device);
		void CreateDescriptorSetLayout(VkDevice device);
		void CreateDescriptorSet(VkDevice device);
		void UpdateAndPushConstants(VkCommandBuffer commandBuffer, unsigned int cascadeIndex);
	private:
		glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane);
		std::vector<glm::mat4> GetLightSpaceMatrices(std::vector<float>shadowCascadeLevels, VulkanShadows::ShadowsUniformBuffer& ubo);
	};
}