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

		VkImageView CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, unsigned int layerCount = 1, unsigned int baseMipLevel = 0);
		void CreateTextureSampler(VkSamplerAddressMode adressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT, VkSamplerMipmapMode mipMapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR, VkFilter magFilter = VK_FILTER_LINEAR, VkFilter minFilter = VK_FILTER_LINEAR);
		void GenerateMipmaps(VkImage image, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, VkFormat format);
		bool CreateTextureImage(VkDevice& device, std::string path, VkFormat& format, bool generateMipMaps = false);
		bool CreateTextureImage(VkDevice device, VkFormat format, int width, int height, bool generateMipMaps = false, VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
		void InitDescriptorSet();
		void InitDescriptorSetLayout();

		VkFormat GetFormat();
		void SetFormat(VkFormat newFormat);

		VkImageLayout GetLayout() {
			return this->mLayout;
		}
		VkImageLayout mLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	private:
		VkFormat mFormat;
		VkBuffer mStagingBuffer;
		VkDeviceMemory mStagingBufferMemory;

		void Load(std::string path) override;
		unsigned int GetTextureID() override;
		ImTextureID GetImGuiTextureID() override;


		friend VulkanRenderer;
	};
}