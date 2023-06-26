#pragma once
#include <GLFW/glfw3.h>
#include "Engine/Application/Application.h"
namespace Engine {
    class Application::Window {
    public:
        static GLFWwindow* InitGLFWWindow();
    };
}