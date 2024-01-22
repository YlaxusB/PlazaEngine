#pragma once
#include "Renderer.h"
#include <stb_image.h>
#include <string>
namespace Plaza {
	class VulkanTexture : public Texture {
        VkDescriptorSet mDescriptorSet;
        VkDescriptorSetLayout mDescriptorSetLayout;
        VkImage mImage;
        VkImageView mImageView;
        VkSampler mSampler;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;

        void CreateTextureImageView();
        VkImageView CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags);
        void CreateTextureSampler();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        void CreateTextureImage(VkDevice device, std::string path);

        void Load(std::string path) override;
        unsigned int GetTextureID() override;

        void InitDescriptorSet();
        void InitDescriptorSetLayout();

        friend VulkanRenderer;
	};
}