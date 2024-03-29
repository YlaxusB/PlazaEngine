#pragma once
#include "Engine/Vendor/GLFW/glfw3.h"
#include "Engine/Application/Application.h"
namespace Plaza {
    class WindowClass {
    public:
        WindowClass() {
            glfwWindow = InitGLFWWindow();
        }
        GLFWwindow* glfwWindow;
        GLFWwindow* InitGLFWWindow();
    };
}