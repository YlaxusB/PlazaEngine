#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
using namespace Engine;

void ApplicationClass::Callbacks::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	std::filesystem::path currentPath(__FILE__);
	std::string projectDirectory = currentPath.parent_path().parent_path().parent_path().string();

	if (key == GLFW_KEY_G && action == GLFW_PRESS)
		Application->Shadows->showDepth = !Application->Shadows->showDepth;

	if(key == GLFW_KEY_R && action == GLFW_PRESS)
		Application->shader = new Shader((projectDirectory + "\\Shaders\\1.model_loadingVertex.glsl").c_str(), (projectDirectory + "\\Shaders\\1.model_loadingFragment.glsl").c_str());

	if (key == GLFW_KEY_U && action == GLFW_PRESS)
		Application->activeCamera->Position = Engine::Editor::selectedGameObject->transform->position;




	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		int size = gameObjects.size();
		for (int i = size; i < size + 3000; i++) {
			GameObject* d = new GameObject(std::to_string(gameObjects.size()), gameObjects.front());
			//d->AddComponent(new Transform());

			d->GetComponent<Transform>()->relativePosition = glm::vec3(4, 0, 0);
			d->transform->UpdateChildrenTransform();
			Engine::Mesh* cubeMesh = Engine::Mesh::Sphere();
			cubeMesh->material.diffuse->rgba = glm::vec4(0.8f, 0.3f, 0.3f, 1.0f);
			cubeMesh->material.specular = new Texture();
			cubeMesh->material.specular->rgba = glm::vec4(0.3f, 0.5f, 0.3f, 1.0f);
			MeshRenderer* meshRenderer = new MeshRenderer(cubeMesh);
			//meshRenderer->mesh = cubeMesh;
			delete cubeMesh;
			//delete meshRenderer;
			d->AddComponent<MeshRenderer>(meshRenderer);
		}
	}
}