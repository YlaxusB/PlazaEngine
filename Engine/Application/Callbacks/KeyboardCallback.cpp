#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Core/Time.h"

#include "Engine/Application/Serializer/Components/MeshSerializer.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"
#include "Engine/Core/Physics.h"

Entity* NewEntity(string name, Entity* parent, Mesh* mesh, bool instanced = true, bool addToScene = true) {
	Entity* obj = new Entity(name, parent, addToScene);
	//obj->changingName = true;
	//Application->activeScene->entities.at(obj->uuid).changingName = true;
	//Gui::Hierarchy::Item::firstFocus = true;
	obj->GetComponent<Transform>()->UpdateChildrenTransform();
	MeshRenderer* meshRenderer = new MeshRenderer(*mesh, Scene::DefaultMaterial());
	meshRenderer->instanced = true;
	//meshRenderer->mesh = new Mesh(*mesh);
	meshRenderer->material = Scene::DefaultMaterial();
	//RenderGroup* newRenderGroup = new RenderGroup(meshRenderer->mesh, meshRenderer->material);
	meshRenderer->renderGroup = Application->activeScene->AddRenderGroup(meshRenderer->mesh, meshRenderer->material);
	//meshRenderer->renderGroup->material = make_shared<Material>(*Scene::DefaultMaterial());
	obj->AddComponent<MeshRenderer>(meshRenderer);
	Editor::selectedGameObject = obj;

	return obj;
}

glm::vec3 randomVec3() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(-500.0f, 500.0f);

	return glm::vec3(dis(gen), dis(gen), dis(gen));
}

void ApplicationClass::Callbacks::processInput(GLFWwindow* window) {
	if (Application->focusedMenu == "Editor") {

		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
			Entity* obj = NewEntity("Sphere", Application->activeScene->mainSceneEntity, Editor::DefaultModels::Cube(), true, true);
			Transform* transform = obj->GetComponent<Transform>();
			transform->relativePosition = randomVec3();
			transform->UpdateSelfAndChildrenTransform();
			Collider* collider = new Collider(obj->uuid);
			collider->CreateShape(ColliderShapeEnum::SPHERE, transform);
			obj->AddComponent<Collider>(collider);
		}

		ApplicationSizes& appSizes = *Application->appSizes;
		ApplicationSizes& lastAppSizes = *Application->lastAppSizes;

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			Application->activeCamera->MovementSpeedTemporaryBoost = 5.0f;

		if (Application->activeCamera->isEditorCamera) {
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::FORWARD, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::BACKWARD, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::LEFT, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::RIGHT, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::UP, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::DOWN, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::UP, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Plaza::Camera::DOWN, Time::deltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
			Application->activeCamera->MovementSpeedTemporaryBoost = 1.0f;
	}
}
