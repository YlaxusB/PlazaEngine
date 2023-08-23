#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Core/Time.h"

#include "Engine/Application/Serializer/Components/MeshSerializer.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"

using namespace Engine;
namespace Engine {
	void DeleteChildrene(GameObject* gameObject) {
		uint64_t uuid = Editor::selectedGameObject->uuid;
		if (gameObject != reinterpret_cast<GameObject*>(0xdddddddddddddddd)) {
			for (uint64_t child : gameObject->childrenUuid) {
				DeleteChildrene(&Application->activeScene->entities[child]);
			}
		}
		//delete(gameObject);
		auto it = std::find_if(Application->activeScene->gameObjects.begin(), Application->activeScene->gameObjects.end(), [uuid](const std::unique_ptr<GameObject>& gameObject) {
			return gameObject->uuid == uuid;
			});

		if (it != Application->activeScene->gameObjects.end()) {
			Application->activeScene->gameObjects.erase(it);
		}

	}
}
void ApplicationClass::Callbacks::processInput(GLFWwindow* window) {
	if (Application->focusedMenu == "Scene") {
		ApplicationSizes& appSizes = *Application->appSizes;
		ApplicationSizes& lastAppSizes = *Application->lastAppSizes;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);


		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
			int size = Application->activeScene->gameObjects.size();
			for (int i = size; i < size + 10; i++) {
				GameObject* d = new GameObject(std::to_string(Application->activeScene->entities.size()), Application->activeScene->mainSceneEntity);
				//d->AddComponent(new Transform());



				std::random_device rd;
				std::mt19937 gen(rd());

				// Define the range for the random numbers (-20 to 20)
				int min = -20;
				int max = 20;
				std::uniform_int_distribution<int> distribution(min, max);
				Transform& test = *d->GetComponent<Transform>();
				d->GetComponent<Transform>()->relativePosition = glm::vec3(distribution(gen), distribution(gen), distribution(gen)) + Application->activeCamera->Position;
				d->GetComponent<Transform>()->UpdateChildrenTransform();
				Mesh cubeMesh = Engine::Mesh();//Engine::Mesh::Cube();
				cubeMesh.material.diffuse.rgba = glm::vec4(0.8f, 0.3f, 0.3f, 1.0f);
				cubeMesh.material.specular = Texture();
				cubeMesh.material.specular.rgba = glm::vec4(0.3f, 0.5f, 0.3f, 1.0f);
				MeshRenderer* meshRenderer = new MeshRenderer(cubeMesh);
				meshRenderer->instanced = true;
				//meshRenderer->mesh = std::make_unique<Mesh>(cubeMesh);
				meshRenderer->mesh = Editor::DefaultModels::Plane();
				//MeshSerializer::Serialize(Application->activeProject->directory + "\\teste.yaml", *cubeMesh);
				d->AddComponent<MeshRenderer>(meshRenderer);
			}
		}



		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			Application->activeCamera->MovementSpeedTemporaryBoost = 5.0f;

		if (Application->activeCamera->isEditorCamera) {
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
				Application->activeCamera->ProcessKeyboard(Engine::Camera::UP, Time::deltaTime);
			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
				Application->activeCamera->ProcessKeyboard(Engine::Camera::DOWN, Time::deltaTime);
		}
		else if (Application->activeScene->rigidBodyComponents.find(Application->activeCamera->uuid) != Application->activeScene->rigidBodyComponents.end()){
			Transform* transform = &Application->activeScene->transformComponents.at(Application->activeCamera->uuid);
			RigidBody* rigidbody = &Application->activeScene->rigidBodyComponents.at(Application->activeCamera->uuid);
			float speed = 10;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				transform->MoveTowards(glm::vec3(-1.0f * speed * Time::deltaTime, 0.0f, 0.0f));
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				transform->MoveTowards(glm::vec3(1.0f * speed * Time::deltaTime, 0.0f, 0.0f));
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				transform->MoveTowards(glm::vec3(0.0f, 0.0f, -1.0f * speed * Time::deltaTime));
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				transform->MoveTowards(glm::vec3(0.0f, 0.0f, 1.0f * speed * Time::deltaTime));
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
				transform->MoveTowards(glm::vec3(0.0f, 1.0f * speed * Time::deltaTime, 0.0f));
			glm::quat quaternion = transform->GetWorldQuaternion();
			physx::PxQuat pxQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

			physx::PxTransform* pxTransform = new physx::PxTransform(
				transform->worldPosition.x, transform->worldPosition.y, transform->worldPosition.z,
				pxQuaternion);

			RigidBody* rigidBody = &Application->activeScene->rigidBodyComponents.at(Application->activeCamera->uuid);
			Collider* collider = &Application->activeScene->colliderComponents.at(Application->activeCamera->uuid);
			// Apply scaling to the existing pxTransform
			if (rigidBody && rigidBody->mRigidActor)
				rigidBody->mRigidActor->setGlobalPose(*pxTransform);
			else if (collider && !collider->mDynamic && collider->mStaticPxRigidBody)
				collider->mStaticPxRigidBody->setGlobalPose(*pxTransform);
		}

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

		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && Editor::selectedGameObject) {
			if (RigidBody* rigidBody = &Application->activeScene->rigidBodyComponents.at(Editor::selectedGameObject->uuid)) {
				rigidBody->ApplyForce(glm::vec3(5000.0f, 0.0f, 0.0f));
			}
		}


		/* Dumb moving */
		if (Editor::selectedGameObject && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			Editor::selectedGameObject->GetComponent<Transform>()->relativePosition.x -= 10.0f * Time::deltaTime;
		}
		if (Editor::selectedGameObject && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			Editor::selectedGameObject->GetComponent<Transform>()->relativePosition.x += 10.0f * Time::deltaTime;
		}
		if (Editor::selectedGameObject && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			Editor::selectedGameObject->GetComponent<Transform>()->relativePosition.z -= 10.0f * Time::deltaTime;
		}
		if (Editor::selectedGameObject && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			Editor::selectedGameObject->GetComponent<Transform>()->relativePosition.z += 10.0f * Time::deltaTime;
		}
		if (Editor::selectedGameObject && glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
			Editor::selectedGameObject->GetComponent<Transform>()->relativePosition.y += 20.0f * Time::deltaTime;
		}
		if (Editor::selectedGameObject) {
			Editor::selectedGameObject->GetComponent<Transform>()->UpdateChildrenTransform();
			Transform transform = *Editor::selectedGameObject->GetComponent<Transform>();
			glm::quat quaternion = transform.GetWorldQuaternion();
			physx::PxQuat pxQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

			physx::PxTransform* pxTransform = new physx::PxTransform(
				transform.worldPosition.x, transform.worldPosition.y, transform.worldPosition.z,
				pxQuaternion);

			RigidBody* rigidBody = Editor::selectedGameObject->GetComponent<RigidBody>();
			Collider* collider = Editor::selectedGameObject->GetComponent<Collider>();
			// Apply scaling to the existing pxTransform
			if (rigidBody && rigidBody->mRigidActor)
				rigidBody->mRigidActor->setGlobalPose(*pxTransform);
			else if (collider && !collider->mDynamic && collider->mStaticPxRigidBody)
				collider->mStaticPxRigidBody->setGlobalPose(*pxTransform);
		}

		if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS && Editor::selectedGameObject) {
			Editor::selectedGameObject->~GameObject();
			//Editor::selectedGameObject->Delete();
			/*
			uint64_t uuid = Editor::selectedGameObject->uuid;
			DeleteChildrene(Editor::selectedGameObject);
			auto it = std::find_if(Application->activeScene->gameObjects.begin(), Application->activeScene->gameObjects.end(), [uuid](const std::unique_ptr<GameObject>& gameObject) {
				return gameObject->uuid == uuid;
				});

			if (it != Application->activeScene->gameObjects.end()) {
				Application->activeScene->gameObjects.erase(it);
			}
			*/
			Editor::selectedGameObject = nullptr;
		}
	}
}
