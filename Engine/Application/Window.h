#pragma once
#define GLFW_INCLUDE_VULKAN
#include "ThirdParty/GLFW/include/GLFW/glfw3.h"
#include "Engine/Application/Application.h"
namespace Plaza {
    class WindowClass {
    public:
        GLFWwindow* glfwWindow;
        GLFWwindow* InitGLFWWindow();
    };
}