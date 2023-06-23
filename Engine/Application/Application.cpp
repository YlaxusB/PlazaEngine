#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/trigonometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <stb/stb_image.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <random>
#include <unordered_map>
#include <fileSystem>
#include <fileSystem/fileSystem.h>

#include "Engine/Application/Application.h"

#include "Engine/Application/ModelLoader.h"
#include "Engine/Application/PickingTexture.h"
#include "Engine/Components/Core/Camera.h"
#include "Engine/Components/Core/Mesh.h"
#include "Engine/Components/Core/GameObject.h"
#include "Engine/Components/Core/Model.h"
#include "Engine/Shaders/Shader.h"
#include "Engine/GUI/guiMain.h"
#include "Engine/GUI/gizmo.h"
#include "Engine/GUI/Style/EditorStyle.h"
#include "Engine/Core/Skybox.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Skybox.h"
#include "Engine/Core/Renderer.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void drop_callback(GLFWwindow* window, int count, const char** paths);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void updateBuffers(GLuint textureColorBuffer, GLuint rbo);

double previousTime = glfwGetTime();
int frameCount = 0;

std::list<Model> models;

EditorStyle editorStyle;

float lastX = Engine::Application::appSizes.appSize.x / 2.0f;
float lastY = Engine::Application::appSizes.appSize.y / 2.0f;
bool firstMouse = true;

bool rightClickPressed;

bool mouseFirstCallback;

unsigned int framebuffer;
unsigned int textureColorbuffer;
unsigned int rbo;

PickingTexture* pickingTexture;

using namespace Editor;

void Engine::Application::CreateApplication() {
	// Initialize GLFW (Window)
	Application::window = Application::InitGLFWWindow();
	// Initialize OpenGL, Shaders and Skybox
	InitOpenGL();
	Application::InitSkybox();

	//Initialize ImGui
	Editor::Gui::Init(window);

	// Initialize OpenGL
}

void Engine::Application::Loop() {
	while (!glfwWindowShouldClose(Application::window)) {
		// Update time
		Time::Update();

		// Update Buffers
		if (appSizes.sceneSize != lastAppSizes.sceneSize || appSizes.sceneStart != lastAppSizes.sceneStart) {
			updateBuffers(textureColorbuffer, rbo);
			pickingTexture->updateSize(appSizes.sceneSize);
		}

		// Update inputs
		processInput(window);

		// Clear buffers
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Render with Picking Shader
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		Renderer::Render(Application::pickingShader);

		// Draw GameObjects
		// Update Skybox
		Skybox::Update();

		//  Draw Outline
		Renderer::RenderOutline(Application::outlineShader);

		// Update ImGui
		Gui::Update();

		// Update last frame
		float currentFrame = static_cast<float>(glfwGetTime());
		Time::lastFrame = currentFrame;

		// GLFW
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Update lastSizes
		Application::lastAppSizes = Application::appSizes;
	}
}

void Engine::Application::Terminate() {
	Skybox::Terminate();
	Gui::Delete();
	glfwTerminate();
}

GLFWwindow* Engine::Application::InitGLFWWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	/* --------- MUST CHANGE THE WAY IT'S STARTING ON SECOND MONITOR ---------*/

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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetDropCallback(window, drop_callback);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	stbi_set_flip_vertically_on_load(false);
	return window;
}

void Engine::Application::InitOpenGL() {
	ApplicationSizes& appSizes = Engine::Application::appSizes;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Init some starting shaders
	Engine::Application::shader = Shader("C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\1.model_loading.vs", "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\1.model_loading.fs");
	Engine::Application::pickingShader = Shader("C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\1.model_loading.vs", "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\objectPickingFragment.glsl");
	Engine::Application::outlineShader = Shader("C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\outlining\\outliningVertex.glsl", "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\outlining\\outliningFragment.glsl");

	// Create buffers
#pragma region Framebuffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
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

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);

	pickingTexture = new PickingTexture();
	pickingTexture->init(appSizes.sceneSize.x, appSizes.sceneSize.y);
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);

#pragma endregion

	Engine::Application::shader.use();
	Engine::Application::shader.setInt("texture1", 0);
}


/* Callbacks */
namespace Engine {
	void Application::drop_callback(GLFWwindow* window, int count, const char** paths)
	{
		int i;
		for (i = 0; i < count; i++) {
			std::string fileParent = filesystem::path{ paths[i] }.parent_path().string();
			vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
			vector<Mesh> meshes = vector<Mesh>();
			string directory = "";

			std::string fileName = filesystem::path{ paths[i] }.filename().string();
			ModelLoader::loadModel(paths[i], directory, &meshes, textures_loaded, fileName);
			std::cout << "finished loading" << std::endl;
		}
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			Application::activeCamera.MovementSpeed += Application::activeCamera.MovementSpeed * (yoffset * 3) * Time::deltaTime;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
			Application::activeCamera.ProcessMouseScroll(static_cast<float>(yoffset));
	}

	void processInput(GLFWwindow* window)
	{
		ApplicationSizes& appSizes = Engine::Application::appSizes;
		ApplicationSizes& lastAppSizes = Engine::Application::lastAppSizes;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
			int size = gameObjects.size();
			for (int i = size; i < size + 1; i++) {
				GameObject* d = new GameObject(std::to_string(gameObjects.size()), gameObjects.front());
				std::cout << d->parent << std::endl;
				//d->AddComponent(new Transform());

				d->GetComponent<Transform>()->relativePosition = glm::vec3(4, 0, 0);
				d->transform->UpdateChildrenTransform();
				Engine::Mesh* cubeMesh = Engine::Mesh::Cube();
				MeshRenderer* meshRenderer = new MeshRenderer(*cubeMesh);
				meshRenderer->mesh = *cubeMesh;
				d->AddComponent<MeshRenderer>(meshRenderer);
			}
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			Application::activeCamera.MovementSpeedTemporaryBoost = 5.0f;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			Application::activeCamera.ProcessKeyboard(Engine::Camera::FORWARD, Time::deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			Application::activeCamera.ProcessKeyboard(Engine::Camera::BACKWARD, Time::deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			Application::activeCamera.ProcessKeyboard(Engine::Camera::LEFT, Time::deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			Application::activeCamera.ProcessKeyboard(Engine::Camera::RIGHT, Time::deltaTime);
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			Application::activeCamera.ProcessKeyboard(Engine::Camera::UP, Time::deltaTime);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			Application::activeCamera.ProcessKeyboard(Engine::Camera::DOWN, Time::deltaTime);
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			Application::activeCamera.ProcessMouseMovement(0, 0, 10);
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			Application::activeCamera.ProcessMouseMovement(0, 0, -10);

		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
			appSizes.sceneSize += 1;
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
			appSizes.sceneSize -= 1;

		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
			gameObjects.back()->transform->position.x += 1;



		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
			Application::activeCamera.MovementSpeedTemporaryBoost = 1.0f;
	}

	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		// Prevent the camera from roughly moving to mouse position when pressed right button
		if (rightClickPressed && mouseFirstCallback) {
			mouseFirstCallback = false;
		}
		else if (rightClickPressed && !mouseFirstCallback) {
			Application::activeCamera.ProcessMouseMovement(xoffset, yoffset);
			mouseFirstCallback = false;
		}
		else if (!rightClickPressed) {
			mouseFirstCallback = true;
		}
		lastX = xpos;
		lastY = ypos;

	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		ApplicationSizes& appSizes = Engine::Application::appSizes;
		ApplicationSizes& lastAppSizes = Engine::Application::lastAppSizes;

		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
			// Pressing right button
			rightClickPressed = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			// No more pressing right button
			rightClickPressed = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

#pragma region Picking
		float xposGame = lastX - appSizes.hierarchySize.x;
		float yposGame = lastY - appSizes.appHeaderSize;
		yposGame = appSizes.sceneSize.y - yposGame;

		// Select nothing if didnt clicked on a GameObject
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver() && pickingTexture->readPixel(xposGame, yposGame) == 0) {
			Gui::changeSelectedGameObject(nullptr);
		}
		else {
			std::cout << pickingTexture->readPixel(xposGame, yposGame) << std::endl;
		}


		// Select the GameObject
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver() && pickingTexture->readPixel(xposGame, yposGame) != 0) {
			int targetName = pickingTexture->readPixel(xposGame, yposGame);
			auto it = std::find_if(gameObjects.begin(), gameObjects.end(), [&](const GameObject* obj) {
				return obj->id == targetName;
				});
			if (it != gameObjects.end()) {
				// Object with the specified name found
				Gui::changeSelectedGameObject(*it);
			}
		}
#pragma endregion Picking
	}
}

/* Updates ? ------------------------------------------------------------------- MUST REVIEW IT*/
namespace Engine {
	void updateBuffers(GLuint textureColorBuffer, GLuint rbo) {
		{
			ApplicationSizes& appSizes = Engine::Application::appSizes;
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGB, GL_FLOAT, nullptr);

			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y);

			glViewport(appSizes.sceneStart.x, appSizes.sceneStart.y, appSizes.sceneSize.x, appSizes.sceneSize.y);
		}
	}
}