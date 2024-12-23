#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Core/Physics.h"
#include "Editor/DefaultAssets/DefaultAssets.h"

#include "Engine/Core/Skybox.h"
#include "Editor/GUI/guiMain.h"
#include "Editor/DefaultAssets/DefaultAssets.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"
#include "Engine/Core/Input/Input.h"
#include "Editor/GUI/guiMain.h"
#include "Engine/Application/FileDialog/FileDialog.h"

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
	// Fix: fix new entity
	////obj->changingName = true;
	////Scene::GetActiveScene()->entities.at(obj->uuid).changingName = true;
	////Gui::Hierarchy::Item::firstFocus = true;
	//obj->GetComponent<TransformComponent>()->UpdateChildrenTransform();
	//MeshRenderer* meshRenderer = new MeshRenderer(mesh, AssetsManager::GetDefaultMaterial());
	//meshRenderer->instanced = true;
	////meshRenderer->mesh = new Mesh(*mesh);
	//meshRenderer->mMaterials.push_back(AssetsManager::GetDefaultMaterial());
	////RenderGroup* newRenderGroup = new RenderGroup(meshRenderer->mesh, meshRenderer->material);
	//meshRenderer->renderGroup = Scene::GetActiveScene()->AddRenderGroup(meshRenderer->mesh, meshRenderer->mMaterials);
	////meshRenderer->renderGroup->material = make_shared<Material>(*AssetsManager::GetDefaultMaterial());
	//obj->AddComponent<MeshRenderer>(meshRenderer);
	//Editor::selectedGameObject = obj;

	return obj;
}
void Callbacks::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// FIX: Get a proper scene instead of the active scene
	Scene* scene = Scene::GetActiveScene();
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

		if (key == GLFW_KEY_K && action == GLFW_PRESS) {
			for (int i = 0; i < 100; ++i) {
				Application::Get()->mRenderer->UpdateMainProgressBar(i / 100.0f);
				std::this_thread::sleep_for(std::chrono::milliseconds(3));
			}
		}

		if (key == GLFW_KEY_F && action == GLFW_PRESS) {
			if (Editor::selectedGameObject) {
				uint64_t newUuid = ECS::EntitySystem::Instantiate(Scene::GetActiveScene(), Editor::selectedGameObject->uuid);
				if (newUuid)
					ECS::TransformSystem::UpdateSelfAndChildrenTransform(*scene->GetComponent<TransformComponent>(newUuid), scene);
				Editor::selectedGameObject = Scene::GetActiveScene()->GetEntity(newUuid);
			}
		}

		if (key == GLFW_KEY_T && action == GLFW_PRESS) {

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
			VulkanRenderGraph* graph = new VulkanRenderGraph(*AssetsSerializer::DeSerializeFile<VulkanRenderGraph>(FileDialog::OpenFileDialog(Standards::plazaRenderGraph.c_str()), Application::Get()->mSettings.mRenderGraphSerializationMode).get());
			Application::Get()->mEditor->mGui.mRenderGraphEditor->LoadRenderGraphNodes(graph);
		}
		//if (key == GLFW_KEY_G && action == GLFW_PRESS)
		//	Scene::GetActiveScene()->entities[Editor::selectedGameObject->uuid].RemoveComponent<RigidBody>();

		if (key == GLFW_KEY_U && action == GLFW_PRESS)
			Application::Get()->activeCamera->Position = scene->GetComponent<TransformComponent>(Plaza::Editor::selectedGameObject->uuid)->GetWorldPosition();

		if (key == GLFW_KEY_END && action == GLFW_PRESS) {

		}
		if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {

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
			ECS::EntitySystem::Delete(scene, Editor::selectedGameObject->uuid);
			Editor::selectedGameObject = nullptr;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		Editor::Gui::sceneViewUsingEditorCamera = !Editor::Gui::sceneViewUsingEditorCamera;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && Application::Get()->focusedMenu == "Scene") {
#ifdef EDITOR_MODE
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