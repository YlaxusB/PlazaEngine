#include "CallbacksHeader.h"
void Engine::Application::Callbacks::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

