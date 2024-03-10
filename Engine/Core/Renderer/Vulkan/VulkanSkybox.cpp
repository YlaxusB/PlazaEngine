#include <Engine/Core/PreCompiledHeaders.h>
#include "VulkanSkybox.h"
#include "VulkanPostEffects.h"

namespace Plaza {
	void VulkanSkybox::Init() {
		this->mResolution = Application->appSizes->sceneSize;
		this->mSkyboxPostEffect = new VulkanPostEffects();
		std::string vertexPath = VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\skybox.vertex");
		std::string fragmentPath = VulkanShadersCompiler::Compile(Application->enginePath + "\\Shaders\\Vulkan\\skybox\\skybox.fragment");
		this->mSkyboxPostEffect->Init(vertexPath, fragmentPath, "", VulkanRenderer::GetRenderer()->mDevice, Application->appSizes->sceneSize, this->mDescriptorSetLayouts[0], this->mPipelineLayoutInfo);
	}

	void VulkanSkybox::DrawSkybox() {
		PLAZA_PROFILE_SECTION("Draw Skybox");
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = this->mSkyboxPostEffect->mRenderPass;
		renderPassInfo.framebuffer = this->mFramebuffer;//mSwapChainFramebuffers[0];//mSwapChainFramebuffers[imageIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent.width = this->mResolution.x;
		renderPassInfo.renderArea.extent.height = this->mResolution.y;

		vkCmdBeginRenderPass(this->mCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(this->mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->mSkyboxPostEffect->mShaders->mPipeline);

		//this->UpdateAndPushConstants(mCommandBuffer);
		vkCmdBindDescriptorSets(this->mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->mSkyboxPostEffect->mShaders->mPipelineLayout, 0, 1, &this->mDescriptorSets[Application->mRenderer->mCurrentFrame], 0, nullptr);

		VkDeviceSize offsets[] = { 0, 0 };

		pushData.projection = Application->activeCamera->GetProjectionMatrix();
		pushData.view = Application->activeCamera->GetViewMatrix();
		vkCmdPushConstants(this->mCommandBuffer, this->mSkyboxPostEffect->mShaders->mPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(VulkanRenderer::PushConstants), &pushData);
		vkCmdDraw(this->mCommandBuffer, 36, 1, 0, 0);

		vkCmdEndRenderPass(mCommandBuffer);
	}

	void VulkanSkybox::UpdateAndPushConstants(VkCommandBuffer commandBuffer) {

	}

	void VulkanSkybox::Termiante() {

	}

}