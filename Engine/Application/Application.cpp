#include "Engine/Core/PreCompiledHeaders.h"


//#include "Engine/Application/Application.h"
#include "Editor/GUI/guiMain.h"

#include "Engine/Application/PickingTexture.h"
#include "Engine/Components/Core/Camera.h"
#include "Engine/Components/Rendering/Mesh.h"
#include "Engine/Components/Core/Entity.h"
#include "Engine/Shaders/Shader.h"

#include "Editor/GUI/Style/EditorStyle.h"
#include "Engine/Core/Skybox.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Renderer.h"
#include "Engine/Editor/Editor.h"
#include "Engine/Application/Window.h"
#include "Engine/Application/Callbacks/CallbacksHeader.h"
#include "Engine/Editor/Outline/Outline.h"
#include "Engine/Editor/Editor.h"

#include "Engine/Application/EntryPoint.h"

#include <cstdlib> // Include the appropriate header for _dupenv_s
#include "Engine/Vendor/Tracy/tracy/Tracy.hpp"
//#include "Engine/Vendor/Tracy/tracy/TracyC.h"
//TRACY_ENABLE = true;
// ...
#include "Engine/Components/Physics/RigidBody.h"
#include "Engine/Core/Physics.h"
#include "Engine/Components/Core/Camera.h"
#include "Engine/Core/Mono.h"
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
	Application->shader = new Shader((Application->enginePath + "\\Shaders\\1.model_loadingVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\1.model_loadingFragment.glsl").c_str());
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

	Application->pickingShader = new Shader((Application->enginePath + "\\Shaders\\picking\\pickingVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\picking\\pickingFragment.glsl").c_str());

	Application->outlineShader = new Shader((Application->enginePath + "\\Shaders\\outlining\\outliningVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\outlining\\outliningFragment.glsl").c_str());

	Application->outlineBlurShader = new Shader((Application->enginePath + "\\Shaders\\blur\\blurVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\blur\\blurFragment.glsl").c_str());

	Application->outlineBlurShader->use();

	Application->outlineBlurShader->setInt("sceneBuffer", 0);

	Application->outlineBlurShader->setInt("depthStencilTexture", 1);

	Application->outlineBlurShader->setInt("depthStencilTexture2", 2);

	Application->combiningShader = new Shader((Application->enginePath + "\\Shaders\\combining\\combiningVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\combining\\combiningFragment.glsl").c_str());
	Application->edgeDetectionShader = new Shader((Application->enginePath + "\\Shaders\\edgeDetection\\edgeDetectionVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\edgeDetection\\edgeDetectionFragment.glsl").c_str());

	Application->singleColorShader = new Shader((Application->enginePath + "\\Shaders\\singleColor\\singleColorVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\singleColor\\singleColorFragment.glsl").c_str());

	Application->shadowsDepthShader = new Shader((Application->enginePath + "\\Shaders\\shadows\\shadowsDepthVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\shadows\\shadowsDepthFragment.glsl").c_str(), (Application->enginePath + "\\Shaders\\shadows\\shadowsDepthGeometry.glsl").c_str());

	//Application->debugDepthShader = new Shader((Application->enginePath + "\\Shaders\\debug\\debugDepthVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\debug\\debugDepthFragment.glsl").c_str());

	Application->hdrShader = new Shader((Application->enginePath + "\\Shaders\\hdr\\hdrVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\hdr\\hdrFragment.glsl").c_str());

	Application->distortionCorrectionShader = new Shader((Application->enginePath + "\\Shaders\\distortionCorrection\\distortionCorrectionVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\distortionCorrection\\distortionCorrectionFragment.glsl").c_str());

	Skybox::skyboxShader = new Shader((Application->enginePath + "\\Shaders\\skybox\\skyboxVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\skybox\\skyboxFragment.glsl").c_str());
	Skybox::skyboxShader->use();
	Skybox::skyboxShader->setInt("skybox", 0);

	Application->distortionCorrectionFrameBuffer = new FrameBuffer(GL_FRAMEBUFFER);
	Application->distortionCorrectionFrameBuffer->InitColorAttachment(GL_TEXTURE_2D, GL_RGBA32F, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, GL_RGBA, GL_FLOAT, GL_LINEAR);
	//Application->distortionCorrectionFrameBuffer->InitRenderBufferObject(GL_DEPTH_COMPONENT, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y);
}

void ApplicationClass::CreateApplication() {
	std::filesystem::path currentPath(__FILE__);
	Application->projectPath = currentPath.parent_path().parent_path().parent_path().string();
	Application->dllPath = currentPath.parent_path().parent_path().parent_path().string() + "\\dllPath";
	Application->enginePath = currentPath.parent_path().parent_path().string();
	Application->editorPath = currentPath.parent_path().parent_path().parent_path().string() + "\\Editor";
	Application->enginePathAppData = std::string(appdataValue) + "\\Plaza Engine\\";
	free(appdataValue);
	//gameObjects.reserve(5000);

	// Initialize GLFW (Window)
	Application->Window = new Plaza::WindowClass();

	
	// Initialize OpenGL, Shaders and Skybox
	InitShaders();
	InitOpenGL();

	Renderer::Init();

	InitBlur();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Application->InitSkybox();
	Skybox::Init();
	//Initialize ImGui
	Editor::Gui::Init(Application->Window->glfwWindow);
}


void ApplicationClass::UpdateProjectManagerGui() {
	Application->projectManagerGui->Update();
}

void ApplicationClass::Loop() {
	static constexpr tracy::SourceLocationData __tracy_source_location151{ "MainLoop", __FUNCTION__, "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Application\\Application.cpp", (uint32_t)151, 0 }; tracy::ScopedZone ___tracy_scoped_zone(&__tracy_source_location151, true);
	//ZoneScoped;
	while (!glfwWindowShouldClose(Application->Window->glfwWindow)) {
		//static constexpr tracy::SourceLocationData __tracy_source_location153{ "Main Loop", __FUNCTION__, "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Application\\Application.cpp", (uint32_t)153, 0 }; tracy::ScopedZone ___tracy_scoped_zone(&__tracy_source_location153, true);
		//ZoneScopedN("Main Loop");
		// Run the Engine (Update Time, Shadows, Inputs, Buffers, Rendering, etc.)
		if (Application->runEngine) {
			Application->UpdateEngine();
		} // Run the Gui for selecting a project
		else if (Application->runProjectManagerGui) {
			Application->UpdateProjectManagerGui();
		}

		// GLFW
		glfwSwapBuffers(Application->Window->glfwWindow);
		glfwPollEvents();
		FrameMark;
	}
}

void ApplicationClass::UpdateEngine() {
	static constexpr tracy::SourceLocationData __tracy_source_location171{ "Update Engine", __FUNCTION__, "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Application\\Application.cpp", (uint32_t)171, 0 }; tracy::ScopedZone ___tracy_scoped_zone(&__tracy_source_location171, true);

	// Update time
	Time::Update();
	float currentFrame = static_cast<float>(glfwGetTime());

	// Update Buffers
	if (Application->appSizes->sceneSize != Application->lastAppSizes->sceneSize || Application->appSizes->sceneStart != Application->lastAppSizes->sceneStart) {
		Application->updateBuffers(Application->textureColorbuffer, Application->rbo);
		Application->pickingTexture->updateSize(Application->appSizes->sceneSize);
		glBindTexture(GL_TEXTURE_2D, Application->pick);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Application->appSizes->sceneSize.x, Application->appSizes->sceneSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	// Update Keyboard inputs
	Callbacks::processInput(Application->Window->glfwWindow);
	glEnable(GL_BLEND);

	// Update Camera Position and Rotation
	Application->activeCamera->Update();

	/* Update Physics */
	if (Application->runningScene) {
		Physics::Advance(Time::deltaTime);
		Physics::Update();
	}

	/* Update Scripts */
	if (Application->runningScene) {
		Mono::Update();
	}

	// Imgui New Frame
	Gui::NewFrame();

	// Clear buffers
	glBindFramebuffer(GL_FRAMEBUFFER, Application->frameBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);




	// Render to shadows depth map
	Application->Shadows->GenerateDepthMap();
	// Draw GameObjects
	glBindFramebuffer(GL_FRAMEBUFFER, Application->frameBuffer);
	Renderer::Render(*Application->shader);
	Renderer::RenderInstances(*Application->shader);

	// Update Skybox
	glBindFramebuffer(GL_FRAMEBUFFER, Application->frameBuffer);
	Skybox::Update();
	//  Draw Outline
	if (Editor::selectedGameObject != nullptr && !Application->Shadows->showDepth)
	{
		static constexpr tracy::SourceLocationData __tracy_source_location213{ "Render Outline", __FUNCTION__, "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Application\\Application.cpp", (uint32_t)213, 0 }; tracy::ScopedZone ___tracy_scoped_zone(&__tracy_source_location213, true);
		Renderer::RenderOutline(*Application->outlineShader);
		combineBuffers();
	}

	if (Application->Shadows->showDepth) {
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
	Renderer::RenderHDR();

	// Update ImGui
	Gui::Update();


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
	delete(Application->editorScene);
	delete(Application->runtimeScene);
	Skybox::Terminate();
	Gui::Delete();
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

	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_RED_INTEGER, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(4, attachments);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);

	Application->pickingTexture = new PickingTexture();
	Application->pickingTexture->init(appSizes.sceneSize.x, appSizes.sceneSize.y);
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
#pragma endregion




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
