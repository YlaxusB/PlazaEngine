#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
void Engine::ApplicationClass::Callbacks::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

