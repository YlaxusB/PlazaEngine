#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Application/Serializer/SceneSerializer.h"
#include "Engine/Core/Physics.h"
#include "Editor/DefaultAssets/DefaultAssets.h"

#include "Engine/Core/Skybox.h"
#include "Editor/GUI/guiMain.h"
#include "Editor/DefaultAssets/DefaultAssets.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"
#include "Engine/Core/Input/Input.h"
#include "Editor/GUI/guiMain.h"
using namespace Plaza;
uint64_t lastUuid;

static unsigned int cascadeIndexDebug = 2;

glm::vec3 randomVec3() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(-500.0f, 500.0f);

	return glm::vec3(dis(gen), dis(gen), dis(gen));
}

Entity* NewEntity(string name, Entity* parent, Mesh* mesh, bool instanced = true, bool addToScene = true) {
	Entity* obj = new Entity(name, parent, addToScene);
	//obj->changingName = true;
	//Application::Get()->activeScene->entities.at(obj->uuid).changingName = true;
	//Gui::Hierarchy::Item::firstFocus = true;
	obj->GetComponent<Transform>()->UpdateChildrenTransform();
	MeshRenderer* meshRenderer = new MeshRenderer(mesh, Scene::DefaultMaterial());
	meshRenderer->instanced = true;
	//meshRenderer->mesh = new Mesh(*mesh);
	meshRenderer->mMaterials.push_back(Scene::DefaultMaterial());
	//RenderGroup* newRenderGroup = new RenderGroup(meshRenderer->mesh, meshRenderer->material);
	meshRenderer->renderGroup = Application::Get()->activeScene->AddRenderGroup(meshRenderer->mesh, meshRenderer->mMaterials);
	//meshRenderer->renderGroup->material = make_shared<Material>(*Scene::DefaultMaterial());
	obj->AddComponent<MeshRenderer>(meshRenderer);
	Editor::selectedGameObject = obj;

	return obj;
}
void Callbacks::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	for (CallbackFunction callbackFunction : sOnKeyPressFunctions) {
		bool isLayerFocused = Editor::Gui::sFocusedLayer == callbackFunction.layerToExecute;
		if (isLayerFocused)
			callbackFunction.function;
	}

	for (auto& tool : Editor::Gui::sEditorTools) {
		tool.second->OnKeyPress(key, scancode, action, mods);
	}

	if (Application::Get()->focusedMenu == "Scene")
		Input::isAnyKeyPressed = true;
	if (Application::Get()->focusedMenu == "Editor") {

		const std::string sceneOutput = "C:\\Users\\Giovane\\Desktop\\Workspace\\PlazaGames\\FPS\\Assets\\Scenes\\cerealScene.plzscn";
		if (key == GLFW_KEY_I && action == GLFW_PRESS) {
			Editor::Gui::OpenAssetImporterContext("asd");
			//std::ofstream os(sceneOutput, std::ios::binary);
			//cereal::BinaryOutputArchive archive(os);
			//archive(*Application::Get()->activeScene);
			//os.close();
		}
		if (key == GLFW_KEY_O && action == GLFW_PRESS) {
			Application::Get()->mThreadsManager->mFrameEndThread->AddToQueue([sceneOutput]() {
				std::ifstream is(sceneOutput, std::ios::binary);
				cereal::BinaryInputArchive archive(is);
				Scene* obj = new Scene();
				archive(*obj);
				is.close();
				obj->mainSceneEntity = obj->GetEntity(Application::Get()->activeScene->mainSceneEntity->uuid);

				Application::Get()->editorScene = obj;
				Application::Get()->activeScene = Application::Get()->editorScene;
				Application::Get()->activeScene->RecalculateAddedComponents();
				});
		}


		if (key == GLFW_KEY_F && action == GLFW_PRESS) {
			if (Editor::selectedGameObject) {
				uint64_t newUuid = Entity::Instantiate(Editor::selectedGameObject->uuid);
				if (newUuid)
					Application::Get()->activeScene->transformComponents.find(newUuid)->second.UpdateSelfAndChildrenTransform();
				Editor::selectedGameObject = Application::Get()->activeScene->GetEntity(newUuid);
			}
		}

		if (key == GLFW_KEY_T && action == GLFW_PRESS) {
			for (int i = 0; i < 1000; ++i) {
				Entity* obj = NewEntity("Sphere", Application::Get()->activeScene->mainSceneEntity, Editor::DefaultModels::Cube(), true, true);
				Transform* transform = obj->GetComponent<Transform>();
				transform->relativePosition = randomVec3();
				transform->UpdateSelfAndChildrenTransform();
				Collider* collider = new Collider(obj->uuid);
				collider->CreateShape(ColliderShape::ColliderShapeEnum::BOX, transform);
				obj->AddComponent<Collider>(collider);
				//RigidBody* rigidBody = new RigidBody(obj->uuid, false);
				//obj->AddComponent<RigidBody>(rigidBody);
			}
		}

		VulkanRenderer* vulkanRenderer = (VulkanRenderer*)Application::Get()->mRenderer;
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
			Application::Get()->showCascadeLevels = !Application::Get()->showCascadeLevels;

		if (key == GLFW_KEY_N && action == GLFW_PRESS) {
			Editor::selectedGameObject->GetComponent<MeshRenderer>()->LoadHeightMap("C:\\Users\\Giovane\\Desktop\\Workspace\\heightmap.save");
		}
		//if (key == GLFW_KEY_G && action == GLFW_PRESS)
		//	Application::Get()->activeScene->entities[Editor::selectedGameObject->uuid].RemoveComponent<RigidBody>();

		if (key == GLFW_KEY_U && action == GLFW_PRESS)
			Application::Get()->activeCamera->Position = Plaza::Editor::selectedGameObject->GetComponent<Transform>()->GetWorldPosition();

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
			if (Application::Get()->runningScene)
				Scene::Stop();
			else
				Scene::Play();
		}
	}

	if (Application::Get()->focusedMenu == "Editor" || Application::Get()->focusedMenu == "Hierarchy") {
		if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS && Editor::selectedGameObject) {
			uint64_t uuid = Editor::selectedGameObject->uuid;
			Editor::selectedGameObject->Delete();
			Editor::selectedGameObject = nullptr;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		Editor::Gui::sceneViewUsingEditorCamera = !Editor::Gui::sceneViewUsingEditorCamera;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && Application::Get()->focusedMenu == "Scene") {
#ifdef GAME_MODE
		glfwSetWindowShouldClose(Application::Get()->mWindow->glfwWindow, true);
#else
		ImGui::SetWindowFocus("Editor");
#endif
	}

#ifdef EDITOR_MODE
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP))
		glfwSetInputMode(Application::Get()->mWindow->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	else if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN))
		glfwSetInputMode(Application::Get()->mWindow->glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif


}