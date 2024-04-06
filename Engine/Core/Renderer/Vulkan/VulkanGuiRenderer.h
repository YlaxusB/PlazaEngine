#pragma once
#include <Engine/Core/Renderer/GuiRenderer.h>
#include "VulkanPostEffects.h"

#include <sstream>
#include <iomanip>

#include "../stb_font_consolas_24_latin1.inl"
#define TEXTOVERLAY_MAX_CHAR_COUNT 2048

namespace Plaza {
	class VulkanGuiRenderer : public GuiRenderer {
	public:
		void Init() override;
		void RenderText(Drawing::UI::TextRenderer* textRendererComponent = nullptr) override;
		void Terminate() override;
		VkRenderPass mRenderPass;
	private:
		void InitializeRenderPass();

		VkPipelineLayoutCreateInfo mPipelineLayoutInfo{};
		VulkanPostEffects* mTextPipeline = nullptr;
		void PreparePipeline();
		VulkanRenderer* mRenderer;

		enum TextAlign { alignLeft, alignCenter, alignRight };
		void AddText(std::string text, float x, float y, float scale, TextAlign align);
		int numLetters = 0;

		VkSampler mSampler;
		VkImage mImage;
		VkImageView mImageView;
		VkDeviceMemory mImageMemory;
		// Character vertex buffer
		VkBuffer mBuffer;
		VkDeviceMemory mMemory;
		VkDescriptorPool mDescriptorPool;
		VkDescriptorSetLayout mDescriptorSetLayout;
		VkDescriptorSet mDescriptorSet;
		VkPipelineLayout mPipelineLayout;
		VkPipeline mPipeline;

		// Passed from the sample
		VkQueue mQueue;
		VkDevice* mDevice;
		uint32_t* frameBufferWidth;
		uint32_t* frameBufferHeight;
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		// Pointer to mapped vertex buffer
		glm::vec4* mapped = nullptr;
		stb_fontchar stbFontData[STB_FONT_consolas_24_latin1_NUM_CHARS];
		
		void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool pool, bool free);
	};
}