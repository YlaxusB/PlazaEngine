#include "CallbacksHeader.h"
#include "Engine/Application/EntryPoint.h"
void Engine::ApplicationClass::Callbacks::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

