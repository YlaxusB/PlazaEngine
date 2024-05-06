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

		VkDescriptorSetLayout mDescriptorSetLayout;
		VkImage mImage;
		VkDeviceMemory mImageMemory;
		VkImageView mImageView;
		VkSampler mSampler;

		void CreateTextureImageView();
		VkImageView CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, unsigned int layerCount = 1);
		void CreateTextureSampler();
		void GenerateMipmaps(VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, VkFormat format);
		void CreateTextureImage(VkDevice& device, std::string path, VkFormat& format, bool generateMipMaps = false);
		void CreateTextureImage(VkDevice device, VkFormat format, int width, int height, bool generateMipMaps = false);
		void InitDescriptorSet();
		void InitDescriptorSetLayout();
	private:
		VkBuffer mStagingBuffer;
		VkDeviceMemory mStagingBufferMemory;

		void Load(std::string path) override;
		unsigned int GetTextureID() override;
		ImTextureID GetImGuiTextureID() override;


		friend VulkanRenderer;
	};
}