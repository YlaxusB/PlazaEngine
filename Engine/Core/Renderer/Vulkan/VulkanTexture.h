#pragma once
#include "Renderer.h"
#include <stb_image.h>
#include <string>
namespace Plaza {
	class VulkanTexture : public Texture {
	public:
		VkDescriptorSet GetDescriptorSet() override;
		VkDescriptorSet mDescriptorSet;
		int mIndexHandle = -1;
		static int mLastBindingIndex;

	private:
		VkDescriptorSetLayout mDescriptorSetLayout;
		VkImage mImage;
		VkDeviceMemory mImageMemory;
		VkImageView mImageView;
		VkSampler mSampler;

		void CreateTextureImageView();
		VkImageView CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags);
		void CreateTextureSampler();

		VkBuffer mStagingBuffer;
		VkDeviceMemory mStagingBufferMemory;

		void CreateTextureImage(VkDevice device, std::string path);

		void Load(std::string path) override;
		unsigned int GetTextureID() override;
		ImTextureID GetImGuiTextureID() override;

		void InitDescriptorSet();
		void InitDescriptorSetLayout();

		friend VulkanRenderer;
	};
}