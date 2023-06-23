#pragma once
#include <glm/glm.hpp>

#include "Engine/Application/ApplicationSizes.h"
#include "Engine/Components/Core/Camera.h"
#include "Engine/Gui/guiMain.h"
#include "Engine/Application/EditorCamera.h"
#include "Engine/Shaders/Shader.h"


namespace Engine {
	class Camera;

	class Application {
	public:
		static ApplicationSizes appSizes; // = new ApplicationSizes();
		static ApplicationSizes lastAppSizes;

		static Camera editorCamera;
		static Camera& activeCamera;

		static unsigned int textureColorbuffer;

		static Shader shader;
		static Shader pickingShader;
		static Shader skyboxShader;
		static Shader outlineShader;

		static GLFWwindow* window;

		static void CreateApplication();
		static GLFWwindow* InitApplication();
		static GLFWwindow* InitGLFWWindow();

		static void InitOpenGL();

		static void InitSkybox();


		static void Loop();
		static void Terminate();
		static void Render();

	private:
		/* Callbacks */
		static void drop_callback(GLFWwindow* window, int count, const char** paths);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void processInput(GLFWwindow* window);
		static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

		// MUST REVIEW
		static void updateBuffers(GLuint textureColorBuffer, GLuint rbo);
	};
}
