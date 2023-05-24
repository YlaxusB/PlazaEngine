#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb/stb_image.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <random>
//#include "Engine/Vendor/filesystem/filesys.h"
#include <fileSystem>
#include <fileSystem/fileSystem.h>
#include "Engine/Application/Application.h"
#include "Engine/GUI/guiMain.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Mesh.h"
#include "Engine/Components/GameObject.h"
#include "Engine/Components/Model.h"
#include "Engine/Shaders/Shader.h"
#include "Engine/Application/EditorCamera.h"
#include "Engine/GUI/gizmo.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void drop_callback(GLFWwindow* window, int count, const char** paths);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

unsigned int loadTexture(const char* path);

double previousTime = glfwGetTime();
int frameCount = 0;

std::list<Model> models;


//gameObjects
AppSizes appSizes;
AppSizes lastAppSizes;

Camera editorCamera(glm::vec3(0.0f, 0.0f, 5.0f));
Camera activeCamera = editorCamera;

float lastX = appSizes.appSize.x / 2.0f;
float lastY = appSizes.appSize.y / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool rightClickPressed;

bool mouseFirstCallback;

int setupVAO(float skyboxVertices[]) {
	unsigned int skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	return skyboxVAO;
}

int setupVBO() {
	unsigned int skyboxVBO;
	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	return skyboxVBO;
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
/*
int setupSkybox(float skyboxVertices[]) {
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	return skyboxVAO;
}
*/
Mesh* CubeMesh() {
	std::vector<unsigned int> indices{
		0, 1, 2, 2, 3, 0,
		1, 5, 6, 6, 2, 1,
		5, 4, 7, 7, 6, 5,
		4, 0, 3, 3, 7, 4,
		3, 2, 6, 6, 7, 3,
		0, 4, 5, 5, 1, 0
	};
	std::vector<Vertex> vertices{
	{glm::vec3(0, 0, 0)},
	{glm::vec3(1, 0, 0)},
	{glm::vec3(1, 1, 0)},
	{glm::vec3(0, 1, 0)},
	{glm::vec3(0, 0, 1)},
	{glm::vec3(1, 0, 1)},
	{glm::vec3(1, 1, 1)},
	{glm::vec3(0, 1, 1)}
	};
	return new Mesh(vertices, indices, std::vector<Texture>());
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
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
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(appSizes.appSize.x, appSizes.appSize.y, "LearnOpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSwapInterval(0); Disable vsync
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetDropCallback(window, drop_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	stbi_set_flip_vertically_on_load(false);
	glEnable(GL_DEPTH_TEST);

	Shader ourShader("C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\1.model_loading.vs", "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\1.model_loading.fs");
	//Model ourModel("C:/Users/Giovane/Desktop/Workspace 2023/OpenGL/OpenGLEngine/Engine/ExampleAssets/backpack/backpack.obj");

	unsigned int cubeTexture = loadTexture("C:/Users/Giovane/Desktop/Workspace 2023/OpenGL/OpenGLEngine/Engine/ExampleAssets/container.jpg");
	// load models
	// -----------
	//Model ourModel(FileSystem::getPath("Engine/ExampleAssets/sponza/sponza.obj");
	//Model ourModel("C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\ExampleAssets\\sponza\\sponza.obj");


	//Initialize ImGui

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	io.IniFilename = NULL;
	ImGui_ImplOpenGL3_Init("#version 330");

	// framebuffer configuration
	// -------------------------
#pragma region Framebuffer
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, appSizes.sceneSize.x, appSizes.sceneSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, appSizes.sceneSize.x, appSizes.sceneSize.y); // use a single renderbuffer object for both a depth AND stencil buffer.
	glViewport(0, 0, appSizes.sceneSize.x, appSizes.sceneSize.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
#pragma endregion

#pragma region Skybox Setup
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<std::string> faces
	{
		"C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\ExampleAssets\\skybox\\right.jpg",
		"C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\ExampleAssets\\skybox\\left.jpg",
		"C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\ExampleAssets\\skybox\\top.jpg",
		"C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\ExampleAssets\\skybox\\bottom.jpg",
		"C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\ExampleAssets\\skybox\\front.jpg",
		"C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\ExampleAssets\\skybox\\back.jpg"
	};
	stbi_set_flip_vertically_on_load(false);
	unsigned int cubemapTexture = loadCubemap(faces);
	Shader skyboxShader("C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\skybox\\skyboxVertex.glsl", "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\skybox\\skyboxFragment.glsl");
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
#pragma endregion

	ourShader.use();
	ourShader.setInt("texture1", 0);
	while (!glfwWindowShouldClose(window)) {

		// Measure speed
		double currentTime = glfwGetTime();
		frameCount++;
		// If a second has passed.
		if (currentTime - previousTime >= 1.0)
		{
			// Display the frame count here any way you want.
			//std::cout << frameCount << std::endl;

			frameCount = 0;
			previousTime = currentTime;
		}


		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;


		//std::cout << 1000 / deltaTime << std::endl;

		processInput(window);
		// Start a new ImGui frame

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glClearColor(0.1f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		ourShader.use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(activeCamera.Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.01f, 10000.0f);
		glm::mat4 view = activeCamera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("model", model);

		//ourModel.Draw(ourShader);

		// Render Game
		for (GameObject* gameObject : gameObjects) {
			MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
			if (meshRenderer) {
				// render the loaded model
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(gameObject->GetComponent<Transform>()->position)); // translate it down so it's at the center of the scene
				glm::vec3 objectEulerAngles = gameObject->GetComponent<Transform>()->rotation;
				model = glm::rotate(model, objectEulerAngles.z, glm::vec3(0.0f, 0.0f, 1.0f));  // Rotate around Z-axis
				model = glm::rotate(model, objectEulerAngles.y, glm::vec3(0.0f, 1.0f, 0.0f));  // Rotate around Y-axis
				model = glm::rotate(model, objectEulerAngles.x, glm::vec3(1.0f, 0.0f, 0.0f));  // Rotate around X-axis

				model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
				ourShader.setMat4("model", model);
				meshRenderer->mesh.Draw(ourShader);
			}
		}

		for (Model model : models) {
			glm::mat4 modelMath = glm::mat4(1.0f);
			
			modelMath = glm::translate(modelMath, glm::vec3(models.size() * 4, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
			modelMath = glm::scale(modelMath, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
			ourShader.setMat4("model", modelMath);
			model.Draw(ourShader);
		}

		// Render Skybox
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(activeCamera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Start the Imgui GUI
		Gui::setupDockspace(window, textureColorbuffer, appSizes, lastAppSizes, activeCamera);

		// Render the ImGui frame
		ImGui::Render();

		// Update the game, so game calculations can run


		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
		lastFrame = currentFrame;
	}
	// Clean up ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &framebuffer);

	glfwTerminate();
	return 0;
}

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	int i;
	for (i = 0; i < count; i++) {
		std::string fileParent = filesystem::path{ paths[i] }.parent_path().string();
		//Model model(fileParent);
		Model model(paths[i]);
		models.push_back(model);
	}
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		int size = gameObjects.size();
		for (int i = size; i < size + 1; i++) {
			GameObject* d = new GameObject(std::to_string(gameObjects.size()));
			d->AddComponent(new Transform());
			d->GetComponent<Transform>()->position = glm::vec3(size + i + 2, 0, 0);

			std::vector<unsigned int> indices = {
				0, 1, 2,  // Front face
				2, 1, 3,  // Front face
				4, 5, 6,  // Back face
				6, 5, 7,  // Back face
				8, 9, 10, // Top face
				10, 9, 11,// Top face
				12, 13, 14,// Bottom face
				14, 13, 15,// Bottom face
				16, 17, 18,// Left face
				18, 17, 19,// Left face
				20, 21, 22,// Right face
				22, 21, 23 // Right face
			};

			std::vector<Vertex> vertices = {
				// Front face vertices
				Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				// Back face vertices
				Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				// Top face vertices
				Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				// Bottom face vertices
				Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				// Left face vertices
				Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				// Right face vertices
				Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0))
			};
			//vertices.push_back(Vertex(glm::vec3(0.5, 0.5, 1), glm::vec3(0, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)));

			Mesh* testingMesh = new Mesh(vertices, indices, std::vector<Texture>());
			MeshRenderer* meshRenderer = new MeshRenderer(*testingMesh);
			meshRenderer->mesh = *testingMesh;
			d->AddComponent(meshRenderer);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		activeCamera.MovementSpeedTemporaryBoost = 5.0f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		activeCamera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		activeCamera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		activeCamera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		activeCamera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		activeCamera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		activeCamera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		activeCamera.ProcessMouseMovement(0, 0, 10);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		activeCamera.ProcessMouseMovement(0, 0, -10);

	//if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
	//	activeCamera.ProcessMouseMovement(0, 0, 0);
	//}




	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
		activeCamera.MovementSpeedTemporaryBoost = 1.0f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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
		activeCamera.ProcessMouseMovement(xoffset, yoffset);
		mouseFirstCallback = false;
	}
	else if (!rightClickPressed) {
		mouseFirstCallback = true;
	}
	lastX = xpos;
	lastY = ypos;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		activeCamera.MovementSpeed += activeCamera.MovementSpeed * (yoffset * 3)* deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
		activeCamera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}