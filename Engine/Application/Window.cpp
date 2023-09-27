#include "Window.h"

#include "Engine/Core/PreCompiledHeaders.h"

//#include "Engine/Application/Callbacks/Callbacks.h"
#include "Engine/Application/Callbacks/CallbacksHeader.h"
//using namespace Plaza;
//using Application->;

//using namespace Plaza;
//using Application->;
using Plaza::ApplicationClass;
namespace Plaza {
	GLFWwindow* WindowClass::InitGLFWWindow() {
		std::cout << "G1 \n";
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		glfwWindowHint(GLFW_SAMPLES, 4);
		std::cout << "G2 \n";
		// --------- MUST CHANGE THE WAY IT'S STARTING ON SECOND MONITOR --------- //

		// Get monitors and create GLFW window on second monitor
		int monitorCount;
		GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
		GLFWmonitor* secondMonitor = monitors[0];
		//const GLFWvidmode* videoMode = glfwGetVideoMode(secondMonitor);
		const GLFWvidmode* mode = glfwGetVideoMode(secondMonitor);
		std::cout << "G3 \n";
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, 5000);
		std::cout << "G4 \n";
		GLFWwindow* window = glfwCreateWindow(Application->appSizes->appSize.x, Application->appSizes->appSize.y, "Plaza Engine", monitors[0], NULL);
		if (!window) {
			// Handle window creation failure
			// You can print an error message or log it
			std::cout << "error \n";
			glfwTerminate(); // Clean up GLFW before exiting
			return nullptr;
		}
		std::cout << "G5 \n";
		// Make it be fullscreen
		glfwMakeContextCurrent(window);
		std::cout << "G6 \n";
		glfwSetWindowMonitor(window, NULL, 0, 0, Application->appSizes->appSize.x, Application->appSizes->appSize.y, GLFW_DONT_CARE);
		std::cout << "G7 \n";
		//glfwSetWindowPos(window, -2560, 0);
		//glfwMaximizeWindow(window);
		glfwMaximizeWindow(window);
		std::cout << "G8 \n";
		glfwSetWindowPos(window, 0, 0);


		std::cout << "G9 \n";
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return nullptr;
		}
		// Set callbacks
		glfwSetFramebufferSizeCallback(window, ApplicationClass::Callbacks::framebufferSizeCallback);
		glfwSetCursorPosCallback(window, ApplicationClass::Callbacks::mouseCallback);
		glfwSetScrollCallback(window, ApplicationClass::Callbacks::scrollCallback);
		glfwSetMouseButtonCallback(window, ApplicationClass::Callbacks::mouseButtonCallback);
		glfwSetDropCallback(window, ApplicationClass::Callbacks::dropCallback);
		glfwSetKeyCallback(window, ApplicationClass::Callbacks::keyCallback);

		glfwSwapInterval(0);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return nullptr;
		}

		stbi_set_flip_vertically_on_load(false);
		return window;
	}
}