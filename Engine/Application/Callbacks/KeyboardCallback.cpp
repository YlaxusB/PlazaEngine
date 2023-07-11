#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Core/Time.h"
using namespace Engine;
void ApplicationClass::Callbacks::processInput(GLFWwindow* window) {
	ApplicationSizes& appSizes = *Application->appSizes;
	ApplicationSizes& lastAppSizes = *Application->lastAppSizes;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		int size = gameObjects.size();
		for (int i = size; i < size + 1; i++) {
			std::unique_ptr<GameObject> d = std::make_unique<GameObject>(std::to_string(gameObjects.size()), gameObjects.front());
			//d->AddComponent(new Transform());

			d->GetComponent<Transform>()->relativePosition = glm::vec3(4, 0, 0);
			d->transform->UpdateChildrenTransform();
			Engine::Mesh* cubeMesh = Engine::Mesh::Sphere();//Engine::Mesh::Cube();
			cubeMesh->material.diffuse->rgba = glm::vec4(0.8f, 0.3f, 0.3f, 1.0f);
			cubeMesh->material.specular = new Texture();
			cubeMesh->material.specular->rgba = glm::vec4(0.3f, 0.5f, 0.3f, 1.0f);
			MeshRenderer* meshRenderer = new MeshRenderer(cubeMesh);
			meshRenderer->mesh = cubeMesh;
			d->AddComponent<MeshRenderer>(meshRenderer);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		int size = gameObjects.size();
		for (int i = size; i < size + 100; i++) {
			std::unique_ptr<GameObject> d = std::make_unique<GameObject>(std::to_string(gameObjects.size()), gameObjects.front());
			//d->AddComponent(new Transform());

			d->GetComponent<Transform>()->relativePosition = glm::vec3(4, 0, 0);
			d->transform->UpdateChildrenTransform();
			Engine::Mesh* cubeMesh = Engine::Mesh::Cube();
			cubeMesh->material.diffuse->rgba = glm::vec4(0.8f, 0.3f, 0.3f, 1.0f);
			cubeMesh->material.specular = new Texture();
			cubeMesh->material.specular->rgba = glm::vec4(0.3f, 0.5f, 0.3f, 1.0f);
			MeshRenderer* meshRenderer = new MeshRenderer(cubeMesh);
			meshRenderer->mesh = cubeMesh;
			d->AddComponent<MeshRenderer>(meshRenderer);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		Application->activeCamera->MovementSpeedTemporaryBoost = 5.0f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Application->activeCamera->ProcessKeyboard(Engine::Camera::FORWARD, Time::deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Application->activeCamera->ProcessKeyboard(Engine::Camera::BACKWARD, Time::deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Application->activeCamera->ProcessKeyboard(Engine::Camera::LEFT, Time::deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Application->activeCamera->ProcessKeyboard(Engine::Camera::RIGHT, Time::deltaTime);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		Application->activeCamera->ProcessKeyboard(Engine::Camera::UP, Time::deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		Application->activeCamera->ProcessKeyboard(Engine::Camera::DOWN, Time::deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		Application->activeCamera->ProcessMouseMovement(0, 0, 10);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		Application->activeCamera->ProcessMouseMovement(0, 0, -10);

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		gameObjects.back()->transform->position.x += 1;


	//if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	//	Application->shader = new Shader("C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\outlining\\outliningVertex.glsl", "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\outlining\\outliningFragment.glsl");

	//gameObjects.back()->transform->position.x += 1;


	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		Application->shader = new Shader("C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\blur\\blurVertex.glsl", "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\blur\\blurFragment.glsl");
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		Application->shader = new Shader("C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\1.model_loading.vs", "C:\\Users\\Giovane\\Desktop\\Workspace 2023\\OpenGL\\OpenGLEngine\\Engine\\Shaders\\1.model_loading.fs");
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
		Application->activeCamera->MovementSpeedTemporaryBoost = 1.0f;
}
