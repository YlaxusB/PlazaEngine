#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
void Plaza::ApplicationClass::Callbacks::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	if (Application->mRenderer->api == RendererAPI::OpenGL)
	{
		glViewport(0, 0, width, height);
	}
	else if (Application->mRenderer->api == RendererAPI::Vulkan) {
		VulkanRenderer& renderer = *(VulkanRenderer*)(Application->mRenderer);
		renderer.mFramebufferResized = true;
	}
}

