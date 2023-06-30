#pragma once
#include <GLFW/glfw3.h>
#include "Engine/Application/Application.h"
namespace Engine {
    class Window {
    public:
        Window() {
            glfwWindow = InitGLFWWindow();
        }
        GLFWwindow* glfwWindow;
        GLFWwindow* InitGLFWWindow();
    };
}