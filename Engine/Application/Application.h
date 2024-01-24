#pragma once
//#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Application/ApplicationSizes.h"
#include "Engine/Components/Core/Camera.h"
#include "Engine/Application/Application.h"

#include "Engine/Application/EditorCamera.h"
#include "Engine/Shaders/Shader.h"
#include "Editor/GUI/guiMain.h"
#include "Engine/Application/PickingTexture.h"
#include "Engine/Core/Lighting/Shadows/Shadows.h"
#include "Engine/Application/Window.h"
#include "Engine/Editor/Editor.h"
#include "Editor/GUI/ProjectManager/ProjectManager.h"
//#include "Engine/Application/Callbacks/CallbacksHeader.h"
#include "Engine/Core/Scene.h"
#include "Editor/Project.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/FrameBuffer.h"

#include "Engine/Core/Renderer/Renderer.h"
#include "Engine/Core/Renderer/OpenGL/Renderer.h"
#include "Engine/Core/Renderer/Vulkan/Renderer.h"
namespace Plaza {
	class Camera;
	class WindowClass;
	class ShadowsClass;
	//class WindowClass;
	class ApplicationClass {
	public:
		Renderer* mRenderer = nullptr;
		RendererAPI mRendererAPI;

		Scene* editorScene = new Scene();
		Scene* runtimeScene = new Scene();
		Scene* activeScene = editorScene;
		bool runningScene = false;
		int drawCalls = 0;
		bool runningEditor = true;

		std::string exeDirectory;
		std::string projectPath;
		std::string dllPath;
		std::string enginePath;
		std::string editorPath;
		std::string enginePathAppData;
		Editor::Project* activeProject = nullptr;

		std::string focusedMenu = "Scene";
		std::string hoveredMenu = "Scene";

		bool copyingScene = false;

		Camera* editorCamera;
		Camera* activeCamera;
		ApplicationClass(); // Initialize the activeCamera reference
		ApplicationSizes* appSizes = new ApplicationSizes(); // = new ApplicationSizes();
		ApplicationSizes* lastAppSizes = appSizes;
		Plaza::Editor::ProjectManagerGui* projectManagerGui = new Plaza::Editor::ProjectManagerGui();

		unsigned int frameBuffer, textureColorbuffer, rbo = 0;
		unsigned int geometryFramebuffer, gPosition, gNormal, gDiffuse, gOthers, gDepth, geometryRboDepth = 0;
		unsigned int edgeDetectionFramebuffer, edgeDetectionColorBuffer, edgeDetectionDepthStencilRBO = 0;
		unsigned int blurFramebuffer, blurColorBuffer, blurDepthStencilRBO = 0;
		unsigned int selectedFramebuffer, selectedColorBuffer, selectedDepthStencilRBO = 0;
		unsigned int hdrFramebuffer, hdrSceneColor, hdrBloomColor = 0;
		unsigned int pick = 0;

		FrameBuffer* distortionCorrectionFrameBuffer = nullptr;
		Shader* distortionCorrectionShader = nullptr;

		// Shadows
		ShadowsClass* Shadows;

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
		Shader* shadowsDepthShader = nullptr;
		Shader* debugDepthShader = nullptr;
		Shader* hdrShader = nullptr;
		Shader* textRenderingShader = nullptr;

		bool runProjectManagerGui = true;
		////   bool runEngine = false;
		bool runEngine = true;

		void CreateApplication();
		static GLFWwindow* InitApplication();

		static void InitOpenGL();
		static void InitShaders();

		static void InitSkybox();


		static void UpdateEngine();
		static void UpdateProjectManagerGui();

		void Loop();
		static void Terminate();
		static void Render();

		std::vector<Material> materials = std::vector<Material>();

		EngineClass* engine = new EngineClass();
		WindowClass* Window;
		class Callbacks;

		// MUST REVIEW
		static void updateBuffers(GLuint textureColorBuffer, GLuint rbo);

		static void InitBlur();
	};

	extern ApplicationClass* Application;
}
