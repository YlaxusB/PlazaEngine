#include "Window.h"

#include "Engine/Application/Application.h"

//#include "Engine/Application/Callbacks/Callbacks.h"
#include "Engine/Application/Callbacks/CallbacksHeader.h"
//using namespace Engine;
//using Engine::Application;

//using namespace Engine;
//using Engine::Application;
using Engine::Application;
namespace Engine {
	GLFWwindow* Application::Window::InitGLFWWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

		// --------- MUST CHANGE THE WAY IT'S STARTING ON SECOND MONITOR --------- //

		// Get monitors and create GLFW window on second monitor
		int monitorCount;
		GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
		GLFWmonitor* secondMonitor = monitors[1];
		const GLFWvidmode* videoMode = glfwGetVideoMode(secondMonitor);
		GLFWwindow* window = glfwCreateWindow(appSizes.appSize.x, appSizes.appSize.y, "OpenGLEngine", secondMonitor, NULL);
		// Make it be fullscreen
		glfwMakeContextCurrent(window);
		glfwSetWindowMonitor(window, nullptr, 1, 10, appSizes.appSize.x, appSizes.appSize.y, GLFW_DONT_CARE);
		glfwSetWindowPos(window, -2560, 0);
		glfwMaximizeWindow(window);

		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return nullptr;
		}
		// Set callbacks
		glfwSetFramebufferSizeCallback(window, Callbacks::framebufferSizeCallback);
		glfwSetCursorPosCallback(window, Callbacks::mouseCallback);
		glfwSetScrollCallback(window, Callbacks::scrollCallback);
		glfwSetMouseButtonCallback(window, Callbacks::mouseButtonCallback);
		glfwSetDropCallback(window, Callbacks::dropCallback);


		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return nullptr;
		}

		stbi_set_flip_vertically_on_load(false);
		return window;
	}
}