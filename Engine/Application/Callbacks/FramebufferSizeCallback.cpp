#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
void Plaza::Callbacks::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	if (Application::Get()->mRenderer->api == RendererAPI::Vulkan) {
		VulkanRenderer& renderer = *(VulkanRenderer*)(Application::Get()->mRenderer);
		renderer.mFramebufferResized = true;
	}
}

