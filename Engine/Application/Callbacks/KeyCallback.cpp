#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Application/Serializer/SceneSerializer.h"
#include "Engine/Core/Physics.h"
#include "Editor/DefaultAssets/DefaultAssets.h"

#include "Engine/Core/Skybox.h"
using namespace Plaza;
uint64_t lastUuid;
void ApplicationClass::Callbacks::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (Application->focusedMenu == "Editor") {

		if (key == GLFW_KEY_G && action == GLFW_PRESS)
			Application->Shadows->showDepth = !Application->Shadows->showDepth;

		if (key == GLFW_KEY_R && action == GLFW_PRESS)
			Editor::selectedGameObject->GetComponent<Transform>()->Rotate(glm::vec3(0.0f, 1.0f, 0.0f));

		if (key == GLFW_KEY_U && action == GLFW_PRESS)
			Application->activeCamera->Position = Plaza::Editor::selectedGameObject->GetComponent<Transform>()->GetWorldPosition();


		if (glfwGetKey(window, GLFW_KEY_INSERT) == GLFW_PRESS) {
			Application->Shadows->debugLayer += 1;
			if (Application->Shadows->debugLayer > Application->Shadows->shadowCascadeLevels.size()) {
				Application->Shadows->debugLayer = 0;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
			Application->shadowsDepthShader = new Shader((Application->enginePath + "\\Shaders\\shadows\\shadowsDepthVertex.glsl").c_str(), (Application->enginePath + "\\Shaders\\shadows\\shadowsDepthFragment.glsl").c_str(), (Application->enginePath + "\\Shaders\\shadows\\shadowsDepthGeometry.glsl").c_str());
		}

		// Play and Pause
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			if (Application->runningScene)
				Scene::Stop();
			else
				Scene::Play();
		}

		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
			int size = Application->activeScene->gameObjects.size();
			for (int i = size; i < size + 1; i++) {
				Entity* d;
				if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
					d = new Entity(std::to_string(Application->activeScene->entities.size()), &Application->activeScene->entities.at(lastUuid));
				else
					d = new Entity(std::to_string(Application->activeScene->entities.size()), Application->activeScene->mainSceneEntity);

				std::random_device rd;
				std::mt19937 gen(rd());

				// Define the range for the random numbers (-20 to 20)
				int min = -20;
				int max = 20;
				std::uniform_int_distribution<int> distribution(min, max);
				Transform& test = *d->GetComponent<Transform>();
				d->GetComponent<Transform>()->relativePosition = glm::vec3(distribution(gen), distribution(gen), distribution(gen)) + Application->activeCamera->Position;
				d->GetComponent<Transform>()->UpdateChildrenTransform();
				Mesh cubeMesh = Plaza::Mesh();//Plaza::Mesh::Cube();
				cubeMesh.material.diffuse.rgba = glm::vec4(0.8f, 0.3f, 0.3f, 1.0f);
				cubeMesh.material.specular = Texture();
				cubeMesh.material.specular.rgba = glm::vec4(0.8f, 0.3f, 0.3f, 1.0f);
				MeshRenderer* meshRenderer = new MeshRenderer(cubeMesh);
				meshRenderer->instanced = true;
				//meshRenderer->mesh = std::make_unique<Mesh>(cubeMesh);
				//Editor::DefaultModels::Init();
				meshRenderer->mesh = Editor::DefaultModels::Cube();
				//MeshSerializer::Serialize(Application->activeProject->directory + "\\teste.yaml", *cubeMesh);
				d->AddComponent<MeshRenderer>(meshRenderer);
				Transform* transform = d->GetComponent<Transform>();
				/*Collider* collider = new Collider(d->uuid);
				physx::PxBoxGeometry geometry(transform->scale.x / 2.1, transform->scale.y / 2.1, transform->scale.z / 2.1);
				collider->AddShape(Physics::m_physics->createShape(geometry, *Physics::defaultMaterial));
				d->AddComponent<Collider>(collider);
				RigidBody* rigidBody = new RigidBody(d->uuid, Application->runningScene);
				rigidBody->uuid = d->uuid;
				d->AddComponent<RigidBody>(rigidBody);
				lastUuid = d->uuid;*/

				// add script
				std::string key = "C:\\Users\\Giovane\\Desktop\\Workspace\\PlazaGames\\Speed Runners\\Source\\carScript.dll";
				CsScriptComponent* script = new CsScriptComponent(d->uuid);
				std::string csFileName = filesystem::path{ key }.replace_extension(".cs").string();
				script->Init(csFileName);;
				d->AddComponent<CsScriptComponent>(script);
				if (Application->runningScene) {
					for(auto& [key, value] : script->scriptClasses)
					Mono::OnStart(value->monoObject);
				}
				d->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
			}
		}

		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
			if (Application->activeCamera->isEditorCamera && Application->activeScene->cameraComponents.size() > 0)
				Application->activeCamera = &Application->activeScene->cameraComponents.begin()->second;
			else
				Application->activeCamera = Application->editorCamera;
		}

		if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
			Application->InitShaders();
		}
	}
}