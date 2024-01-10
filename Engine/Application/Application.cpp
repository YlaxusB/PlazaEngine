#include "Engine/Core/PreCompiledHeaders.h"

#include "Editor/GUI/guiMain.h"

#include "Engine/Application/PickingTexture.h"
#include "Engine/Components/Core/Camera.h"
#include "Engine/Components/Rendering/Mesh.h"
#include "Engine/Components/Core/Entity.h"
#include "Engine/Shaders/Shader.h"
#include "Editor/GUI/Style/EditorStyle.h"
#include "Engine/Core/Skybox.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Renderer/Renderer.h"
#include "Engine/Editor/Editor.h"
#include "Engine/Application/Window.h"
#include "Engine/Application/Callbacks/CallbacksHeader.h"
#include "Engine/Editor/Outline/Outline.h"
#include "Engine/Editor/Editor.h"

#include <cstdlib> // Include the appropriate header for _dupenv_s

//#include "ThirdParty/Tracy/tracy/TracyC.h"
#include "Engine/Components/Physics/RigidBody.h"
#include "Engine/Core/Physics.h"
#include "Engine/Components/Core/Camera.h"
#include "Engine/Core/Scripting/Mono.h"
#include "Editor/Filewatcher.h"
#include "Engine/Core/Input/Input.h"
#include "Engine/Core/Audio/Audio.h"
#include "Engine/Core/Lighting/ClusteredForward.h"
#include "Engine/Shaders/ComputeShader.h"
#include "Engine/Components/Rendering/Light.h"
#include "Engine/Core/Renderer/OpenGL/Bloom.h"
#include "Engine/Core/Renderer/OpenGL/ScreenSpaceReflections.h"


char* appdataValue;
size_t len;
errno_t err = _dupenv_s(&appdataValue, &len, "APPDATA");


//Plaza::ApplicationClass* Application;

void blurFramebuffer1();
void blurFramebuffer2();
void blurFramebuffer3();

using namespace Plaza;
/// ---------------------------------------------------------------------

//std::list<Model> models;

EditorStyle editorStyle;

using namespace Plaza::Editor;

#define DEFAULT_GRAPHICAL_API "Vulkan"

Plaza::ApplicationClass::ApplicationClass() {
	editorCamera = new Plaza::Camera(glm::vec3(0.0f, 0.0f, 5.0f));
	editorCamera->isEditorCamera = true;
	activeCamera = editorCamera;
}
//ApplicationClass* Application = nullptr;
void combineBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, Application->frameBuffer);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glViewport(Application->appSizes->sceneStart.x, Application->appSizes->sceneStart.y, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Application->textureColorbuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Application->edgeDetectionColorBuffer);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Application->rbo);
	// Load depth data into the texture

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Outline::blurringBuffer->depthBuffer);

	Application->combiningShader->use();
	Application->combiningShader->setInt("buffer1", 0);
	Application->combiningShader->setInt("buffer2", 1);
	Application->combiningShader->setInt("Depth0", 2);
	Application->combiningShader->setInt("Depth1", 3);
	//glUniform1i(glGetUniformLocation(Application->edgeDetectionShader->ID, "screenTexture"), 0);

	glBindVertexArray(Application->blurVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//Renderer::Render(*Application->shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ApplicationClass::InitShaders() {
	// Initialize Shaders
#ifdef  GAME_REL
	std::string shadersFolder = Application->projectPath;
#else
	std::string shadersFolder = Application->enginePath;
#endif //  GAME_REL

	std::cout << "ProjectPath2  : " << Application->projectPath << "\n";
	std::cout << "shadersFolder  : " << shadersFolder << "\n";

	Application->shader = new Shader((shadersFolder + "\\Shaders\\1.model_loadingVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\1.model_loadingFragment.glsl").c_str());
	constexpr const char* textureDiffuseUniform = "texture_diffuse";
	constexpr const char* textureSpecularUniform = "texture_specular";
	constexpr const char* textureNormalUniform = "texture_normal";
	constexpr const char* textureHeightUniform = "texture_height";
	glUniform1i(glGetUniformLocation(Application->shader->ID, textureDiffuseUniform), 0);
	glUniform1i(glGetUniformLocation(Application->shader->ID, textureSpecularUniform), 1);
	glUniform1i(glGetUniformLocation(Application->shader->ID, textureNormalUniform), 2);
	glUniform1i(glGetUniformLocation(Application->shader->ID, textureHeightUniform), 3);
	Application->shader->use();
	Application->shader->setInt("texture_diffuse", 0);
	Application->shader->setInt("texture_specular", 1);
	Application->shader->setInt("texture_normal", 2);
	Application->shader->setInt("texture_height", 3);
	Application->shader->setInt("texture_metalness", 4);
	Application->shader->setInt("texture_roughness", 5);



	Application->pickingShader = new Shader((shadersFolder + "\\Shaders\\picking\\pickingVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\picking\\pickingFragment.glsl").c_str());

	Application->outlineShader = new Shader((shadersFolder + "\\Shaders\\outlining\\outliningVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\outlining\\outliningFragment.glsl").c_str());

	Application->outlineBlurShader = new Shader((shadersFolder + "\\Shaders\\blur\\blurVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\blur\\blurFragment.glsl").c_str());

	Application->mRenderer->InitShaders(shadersFolder);


	Application->outlineBlurShader->use();

	Application->outlineBlurShader->setInt("sceneBuffer", 0);

	Application->outlineBlurShader->setInt("depthStencilTexture", 1);

	Application->outlineBlurShader->setInt("depthStencilTexture2", 2);

	Application->combiningShader = new Shader((shadersFolder + "\\Shaders\\combining\\combiningVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\combining\\combiningFragment.glsl").c_str());
	Application->edgeDetectionShader = new Shader((shadersFolder + "\\Shaders\\edgeDetection\\edgeDetectionVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\edgeDetection\\edgeDetectionFragment.glsl").c_str());

	Application->singleColorShader = new Shader((shadersFolder + "\\Shaders\\singleColor\\singleColorVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\singleColor\\singleColorFragment.glsl").c_str());

	Application->shadowsDepthShader = new Shader((shadersFolder + "\\Shaders\\shadows\\shadowsDepthVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\shadows\\shadowsDepthFragment.glsl").c_str(), (shadersFolder + "\\Shaders\\shadows\\shadowsDepthGeometry.glsl").c_str());

	//Application->debugDepthShader = new Shader((Application->enginePath + "\\Shaders\\debug\\debugDepthVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\debug\\debugDepthFragment.glsl").c_str());

	Application->hdrShader = new Shader((shadersFolder + "\\Shaders\\hdr\\hdrVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\hdr\\hdrFragment.glsl").c_str());

	Application->distortionCorrectionShader = new Shader((shadersFolder + "\\Shaders\\distortionCorrection\\distortionCorrectionVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\distortionCorrection\\distortionCorrectionFragment.glsl").c_str());

	Lighting::mLightAccumulationShader = new Shader((shadersFolder + "\\Shaders\\ClusteredForward\\accumulationVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\ClusteredForward\\accumulationFragment.glsl").c_str());
	Lighting::mLightMergerShader = new Shader((shadersFolder + "\\Shaders\\ClusteredForward\\lightMergerVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\ClusteredForward\\lightMergerFragment.glsl").c_str());
	Lighting::mLightMergerShader->use();
	Lighting::mLightMergerShader->setInt("gPosition", 0);
	Lighting::mLightMergerShader->setInt("gNormal", 1);
	Lighting::mLightMergerShader->setInt("gDiffuse", 2);
	Lighting::mLightMergerShader->setInt("gOthers", 3);

	Lighting::mLightSorterComputeShader = new ComputeShader((shadersFolder + "\\Shaders\\ClusteredForward\\lightSorterCompute.glsl").c_str());

	ScreenSpaceReflections::mScreenSpaceReflectionsShader = new Shader((shadersFolder + "\\Shaders\\screenSpaceReflections\\screenSpaceReflectionsVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\screenSpaceReflections\\screenSpaceReflectionsFragment.glsl").c_str());
	ScreenSpaceReflections::mScreenSpaceReflectionsShader->use();
	ScreenSpaceReflections::mScreenSpaceReflectionsShader->setInt("gPosition", 0);
	ScreenSpaceReflections::mScreenSpaceReflectionsShader->setInt("gNormal", 1);
	ScreenSpaceReflections::mScreenSpaceReflectionsShader->setInt("gDiffuse", 2);
	ScreenSpaceReflections::mScreenSpaceReflectionsShader->setInt("gOthers", 3);
	ScreenSpaceReflections::mScreenSpaceReflectionsShader->setInt("gDepth", 4);
	ScreenSpaceReflections::mScreenSpaceReflectionsShader->setInt("sceneColor", 5);

	Bloom::mBloomDownScaleShader = new ComputeShader((shadersFolder + "\\Shaders\\bloom\\bloomDownScaleCompute.glsl").c_str());
	Bloom::mBloomUpScaleShader = new ComputeShader((shadersFolder + "\\Shaders\\bloom\\bloomUpScaleCompute.glsl").c_str());
	Bloom::mBloomComputeShader = new ComputeShader((shadersFolder + "\\Shaders\\bloom\\bloomCompute.glsl").c_str());
	Bloom::mBloomBlendComputeShader = new ComputeShader((shadersFolder + "\\Shaders\\bloom\\bloomBlendCompute.glsl").c_str());
	Bloom::mBloomBrightSeparatorComputeShader = new ComputeShader((shadersFolder + "\\Shaders\\bloom\\bloomBrightSeparator.glsl").c_str());


	Application->textRenderingShader = new Shader((shadersFolder + "\\Shaders\\textRendering\\textRenderingVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\textRendering\\textRenderingFragment.glsl").c_str());

	Skybox::skyboxShader = new Shader((shadersFolder + "\\Shaders\\skybox\\skyboxVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\skybox\\skyboxFragment.glsl").c_str());
	Skybox::skyboxShader->use();
	Skybox::skyboxShader->setInt("skybox", 0);

	Application->distortionCorrectionFrameBuffer = new FrameBuffer(GL_FRAMEBUFFER);
	Application->distortionCorrectionFrameBuffer->InitColorAttachment(GL_TEXTURE_2D, GL_RGBA32F, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, GL_RGBA, GL_FLOAT, GL_LINEAR);
	//Application->distortionCorrectionFrameBuffer->InitRenderBufferObject(GL_DEPTH_COMPONENT, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);
}

void ApplicationClass::CreateApplication() {
	/* Get paths */
	std::filesystem::path currentPath(__FILE__);
	//Application->projectPath = currentPath.parent_path().parent_path().parent_path().string();
	Application->dllPath = currentPath.parent_path().parent_path().parent_path().string() + "\\dll";
	Application->enginePath = currentPath.parent_path().parent_path().string();
	Application->editorPath = currentPath.parent_path().parent_path().parent_path().string() + "\\Editor";
	Application->enginePathAppData = std::string(appdataValue) + "\\PlazaEngine\\";
	free(appdataValue);


	Application->Window = new Plaza::WindowClass();
	Application->Window->glfwWindow = Application->Window->InitGLFWWindow();


	if (Settings::mDefaultRendererAPI == RendererAPI::OpenGL) {
		Application->mRenderer = new OpenGLRenderer();
	}
	else if (Settings::mDefaultRendererAPI == RendererAPI::Vulkan) {
		Application->mRenderer = new VulkanRenderer();
	}

#ifdef GAME_REL	
	// Set the scene size to be the entire screen
	int width, height;
	glfwGetWindowSize(Application->Window->glfwWindow, &width, &height);
	Application->appSizes->sceneSize = glm::vec2(width, height);
#else

	/* Check if the engine app data folder doesnt exists, if not, then create */
	if (!std::filesystem::is_directory(Application->enginePathAppData) && Application->runningEditor) {
		std::filesystem::create_directory(Application->enginePathAppData);
		if (!std::filesystem::exists(Application->enginePathAppData + "\\cache.yaml")) {

		}
	}
#endif 

	// Initialize OpenGL, Shaders and Skybox
	InitShaders();

	if (Settings::mDefaultRendererAPI == RendererAPI::OpenGL)
		InitOpenGL();

	Application->mRenderer->Init();

	InitBlur();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Application->InitSkybox();

	//Initialize ImGui
#ifdef GAME_REL
#else
	std::cout << "Gui Initialized \n";
	Editor::Gui::Init(Application->Window->glfwWindow);
#endif // !GAME_REL



	/* Initialize clustered forward rendering */
	Lighting::InitializeClusters(12, 12, 12, Lighting::mClusters);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);
	std::uniform_real_distribution<float> dis2(-100.0f, 100.0f);

	for (int i = 0; i < 2000; i++) {
		glm::vec3 randomPos(dis2(gen), 0, dis2(gen));
		glm::vec3 randomColor(dis(gen), dis(gen), dis(gen));
		//Lighting::mLights.push_back(Lighting::LightStruct(randomPos, glm::vec3(randomColor), 1.0f, 1.0f, 1.0f));
	}
	Lighting::AssignLightsToClusters(Lighting::mLights, Lighting::mClusters);


	Lighting::CreateClusterBuffers(Lighting::mClusters);
}


void ApplicationClass::UpdateProjectManagerGui() {
	Application->projectManagerGui->Update();
}

void ApplicationClass::Loop() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	while (!glfwWindowShouldClose(Application->Window->glfwWindow)) {
		PLAZA_PROFILE_SECTION("Loop");
		// Run the Engine (Update Time, Shadows, Inputs, Buffers, Rendering, etc.)
		if (Application->runEngine) {
			Application->UpdateEngine();
		} // Run the Gui for selecting a project
		else if (Application->runProjectManagerGui) {
			Application->UpdateProjectManagerGui();
		}

		// GLFW
		{
			PLAZA_PROFILE_SECTION("Swap Buffers");
			glfwSwapBuffers(Application->Window->glfwWindow);
		}
		{
			PLAZA_PROFILE_SECTION("Poll Events");
			glfwPollEvents();
		}

	}
}

void ApplicationClass::UpdateEngine() {
	PLAZA_PROFILE_SECTION("Update Engine");
	// Update time
	Time::Update();
	float currentFrame = static_cast<float>(glfwGetTime());

	// Update Buffers
	if (Application->appSizes->sceneSize != Application->lastAppSizes->sceneSize || Application->appSizes->sceneStart != Application->lastAppSizes->sceneStart) {
		PLAZA_PROFILE_SECTION("Update Buffers");
		Application->updateBuffers(Application->textureColorbuffer, Application->rbo);
		Application->pickingTexture->updateSize(Application->appSizes->sceneSize);
		glBindTexture(GL_TEXTURE_2D, Application->pick);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	// Update Keyboard inputs
	Callbacks::processInput(Application->Window->glfwWindow);
	Input::Update();
	glEnable(GL_BLEND);

	// Update Camera Position and Rotation
	Application->activeCamera->Update();
	// Update Audio Listener
	Audio::UpdateListener();

	// Update Filewatcher main thread
	Filewatcher::UpdateOnMainThread();

	/* Update Physics */
	if (Application->runningScene) {
		PLAZA_PROFILE_SECTION("Update Physics");
		Physics::Advance(Time::deltaTime);
		Physics::Update();
	}

	/* Update Scripts */
	if (Application->runningScene) {
		PLAZA_PROFILE_SECTION("Mono Update");
		Mono::Update();
	}

	/* Update lights buffer */
	Lighting::UpdateBuffers();

	// Imgui New Frame (only if running editor)
#ifdef GAME_REL
#else
	Gui::NewFrame();
#endif // GAME_REL == 0




	// Clear buffers
	glBindFramebuffer(GL_FRAMEBUFFER, Application->frameBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);




	Application->mRenderer->AddInstancesToRender();
	// Render to shadows depth map
	Application->Shadows->GenerateDepthMap();
	// Draw GameObjects
	glBindFramebuffer(GL_FRAMEBUFFER, Application->geometryFramebuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, RenderGroup::renderMode == GL_TRIANGLES ? GL_FILL : RenderGroup::renderMode);
	Application->mRenderer->RenderInstances(*Application->shader);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Lighting::LightingPass(Lighting::mClusters, Lighting::mLights);

	// Update Skybox
	glBindFramebuffer(GL_READ_FRAMEBUFFER, Application->geometryFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Application->frameBuffer);
	glBlitFramebuffer(0, 0, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 0, 0, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	Skybox::Update();

	// Draw Outline
	if (Editor::selectedGameObject != nullptr && !Application->Shadows->showDepth && Application->focusedMenu != "Scene")
	{
		PLAZA_PROFILE_SECTION("Draw Outline");
		Application->mRenderer->RenderOutline();
		combineBuffers();
	}

	// Show the debug for shadows depth buffer
	if (Application->Shadows->showDepth) {
		PLAZA_PROFILE_SECTION("Debug Depth Buffer");
		glBindFramebuffer(GL_FRAMEBUFFER, Application->frameBuffer);
		Application->debugDepthShader->use();
		float near_plane = 0.1f, far_plane = 7.5f;
		Application->debugDepthShader->setInt("layer", Application->Shadows->debugLayer);
		Application->debugDepthShader->setFloat("near_plane", near_plane);
		Application->debugDepthShader->setFloat("far_plane", far_plane);
		Application->debugDepthShader->setInt("depthMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, Application->Shadows->shadowsDepthMap);
		glBindVertexArray(Application->blurVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Apply Distortion Correction
	glBindFramebuffer(GL_FRAMEBUFFER, Application->distortionCorrectionFrameBuffer->buffer);
	Application->distortionCorrectionShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Application->textureColorbuffer);
	Application->distortionCorrectionShader->setInt("sceneTexture", 0);
	glBindVertexArray(Application->blurVAO);
	int w = Application->appSizes->sceneSize.x;
	int h = Application->appSizes->sceneSize.y;
	glViewport(0, 0, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Render HDR
	Application->mRenderer->RenderHDR();

	// Render Screen Space Reflections
	ScreenSpaceReflections::Update();

	// Render Bloom
	Application->mRenderer->RenderBloom();

	/* Copy contents of HDR/Bloom to app framebuffer or 0 (when its a game)*/
	Application->mRenderer->CopyLastFramebufferToFinalDrawBuffer();


	// Render In-Game UI
	if (Application->focusedMenu == "Scene") {
		//#ifdef GAME_REL
		//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//#else
		//		glBindFramebuffer(GL_FRAMEBUFFER, Application->frameBuffer);
		//#endif // GAME_REL
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		/* Draw Texts */
		for (auto& [key, value] : Application->activeScene->UITextRendererComponents) {
			PLAZA_PROFILE_SECTION("Draw UI Components Text");
			value.Render(*Application->textRenderingShader);
		}
	}

	// Update ImGui (only if running editor)
#ifndef GAME_REL
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Gui::Update();
#endif
	// Update last frame

	Time::lastFrame = currentFrame;
	Time::drawCalls = 0;
	Time::addInstanceCalls = 0;
	// Update lastSizes
	Application->lastAppSizes = Application->appSizes;
	//FrameMark;

}

void ApplicationClass::Terminate() {
	for (const auto& meshRenderer : Application->activeScene->meshRenderers) {
		meshRenderer->mesh->Terminate();
	}
	//free(Application->editorScene);
	//free(Application->runtimeScene);
	Skybox::Terminate();
#ifndef GAME_REL
	Gui::Delete();
#endif // !GAME_REL
	glfwTerminate();
}

void ApplicationClass::InitBlur() {
	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int& quadVAO = Application->blurVAO;
	unsigned int& quadVBO = Application->blurVBO;
	unsigned int EBO = 0;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

}

void ApplicationClass::InitOpenGL() {
	ApplicationSizes& appSizes = *Application->appSizes;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_MULTISAMPLE);
	// Create buffers

	glPointSize(15);

	/* Edge Detection Framebuffer */
	blurFramebuffer1();
	Outline::Init();
	//blurFramebuffer2();
	Application->Shadows = new ShadowsClass();
	Application->Shadows->Init();


#pragma region Framebuffer
	/* Geometry Framebuffer */
	unsigned int& geometryBuffer = Application->geometryFramebuffer;
	glGenFramebuffers(1, &Application->geometryFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, Application->geometryFramebuffer);
	// - position color buffer
	glGenTextures(1, &Application->gPosition);
	glBindTexture(GL_TEXTURE_2D, Application->gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application->gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &Application->gNormal);
	glBindTexture(GL_TEXTURE_2D, Application->gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Application->gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &Application->gDiffuse);
	glBindTexture(GL_TEXTURE_2D, Application->gDiffuse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, Application->gDiffuse, 0);

	// - others, metalness specular and roughness
	glGenTextures(1, &Application->gOthers);
	glBindTexture(GL_TEXTURE_2D, Application->gOthers);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, Application->gOthers, 0);

	// - Depth
	glGenTextures(1, &Application->gDepth);
	glBindTexture(GL_TEXTURE_2D, Application->gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // GL_FLOAT for a depth texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, Application->gDepth, 0);

	GLenum geometryAttachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, geometryAttachments);

	glGenRenderbuffers(1, &Application->geometryRboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, Application->geometryRboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, appSizes.sceneSize.x, appSizes.sceneSize.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Application->geometryRboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	unsigned int& frameBuffer = Application->frameBuffer;
	//unsigned int& textureColorbuffer = Application->textureColorbuffer;
	glGenFramebuffers(1, &Application->frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, Application->frameBuffer);
	// create a color attachment texture
	glGenTextures(1, &Application->textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, Application->textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application->textureColorbuffer, 0);

	glGenTextures(1, &Application->pick);
	glBindTexture(GL_TEXTURE_2D, Application->pick);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, Application->pick, 0);
	//glReadBuffer(GL_NONE);
	//glDrawBuffer(GL_COLOR_ATTACHMENT4);

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &Application->rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, Application->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y); // use a single renderbuffer object for both a depth AND stencil buffer.
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Application->rbo); // now actually attach it

	// Disable reading
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_RED_INTEGER, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10 };
	glDrawBuffers(7, attachments);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);

	Application->pickingTexture = new PickingTexture();
	Application->pickingTexture->init(appSizes.sceneSize.x, appSizes.sceneSize.y);
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
#pragma endregion


	/* HDR/Bloom */
	//unsigned int& textureColorbuffer = Application->textureColorbuffer;
	glGenFramebuffers(1, &Application->hdrFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, Application->hdrFramebuffer);
	// HDR Scene
	glGenTextures(1, &Application->hdrSceneColor);
	glBindTexture(GL_TEXTURE_2D, Application->hdrSceneColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application->hdrSceneColor, 0);
	// Bloom
	glGenTextures(1, &Application->hdrBloomColor);
	glBindTexture(GL_TEXTURE_2D, Application->hdrBloomColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Application->hdrBloomColor, 0);
	GLenum hdrAttachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, hdrAttachments);

	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
}

void blurFramebuffer1() {
	ApplicationSizes& appSizes = *Application->appSizes;
	unsigned int& frameBuffer = Application->edgeDetectionFramebuffer;
	unsigned int& textureColorbuffer = Application->edgeDetectionColorBuffer;
	unsigned int& rbo = Application->edgeDetectionDepthStencilRBO;
	//unsigned int& textureColorbuffer = Application->textureColorbuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y); // use a single renderbuffer object for both a depth AND stencil buffer.
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_RED_INTEGER };
	glDrawBuffers(3, attachments);
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);

	Application->edgeDetectionShader->setInt("sceneBuffer", 0);
}

void blurFramebuffer2() {
	ApplicationSizes& appSizes = *Application->appSizes;
	unsigned int& frameBuffer = Application->blurFramebuffer;
	unsigned int& textureColorbuffer = Application->blurColorBuffer;
	unsigned int& rbo = Application->blurDepthStencilRBO;

	//unsigned int& textureColorbuffer = Application->textureColorbuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y); // use a single renderbuffer object for both a depth AND stencil buffer.
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_RED_INTEGER };
	glDrawBuffers(3, attachments);
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);

	Application->outlineBlurShader->setInt("sceneBuffer", 0);
}
void blurFramebuffer3() {
	ApplicationSizes& appSizes = *Application->appSizes;
	unsigned int& frameBuffer = Application->selectedFramebuffer;
	unsigned int& textureColorbuffer = Application->selectedColorBuffer;
	unsigned int& rbo = Application->selectedDepthStencilRBO;

	//unsigned int& textureColorbuffer = Application->textureColorbuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y); // use a single renderbuffer object for both a depth AND stencil buffer.
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_RED_INTEGER };
	glDrawBuffers(3, attachments);
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
}

/* Updates ? ------------------------------------------------------------------- MUST REVIEW IT*/
void ApplicationClass::updateBuffers(GLuint textureColorBuffer, GLuint rbo) {
	{
		ApplicationSizes& appSizes = *Application->appSizes;
		glBindTexture(GL_TEXTURE_2D, Application->textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGB, GL_FLOAT, nullptr);

		glBindTexture(GL_TEXTURE_2D, Application->edgeDetectionColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGB, GL_FLOAT, nullptr);

		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y);

		glBindRenderbuffer(GL_RENDERBUFFER, Application->edgeDetectionDepthStencilRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y);

		glViewport(appSizes.sceneStart.x, appSizes.sceneStart.y, appSizes.sceneSize.x, appSizes.sceneSize.y);
	}
}
