#include "Engine/Core/PreCompiledHeaders.h"

#include "Editor/GUI/guiMain.h"

#include "Engine/Application/PickingTexture.h"
#include "Engine/Components/Core/Camera.h"
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


void blurFramebuffer1();
void blurFramebuffer2();
void blurFramebuffer3();

using namespace Plaza;
/// ---------------------------------------------------------------------

//std::list<Model> models;

EditorStyle editorStyle;

using namespace Plaza::Editor;

#define DEFAULT_GRAPHICAL_API "Vulkan"

Plaza::Application::Application() {
     editorCamera = new Plaza::Camera(glm::vec3(0.0f, 0.0f, 5.0f));
     editorCamera->isEditorCamera = true;
     activeCamera = editorCamera;
}
//ApplicationClass* Application = nullptr;
void combineBuffers() {
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
     glBindFramebuffer(GL_FRAMEBUFFER, Application::Get()->frameBuffer);

     glEnable(GL_DEPTH_TEST);
     glEnable(GL_STENCIL_TEST);
     glStencilMask(0xFF);
     glStencilFunc(GL_ALWAYS, 1, 0xFF);
     glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
     glViewport(Application::Get()->appSizes->sceneStart.x, Application::Get()->appSizes->sceneStart.y, Application::Get()->appSizes->sceneSize.x, Application::Get()->appSizes->sceneSize.y);

     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, Application::Get()->textureColorbuffer);
     glActiveTexture(GL_TEXTURE1);
     glBindTexture(GL_TEXTURE_2D, Application::Get()->edgeDetectionColorBuffer);

     glActiveTexture(GL_TEXTURE2);
     glBindTexture(GL_TEXTURE_2D, Application::Get()->rbo);
     // Load depth data into the texture

     glActiveTexture(GL_TEXTURE3);
     glBindTexture(GL_TEXTURE_2D, Outline::blurringBuffer->depthBuffer);

     Application::Get()->combiningShader->use();
     Application::Get()->combiningShader->setInt("buffer1", 0);
     Application::Get()->combiningShader->setInt("buffer2", 1);
     Application::Get()->combiningShader->setInt("Depth0", 2);
     Application::Get()->combiningShader->setInt("Depth1", 3);
     //glUniform1i(glGetUniformLocation(Application::Get()->edgeDetectionShader->ID, "screenTexture"), 0);

     glBindVertexArray(Application::Get()->blurVAO);
     glDrawArrays(GL_TRIANGLES, 0, 6);
     //Renderer::Render(*Application::Get()->shader);
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

void Application::InitShaders() {
     // Initialize Shaders
#ifdef  GAME_REL
     std::string shadersFolder = Application::Get()->projectPath;
#else
     std::string shadersFolder = Application::Get()->enginePath;
#endif //  GAME_REL

     std::cout << "ProjectPath2  : " << Application::Get()->projectPath << "\n";
     std::cout << "shadersFolder  : " << shadersFolder << "\n";

     Application::Get()->shader = new Shader((shadersFolder + "\\Shaders\\1.model_loadingVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\1.model_loadingFragment.glsl").c_str());
     constexpr const char* textureDiffuseUniform = "texture_diffuse";
     constexpr const char* textureSpecularUniform = "texture_specular";
     constexpr const char* textureNormalUniform = "texture_normal";
     constexpr const char* textureHeightUniform = "texture_height";
     glUniform1i(glGetUniformLocation(Application::Get()->shader->ID, textureDiffuseUniform), 0);
     glUniform1i(glGetUniformLocation(Application::Get()->shader->ID, textureSpecularUniform), 1);
     glUniform1i(glGetUniformLocation(Application::Get()->shader->ID, textureNormalUniform), 2);
     glUniform1i(glGetUniformLocation(Application::Get()->shader->ID, textureHeightUniform), 3);
     Application::Get()->shader->use();
     Application::Get()->shader->setInt("texture_diffuse", 0);
     Application::Get()->shader->setInt("texture_specular", 1);
     Application::Get()->shader->setInt("texture_normal", 2);
     Application::Get()->shader->setInt("texture_height", 3);
     Application::Get()->shader->setInt("texture_metalness", 4);
     Application::Get()->shader->setInt("texture_roughness", 5);



     Application::Get()->pickingShader = new Shader((shadersFolder + "\\Shaders\\picking\\pickingVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\picking\\pickingFragment.glsl").c_str());

     Application::Get()->outlineShader = new Shader((shadersFolder + "\\Shaders\\outlining\\outliningVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\outlining\\outliningFragment.glsl").c_str());

     Application::Get()->outlineBlurShader = new Shader((shadersFolder + "\\Shaders\\blur\\blurVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\blur\\blurFragment.glsl").c_str());

     Application::Get()->mRenderer->InitShaders(shadersFolder);


     Application::Get()->outlineBlurShader->use();

     Application::Get()->outlineBlurShader->setInt("sceneBuffer", 0);

     Application::Get()->outlineBlurShader->setInt("depthStencilTexture", 1);

     Application::Get()->outlineBlurShader->setInt("depthStencilTexture2", 2);

     Application::Get()->combiningShader = new Shader((shadersFolder + "\\Shaders\\combining\\combiningVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\combining\\combiningFragment.glsl").c_str());
     Application::Get()->edgeDetectionShader = new Shader((shadersFolder + "\\Shaders\\edgeDetection\\edgeDetectionVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\edgeDetection\\edgeDetectionFragment.glsl").c_str());

     Application::Get()->singleColorShader = new Shader((shadersFolder + "\\Shaders\\singleColor\\singleColorVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\singleColor\\singleColorFragment.glsl").c_str());

     Application::Get()->shadowsDepthShader = new Shader((shadersFolder + "\\Shaders\\shadows\\shadowsDepthVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\shadows\\shadowsDepthFragment.glsl").c_str(), (shadersFolder + "\\Shaders\\shadows\\shadowsDepthGeometry.glsl").c_str());

     //Application::Get()->debugDepthShader = new Shader((Application::Get()->enginePath + "\\Shaders\\debug\\debugDepthVertex.glsl").c_str(), (Application::Get()->enginePath + "\\Shaders\\debug\\debugDepthFragment.glsl").c_str());

     Application::Get()->hdrShader = new Shader((shadersFolder + "\\Shaders\\hdr\\hdrVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\hdr\\hdrFragment.glsl").c_str());

     Application::Get()->distortionCorrectionShader = new Shader((shadersFolder + "\\Shaders\\distortionCorrection\\distortionCorrectionVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\distortionCorrection\\distortionCorrectionFragment.glsl").c_str());

     //Lighting::mLightAccumulationShader = new Shader((shadersFolder + "\\Shaders\\ClusteredForward\\accumulationVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\ClusteredForward\\accumulationFragment.glsl").c_str());
     //Lighting::mLightMergerShader = new Shader((shadersFolder + "\\Shaders\\ClusteredForward\\lightMergerVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\ClusteredForward\\lightMergerFragment.glsl").c_str());
     //Lighting::mLightMergerShader->use();
     //Lighting::mLightMergerShader->setInt("gPosition", 0);
     //Lighting::mLightMergerShader->setInt("gNormal", 1);
     //Lighting::mLightMergerShader->setInt("gDiffuse", 2);
     //Lighting::mLightMergerShader->setInt("gOthers", 3);
     //
     //Lighting::mLightSorterComputeShader = new ComputeShader((shadersFolder + "\\Shaders\\ClusteredForward\\lightSorterCompute.glsl").c_str());

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


     Application::Get()->textRenderingShader = new Shader((shadersFolder + "\\Shaders\\textRendering\\textRenderingVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\textRendering\\textRenderingFragment.glsl").c_str());

     OpenGLSkybox::skyboxShader = new Shader((shadersFolder + "\\Shaders\\skybox\\skyboxVertex.glsl").c_str(), (shadersFolder + "\\Shaders\\skybox\\skyboxFragment.glsl").c_str());
     OpenGLSkybox::skyboxShader->use();
     OpenGLSkybox::skyboxShader->setInt("skybox", 0);

     Application::Get()->distortionCorrectionFrameBuffer = new FrameBuffer(GL_FRAMEBUFFER);
     Application::Get()->distortionCorrectionFrameBuffer->InitColorAttachment(GL_TEXTURE_2D, GL_RGBA32F, Application::Get()->appSizes->sceneSize.x, Application::Get()->appSizes->sceneSize.y, GL_RGBA, GL_FLOAT, GL_LINEAR);
     //Application::Get()->distortionCorrectionFrameBuffer->InitRenderBufferObject(GL_DEPTH_COMPONENT, Application::Get()->appSizes->sceneSize.x, Application::Get()->appSizes->sceneSize.y);
}

void Application::CreateApplication() {
    PL_CORE_INFO("Creating Application");

    this->GetPaths();
    AssetsManager::Init();

    Application::Get()->mRenderer->api = Settings::mDefaultRendererAPI;
    switch (Settings::mDefaultRendererAPI) {
    case RendererAPI::OpenGL:
        Application::Get()->mRenderer = new OpenGLRenderer();
        break;
    case RendererAPI::Vulkan:
        Application::Get()->mRenderer = new VulkanRenderer();
        break;
    }

     Application::Get()->mWindow->glfwWindow = Application::Get()->mWindow->InitGLFWWindow();

     this->GetAppSize();
#ifdef EDITOR_MODE
     this->CheckEditorCache();
#endif
     mRenderer->Init();
     Audio::Init();
     Physics::Init();
     this->LoadProject();
}

void Application::GetPaths() {
    char* appdataValue;
    size_t len;
    errno_t err = _dupenv_s(&appdataValue, &len, "APPDATA");
    std::filesystem::path currentPath(__FILE__);
    Application::Get()->enginePath = currentPath.parent_path().parent_path().string();
    Application::Get()->editorPath = currentPath.parent_path().parent_path().parent_path().string() + "\\Editor";
    Application::Get()->enginePathAppData = std::string(appdataValue) + "\\PlazaEngine\\";
    Application::Get()->exeDirectory = filesystem::current_path().string();
}

void Application::GetAppSize() {
#ifdef GAME_MODE
    // Set the scene size to be the entire screen
    int width, height;
    glfwGetWindowSize(Application::Get()->mWindow->glfwWindow, &width, &height);
    Application::Get()->appSizes->sceneSize = glm::vec2(width, height);
#else

#endif
}

void Application::CheckEditorCache() {
    /* Check if the engine app data folder doesnt exists, if not, then create */
    if (!std::filesystem::is_directory(Application::Get()->enginePathAppData) && Application::Get()->runningEditor) {
        std::filesystem::create_directory(Application::Get()->enginePathAppData);
        if (!std::filesystem::exists(Application::Get()->enginePathAppData + "\\cache.yaml")) {

        }
    }
}

void Application::LoadProject() {
#ifdef GAME_MODE
    std::cout << "Loading Project \n";
    for (auto const& entry : std::filesystem::directory_iterator{ Application::Get()->exeDirectory }) {
        if (entry.path().extension() == Standards::projectExtName) {
            Editor::Project::Load(entry.path().string());

            std::cout << "Starting Scene\n";
            Scene::Play();
            std::cout << "Scene Played \n";
            if (Application::Get()->activeScene->cameraComponents.size() > 0)
                Application::Get()->activeCamera = &Application::Get()->activeScene->cameraComponents.begin()->second;
            else
                Application::Get()->activeCamera = Application::Get()->activeScene->mainSceneEntity->AddComponent<Camera>(new Camera());
        }
    }

#else
    if (filesystem::exists(Application::Get()->enginePathAppData + "cache.yaml"))
        Editor::Cache::Load();
#endif
}


void Application::UpdateProjectManagerGui() {
     Application::Get()->projectManagerGui->Update();
}

void Application::Loop() {
    PL_CORE_INFO("Starting Loop");
     while (!glfwWindowShouldClose(Application::Get()->mWindow->glfwWindow)) {
          PLAZA_PROFILE_SECTION("Loop");
          // Run the Engine (Update Time, Shadows, Inputs, Buffers, Rendering, etc.)
          if (Application::Get()->runEngine) {
               Application::Get()->UpdateEngine();
          } // Run the Gui for selecting a project
          else if (Application::Get()->runProjectManagerGui) {
               Application::Get()->UpdateProjectManagerGui();
          }

          // GLFW
          {
               PLAZA_PROFILE_SECTION("Swap Buffers");
               glfwSwapBuffers(Application::Get()->mWindow->glfwWindow);
          }
          {
               PLAZA_PROFILE_SECTION("Poll Events");
               glfwPollEvents();
          }
     }
}

void Application::UpdateEngine() {
     Plaza::Editor::selectedFiles;
     PLAZA_PROFILE_SECTION("Update Engine");
     // Update time
     Time::Update();
     float currentFrame = static_cast<float>(glfwGetTime());

     // Update Buffers
     if (Application::Get()->mRenderer->api == RendererAPI::OpenGL) {
          if (Application::Get()->appSizes->sceneSize != Application::Get()->lastAppSizes->sceneSize || Application::Get()->appSizes->sceneStart != Application::Get()->lastAppSizes->sceneStart) {
               PLAZA_PROFILE_SECTION("Update Buffers");
               Application::Get()->updateBuffers(Application::Get()->textureColorbuffer, Application::Get()->rbo);
               Application::Get()->pickingTexture->updateSize(Application::Get()->appSizes->sceneSize);
               glBindTexture(GL_TEXTURE_2D, Application::Get()->pick);
               glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Application::Get()->appSizes->sceneSize.x, Application::Get()->appSizes->sceneSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
          }
     }
     // Update Keyboard inputs
     Callbacks::processInput(Application::Get()->mWindow->glfwWindow);
     Input::Update();

     Application::Get()->mThreadsManager->UpdateFrameStartThread();

     if (Application::Get()->mRenderer->api == RendererAPI::OpenGL)
          glEnable(GL_BLEND);

     // Update Audio Listener
     Audio::UpdateListener();

     // Update Filewatcher main thread
     Filewatcher::UpdateOnMainThread();

     // Update Animations
     for (auto& [key, value] : Application::Get()->activeScene->mPlayingAnimations) {
         value->UpdateTime(Time::deltaTime);
     }

     /* Update Physics */
     if (Application::Get()->runningScene) {
          PLAZA_PROFILE_SECTION("Update Physics");
          Physics::Advance(Time::deltaTime);
          Physics::Update();
     }

     /* Update Scripts */
     if (Application::Get()->runningScene) {
          PLAZA_PROFILE_SECTION("Mono Update");
          Mono::Update();
     }

     // Update Camera Position and Rotation
     Application::Get()->activeCamera->Update();

	// Imgui New Frame (only if running editor)
#ifdef EDITOR_MODE
     Gui::NewFrame();
     if (Application::Get()->mRenderer->api == RendererAPI::Vulkan)
          Gui::Update();
#endif

     Time::drawCalls = 0;
     Time::addInstanceCalls = 0;
     Time::mUniqueTriangles = 0;
     Time::mTotalTriangles = 0;
     Application::Get()->mRenderer->RenderInstances(*Application::Get()->shader);

     Application::Get()->mThreadsManager->UpdateFrameEndThread();

     // Update last frame
     Time::lastFrame = currentFrame;

     // Update lastSizes
     Application::Get()->lastAppSizes = Application::Get()->appSizes;
     Input::isAnyKeyPressed = false;
     Application::Get()->mThreadsManager->mFrameEndThread->Update();
}

void Application::Terminate() {
    PL_CORE_INFO("Terminate");
     for (const auto& meshRenderer : Application::Get()->activeScene->meshRenderers) {
          //  meshRenderer->mesh->Terminate();
     }
     //free(Application::Get()->editorScene);
     //free(Application::Get()->runtimeScene);
     if (Application::Get()->mRenderer->api = RendererAPI::OpenGL)
          OpenGLSkybox::Terminate();
#ifdef EDITOR_MODE
     Gui::Delete();
#endif // !GAME_REL
     glfwTerminate();
}

void Application::InitBlur() {
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

     unsigned int& quadVAO = Application::Get()->blurVAO;
     unsigned int& quadVBO = Application::Get()->blurVBO;
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

void Application::InitOpenGL() {
     ApplicationSizes& appSizes = *Application::Get()->appSizes;

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
     Application::Get()->Shadows = new ShadowsClass();
     Application::Get()->Shadows->Init();


#pragma region Framebuffer
     /* Geometry Framebuffer */
     unsigned int& geometryBuffer = Application::Get()->geometryFramebuffer;
     glGenFramebuffers(1, &Application::Get()->geometryFramebuffer);
     glBindFramebuffer(GL_FRAMEBUFFER, Application::Get()->geometryFramebuffer);
     // - position color buffer
     glGenTextures(1, &Application::Get()->gPosition);
     glBindTexture(GL_TEXTURE_2D, Application::Get()->gPosition);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application::Get()->gPosition, 0);

     // - normal color buffer
     glGenTextures(1, &Application::Get()->gNormal);
     glBindTexture(GL_TEXTURE_2D, Application::Get()->gNormal);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Application::Get()->gNormal, 0);

     // - color + specular color buffer
     glGenTextures(1, &Application::Get()->gDiffuse);
     glBindTexture(GL_TEXTURE_2D, Application::Get()->gDiffuse);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, Application::Get()->gDiffuse, 0);

     // - others, metalness specular and roughness
     glGenTextures(1, &Application::Get()->gOthers);
     glBindTexture(GL_TEXTURE_2D, Application::Get()->gOthers);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, Application::Get()->gOthers, 0);

     // - Depth
     glGenTextures(1, &Application::Get()->gDepth);
     glBindTexture(GL_TEXTURE_2D, Application::Get()->gDepth);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // GL_FLOAT for a depth texture
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, Application::Get()->gDepth, 0);

     GLenum geometryAttachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
     glDrawBuffers(5, geometryAttachments);

     glGenRenderbuffers(1, &Application::Get()->geometryRboDepth);
     glBindRenderbuffer(GL_RENDERBUFFER, Application::Get()->geometryRboDepth);
     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, appSizes.sceneSize.x, appSizes.sceneSize.y);
     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Application::Get()->geometryRboDepth);
     // finally check if framebuffer is complete
     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
          std::cout << "Framebuffer not complete!" << std::endl;
     glBindFramebuffer(GL_FRAMEBUFFER, 0);



     unsigned int& frameBuffer = Application::Get()->frameBuffer;
     //unsigned int& textureColorbuffer = Application::Get()->textureColorbuffer;
     glGenFramebuffers(1, &Application::Get()->frameBuffer);
     glBindFramebuffer(GL_FRAMEBUFFER, Application::Get()->frameBuffer);
     // create a color attachment texture
     glGenTextures(1, &Application::Get()->textureColorbuffer);
     glBindTexture(GL_TEXTURE_2D, Application::Get()->textureColorbuffer);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application::Get()->textureColorbuffer, 0);

     glGenTextures(1, &Application::Get()->pick);
     glBindTexture(GL_TEXTURE_2D, Application::Get()->pick);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGB, GL_FLOAT, NULL);
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, Application::Get()->pick, 0);
     //glReadBuffer(GL_NONE);
     //glDrawBuffer(GL_COLOR_ATTACHMENT4);

     // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
     glGenRenderbuffers(1, &Application::Get()->rbo);
     glBindRenderbuffer(GL_RENDERBUFFER, Application::Get()->rbo);
     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y); // use a single renderbuffer object for both a depth AND stencil buffer.
     glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Application::Get()->rbo); // now actually attach it

     // Disable reading
     glReadBuffer(GL_NONE);
     glDrawBuffer(GL_COLOR_ATTACHMENT0);

     GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_RED_INTEGER, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10 };
     glDrawBuffers(7, attachments);

     glBindTexture(GL_TEXTURE_2D, 0);
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
     glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);

     Application::Get()->pickingTexture = new PickingTexture();
     Application::Get()->pickingTexture->init(appSizes.sceneSize.x, appSizes.sceneSize.y);
     glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
#pragma endregion


     /* HDR/Bloom */
     //unsigned int& textureColorbuffer = Application::Get()->textureColorbuffer;
     glGenFramebuffers(1, &Application::Get()->hdrFramebuffer);
     glBindFramebuffer(GL_FRAMEBUFFER, Application::Get()->hdrFramebuffer);
     // HDR Scene
     glGenTextures(1, &Application::Get()->hdrSceneColor);
     glBindTexture(GL_TEXTURE_2D, Application::Get()->hdrSceneColor);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Application::Get()->hdrSceneColor, 0);
     // Bloom
     glGenTextures(1, &Application::Get()->hdrBloomColor);
     glBindTexture(GL_TEXTURE_2D, Application::Get()->hdrBloomColor);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGBA, GL_FLOAT, NULL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Application::Get()->hdrBloomColor, 0);
     GLenum hdrAttachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
     glDrawBuffers(2, hdrAttachments);

     glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
     glBindTexture(GL_TEXTURE_2D, 0);
     glBindFramebuffer(GL_FRAMEBUFFER, 0);
     glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
}

void blurFramebuffer1() {
     ApplicationSizes& appSizes = *Application::Get()->appSizes;
     unsigned int& frameBuffer = Application::Get()->edgeDetectionFramebuffer;
     unsigned int& textureColorbuffer = Application::Get()->edgeDetectionColorBuffer;
     unsigned int& rbo = Application::Get()->edgeDetectionDepthStencilRBO;
     //unsigned int& textureColorbuffer = Application::Get()->textureColorbuffer;
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

     Application::Get()->edgeDetectionShader->setInt("sceneBuffer", 0);
}

void blurFramebuffer2() {
     ApplicationSizes& appSizes = *Application::Get()->appSizes;
     unsigned int& frameBuffer = Application::Get()->blurFramebuffer;
     unsigned int& textureColorbuffer = Application::Get()->blurColorBuffer;
     unsigned int& rbo = Application::Get()->blurDepthStencilRBO;

     //unsigned int& textureColorbuffer = Application::Get()->textureColorbuffer;
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

     Application::Get()->outlineBlurShader->setInt("sceneBuffer", 0);
}
void blurFramebuffer3() {
     ApplicationSizes& appSizes = *Application::Get()->appSizes;
     unsigned int& frameBuffer = Application::Get()->selectedFramebuffer;
     unsigned int& textureColorbuffer = Application::Get()->selectedColorBuffer;
     unsigned int& rbo = Application::Get()->selectedDepthStencilRBO;

     //unsigned int& textureColorbuffer = Application::Get()->textureColorbuffer;
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
void Application::updateBuffers(GLuint textureColorBuffer, GLuint rbo) {
     {
          ApplicationSizes& appSizes = *Application::Get()->appSizes;
          glBindTexture(GL_TEXTURE_2D, Application::Get()->textureColorbuffer);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGB, GL_FLOAT, nullptr);

          glBindTexture(GL_TEXTURE_2D, Application::Get()->edgeDetectionColorBuffer);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGB, GL_FLOAT, nullptr);

          glBindRenderbuffer(GL_RENDERBUFFER, rbo);
          glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y);

          glBindRenderbuffer(GL_RENDERBUFFER, Application::Get()->edgeDetectionDepthStencilRBO);
          glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y);

          glViewport(appSizes.sceneStart.x, appSizes.sceneStart.y, appSizes.sceneSize.x, appSizes.sceneSize.y);
     }
}
