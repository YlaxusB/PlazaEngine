#pragma once
#include <Engine/Core/Renderer/Skybox.h>
#include "VulkanPlazaPipeline.h"

namespace Plaza {
	class VulkanSkybox : public Skybox {
	public:
		struct PushConstants {
			glm::mat4 projection;
			glm::mat4 view;
			float skyboxIntensity;
			float gamma;
			float exposure;
		} pushData;

		struct ConverterPushConstants {
			glm::mat4 mvp;
			bool first;
			float deltaPhi = (2.0f * float(3.14159265358979323846)) / 180.0f;
			float deltaTheta = (0.5f * float(3.14159265358979323846)) / 64.0f;
			float roughness = 1.0f;
			unsigned int numSamples = 32u;
		};

		VulkanMesh* mSkyboxMesh = nullptr;
		std::vector<std::string> mSkyboxPaths = std::vector<std::string>(6);
		VkFormat mSkyboxFormat = VK_FORMAT_R32G32B32A32_SFLOAT;//VK_FORMAT_B8G8R8A8_UNORM;

		VulkanPlazaPipeline* mSkyboxPostEffect = nullptr;
		std::vector<VkFramebuffer> mFramebuffers = std::vector<VkFramebuffer>();
		std::vector<VkDescriptorSet> mDescriptorSets = std::vector<VkDescriptorSet>();
		VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayoutCreateInfo mPipelineLayoutInfo{};
		VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
		void Init() override;
		void DrawSkybox() override;
		void Terminate() override;

		VulkanTexture* mIrradianceTexture = nullptr;
		VulkanTexture* mPreFilteredTexture = nullptr;
	private:
		glm::vec2 mScreenSize;
		VkBuffer mStagingBuffer;
		VkDeviceMemory mStagingBufferMemory;

		VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
		VkSampler mSkyboxSampler = VK_NULL_HANDLE;
		VkImage mSkyboxImage = VK_NULL_HANDLE;
		std::vector<VkImageView> mSkyboxImageViews = std::vector<VkImageView>();
		void InitializeImageSampler();
		void InitializeImageView();
		void InitializeDescriptorPool();
		void InitializeDescriptorSets();
		void InitializeRenderPass();
		void GenerateSkyboxMipmaps(VkCommandBuffer commandBuffer, VkImage image, int32_t width, int32_t height, uint32_t mipLevels);
		void GeneratePreFilteredEnvironment();
		void InitializeIrradiance();
		void UpdateAndPushConstants(VkCommandBuffer commandBuffer);

		const std::vector<glm::mat4> matrices = {
			// POSITIVE_X
			glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			// NEGATIVE_X
			glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			// POSITIVE_Y
			glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			// NEGATIVE_Y
			glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			// POSITIVE_Z
			glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
			// NEGATIVE_Z
			glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		};
	};
}