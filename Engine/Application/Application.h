#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "Engine/Application/ApplicationSizes.h"
#include "Engine/Components/Core/Camera.h"

#include "Engine/Application/EditorCamera.h"
#include "Engine/Shaders/Shader.h"
#include "Engine/GUI/guiMain.h"
#include "Engine/Application/PickingTexture.h"
#include "Engine/Core/Lightning/Shadows/Shadows.h"
#include "Engine/Application/Window.h"
#include "Engine/Editor/Editor.h"
//#include "Engine/Application/Callbacks/CallbacksHeader.h"
namespace Engine {
	class Camera;
	class ApplicationClass {
	public:
		ApplicationClass();
		static ApplicationSizes appSizes; // = new ApplicationSizes();
		static ApplicationSizes lastAppSizes;

		Camera editorCamera;
		Camera& activeCamera;

		unsigned int frameBuffer, textureColorbuffer, rbo = 0;
		unsigned int edgeDetectionFramebuffer, edgeDetectionColorBuffer, edgeDetectionDepthStencilRBO = 0;
		unsigned int blurFramebuffer, blurColorBuffer, blurDepthStencilRBO = 0;
		unsigned int selectedFramebuffer, selectedColorBuffer, selectedDepthStencilRBO = 0;


		// Shadows
		Shadows Shadows;

		 unsigned int blurVAO = 0;
		 unsigned int blurVBO = 0;

		 unsigned int verticalBlurColorBuffer = 0;
		 unsigned int horizontalBlurColorBuffer = 0;

		 PickingTexture* pickingTexture = nullptr;

		 Shader* shader = nullptr;
		 Shader* pickingShader = nullptr;
		 Shader* outlineShader = nullptr;
		 Shader* outlineBlurShader = nullptr;
		 Shader* edgeDetectionShader = nullptr;
		 Shader* combiningShader = nullptr;
		 Shader* singleColorShader = nullptr;

		void CreateApplication();
		static GLFWwindow* InitApplication();

		static void InitOpenGL();

		static void InitSkybox();


		static void Loop();
		static void Terminate();
		static void Render();

		Window* Window;
		class Callbacks;

		// MUST REVIEW
		static void updateBuffers(GLuint textureColorBuffer, GLuint rbo);

		static void InitBlur();
	};
}
