#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "Engine/Application/ApplicationSizes.h"
#include "Engine/Components/Core/Camera.h"

#include "Engine/Application/EditorCamera.h"
#include "Engine/Shaders/Shader.h"
#include "Engine/GUI/guiMain.h"
#include "Engine/Application/PickingTexture.h"
//#include "Engine/Application/Callbacks/CallbacksHeader.h"
namespace Engine {
	class Camera;

	class Application {
	public:
		static ApplicationSizes appSizes; // = new ApplicationSizes();
		static ApplicationSizes lastAppSizes;

		static Camera editorCamera;
		static Camera& activeCamera;

		static unsigned int textureColorbuffer;
		static unsigned int edgeDetectionColorBuffer;
		static unsigned int edgeDetectionFramebuffer;
		static unsigned int edgeDetectionDepthStencilRBO;
		static unsigned int blurColorBuffer;
		static unsigned int blurFramebuffer;
		static unsigned int blurDepthStencilRBO;
		static unsigned int selectedColorBuffer;
		static unsigned int selectedFramebuffer;
		static unsigned int selectedDepthStencilRBO;
		static unsigned int frameBuffer;
		static unsigned int rbo;

		static unsigned int blurVAO;
		static unsigned int blurVBO;

		static unsigned int verticalBlurColorBuffer;
		static unsigned int horizontalBlurColorBuffer;

		static PickingTexture* pickingTexture;

		static Shader* shader;
		static Shader* pickingShader;
		static Shader* outlineShader;
		static Shader* outlineBlurShader;
		static Shader* edgeDetectionShader;
		static Shader* combiningShader;
		static Shader* singleColorShader;

		static GLFWwindow* window;

		static void CreateApplication();
		static GLFWwindow* InitApplication();

		static void InitOpenGL();

		static void InitSkybox();


		static void Loop();
		static void Terminate();
		static void Render();

		class Window;
		class Callbacks;

		// MUST REVIEW
		static void updateBuffers(GLuint textureColorBuffer, GLuint rbo);

		static void InitBlur();
	};
}
