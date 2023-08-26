#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
void Plaza::ApplicationClass::Callbacks::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

