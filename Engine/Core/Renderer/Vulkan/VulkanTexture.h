#pragma once
#include "Renderer.h"
#include <stb_image.h>
#include <string>
namespace Plaza {
	class VulkanTexture : public Texture {
	public:
		VkDescriptorSet GetDescriptorSet() override;
		VkDescriptorSet mDescriptorSet;
		static int mLastBindingIndex;

		uint16_t mMipLevels = 8;
	private:
		VkDescriptorSetLayout mDescriptorSetLayout;
		VkImage mImage;
		VkDeviceMemory mImageMemory;
		VkImageView mImageView;
		VkSampler mSampler;

		void CreateTextureImageView();
		VkImageView CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags);
		void CreateTextureSampler();
		void GenerateMipmaps(VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, VkFormat format);

		VkBuffer mStagingBuffer;
		VkDeviceMemory mStagingBufferMemory;

		void CreateTextureImage(VkDevice device, std::string path, VkFormat format);

		void Load(std::string path) override;
		unsigned int GetTextureID() override;
		ImTextureID GetImGuiTextureID() override;

		void InitDescriptorSet();
		void InitDescriptorSetLayout();

		friend VulkanRenderer;
	};
}