#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Application/Serializer/SceneSerializer.h"
using namespace Engine;

void ApplicationClass::Callbacks::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (Application->focusedMenu == "Scene") {

		if (key == GLFW_KEY_G && action == GLFW_PRESS)
			Application->Shadows->showDepth = !Application->Shadows->showDepth;

		if (key == GLFW_KEY_R && action == GLFW_PRESS)
			Application->shader = new Shader((Application->enginePath + "\\Shaders\\1.model_loadingVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\1.model_loadingFragment.glsl").c_str());

		if (key == GLFW_KEY_U && action == GLFW_PRESS)
			Application->activeCamera->Position = Engine::Editor::selectedGameObject->transform->position;
		/*
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
			int size = Application->activeScene->gameObjects.size();
			for (int i = size; i < size + 3000; i++) {
				GameObject* d = new GameObject(std::to_string(Application->activeScene->gameObjects.size()), Application->activeScene->gameObjects.front().get());
				//d->AddComponent(new Transform());

				d->GetComponent<Transform>()->relativePosition = glm::vec3(4, 0, 0);
				d->transform->UpdateChildrenTransform();
				Mesh cubeMesh = Engine::Mesh::Sphere();
				cubeMesh.material.diffuse.rgba = glm::vec4(0.8f, 0.3f, 0.3f, 1.0f);
				cubeMesh.material.specular = Texture();
				cubeMesh.material.specular.rgba = glm::vec4(0.3f, 0.5f, 0.3f, 1.0f);
				MeshRenderer* meshRenderer = new MeshRenderer(cubeMesh);
				//meshRenderer->mesh = cubeMesh;

				//delete meshRenderer;
				d->AddComponent<MeshRenderer>(meshRenderer);
			}
		}
		*/


		if (glfwGetKey(window, GLFW_KEY_INSERT) == GLFW_PRESS) {
			Application->Shadows->debugLayer += 1;
			if (Application->Shadows->debugLayer > Application->Shadows->shadowCascadeLevels.size()) {
				Application->Shadows->debugLayer = 0;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
			Application->shadowsDepthShader = new Shader((Application->enginePath + "\\Shaders\\shadows\\shadowsDepthVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\shadows\\shadowsDepthFragment.glsl").c_str(), (Application->enginePath + "\\Shaders\\shadows\\shadowsDepthGeometry.glsl").c_str());
		}

		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			// Stop
			if (Application->runningScene) {
				Editor::selectedGameObject = nullptr;
				if (Editor::selectedGameObject)
					Engine::Editor::Gui::changeSelectedGameObject(Application->editorScene->gameObjects.find(Editor::selectedGameObject->uuid));
				// Change active scene, update the selected object scene, delete runtime and set running to false.
				delete(Application->runtimeScene);
				Application->runningScene = false;
				Application->activeScene = Application->editorScene;
			} // Play
			else {
				// Create a new empty Scene, change active scene to runtime, copy the contents of editor scene into runtime scene and update the selected object scene
				Application->runtimeScene = new Scene();
				Application->activeScene = Application->runtimeScene;
				Application->runtimeScene = Scene::Copy(Application->runtimeScene, Application->editorScene);
				if (Editor::selectedGameObject)
					Engine::Editor::Gui::changeSelectedGameObject(Application->activeScene->gameObjects.find(Editor::selectedGameObject->name));

				Application->runningScene = true;
			}
		}


		if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
			Application->shader = new Shader((Application->enginePath + "\\Shaders\\1.model_loadingVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\1.model_loadingFragment.glsl").c_str());
			Application->pickingShader = new Shader((Application->enginePath + "\\Shaders\\picking\\pickingVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\picking\\pickingFragment.glsl").c_str());
			Application->outlineShader = new Shader((Application->enginePath + "\\Shaders\\outlining\\outliningVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\outlining\\outliningFragment.glsl").c_str());
			Application->outlineBlurShader = new Shader((Application->enginePath + "\\Shaders\\blur\\blurVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\blur\\blurFragment.glsl").c_str());
			Application->combiningShader = new Shader((Application->enginePath + "\\Shaders\\combining\\combiningVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\combining\\combiningFragment.glsl").c_str());
			Application->edgeDetectionShader = new Shader((Application->enginePath + "\\Shaders\\edgeDetection\\edgeDetectionVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\edgeDetection\\edgeDetectionFragment.glsl").c_str());
			Application->singleColorShader = new Shader((Application->enginePath + "\\Shaders\\singleColor\\singleColorVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\singleColor\\singleColorFragment.glsl").c_str());
			Application->shadowsDepthShader = new Shader((Application->enginePath + "\\Shaders\\shadows\\shadowsDepthVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\shadows\\shadowsDepthFragment.glsl").c_str(), (Application->enginePath + "\\Shaders\\shadows\\shadowsDepthGeometry.glsl").c_str());
			Application->debugDepthShader = new Shader((Application->enginePath + "\\Shaders\\debug\\debugDepthVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\debug\\debugDepthFragment.glsl").c_str());

		}
	}
}