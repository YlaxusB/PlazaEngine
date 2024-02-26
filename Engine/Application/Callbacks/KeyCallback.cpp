#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Application/Serializer/SceneSerializer.h"
#include "Engine/Core/Physics.h"
#include "Editor/DefaultAssets/DefaultAssets.h"

#include "Engine/Core/Skybox.h"
#include "Editor/GUI/guiMain.h"
#include "Engine/Core/Lighting/ClusteredForward.h"
using namespace Plaza;
uint64_t lastUuid;

static unsigned int cascadeIndexDebug = 2;
void ApplicationClass::Callbacks::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (Application->focusedMenu == "Editor") {
		VulkanRenderer* vulkanRenderer = (VulkanRenderer*)Application->mRenderer;
		if (key == GLFW_KEY_H && action == GLFW_PRESS) {
			vulkanRenderer->ChangeFinalDescriptorImageView(vulkanRenderer->mShadows->mCascades[cascadeIndexDebug].mImageView);
		}
		if (key == GLFW_KEY_J && action == GLFW_PRESS) {
			vulkanRenderer->ChangeFinalDescriptorImageView(vulkanRenderer->mFinalSceneImageView);
		}
		if (key == GLFW_KEY_L && action == GLFW_PRESS) {
			cascadeIndexDebug++;
			if (cascadeIndexDebug > 8)
				cascadeIndexDebug = 0;
			vulkanRenderer->ChangeFinalDescriptorImageView(vulkanRenderer->mShadows->mCascades[cascadeIndexDebug].mImageView);
		}
		if (key == GLFW_KEY_G && action == GLFW_PRESS)
			Application->showCascadeLevels = !Application->showCascadeLevels;

		if (key == GLFW_KEY_N && action == GLFW_PRESS) {
			Editor::selectedGameObject->GetComponent<MeshRenderer>()->LoadHeightMap("C:\\Users\\Giovane\\Desktop\\Workspace\\heightmap.save");
		}
		//if (key == GLFW_KEY_G && action == GLFW_PRESS)
		//	Application->activeScene->entities[Editor::selectedGameObject->uuid].RemoveComponent<RigidBody>();

		if (key == GLFW_KEY_U && action == GLFW_PRESS)
			Application->activeCamera->Position = Plaza::Editor::selectedGameObject->GetComponent<Transform>()->GetWorldPosition();

		if (key == GLFW_KEY_END && action == GLFW_PRESS) {
			Editor::selectedGameObject->GetComponent<Transform>()->rotation *= glm::quat(glm::vec3(0.0f, 0.1f, 0.0f));
			Editor::selectedGameObject->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
		}
		if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
			Editor::selectedGameObject->GetComponent<Transform>()->rotation *= glm::quat(glm::vec3(0.0f, -0.1f, 0.0f));
			Editor::selectedGameObject->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
		}

		// Play and Pause
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			if (Application->runningScene)
				Scene::Stop();
			else
				Scene::Play();
		}

		if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
			Application->InitShaders();
		}

		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
			for (Lighting::LightStruct& light : Lighting::mLights) {
				uint64_t uuid = *Application->activeScene->entitiesNames.find("Light")->second.begin();
				uint64_t instantiatedUuid = Application->activeScene->entities.at(uuid).Instantiate(uuid);
				Application->activeScene->entities.find(instantiatedUuid)->second.GetComponent<Transform>()->SetRelativePosition(light.position);
			}
		}

		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
			if (RenderGroup::renderMode == GL_TRIANGLES) {
				RenderGroup::renderMode = GL_POINTS;
				glPolygonMode(GL_FRONT_AND_BACK, RenderGroup::renderMode);
			}
			else {
				RenderGroup::renderMode = GL_TRIANGLES;
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
	}

	if (Application->focusedMenu == "Editor" || Application->focusedMenu == "Hierarchy") {
		if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS && Editor::selectedGameObject) {
			uint64_t uuid = Editor::selectedGameObject->uuid;
			Editor::selectedGameObject->~Entity();
			auto it = Application->activeScene->entities.find(uuid); // Find the iterator for the key
			if (it != Application->activeScene->entities.end()) {
				Application->activeScene->entities.erase(it); // Erase the element if found
			}
			Editor::selectedGameObject = nullptr;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		Editor::Gui::sceneViewUsingEditorCamera = !Editor::Gui::sceneViewUsingEditorCamera;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && Application->focusedMenu == "Scene") {
#ifdef GAME_REL
		glfwSetWindowShouldClose(Application->Window->glfwWindow, true);
#else
		ImGui::SetWindowFocus("Editor");
#endif
	}

	if (glfwGetKey(window, GLFW_KEY_PAGE_UP))
		glfwSetInputMode(Application->Window->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	else if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN))
		glfwSetInputMode(Application->Window->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);



}