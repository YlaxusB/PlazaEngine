#include "Engine/Core/PreCompiledHeaders.h"
#include "Scene.h"
#include "Engine/Core/Scripting/Mono.h"
#include "Engine/Components/Core/Entity.h"
#include "Editor/ScriptManager/ScriptManager.h"
#include "Engine/Core/Input/Input.h"
#include "Engine/Core/Input/Cursor.h"
#include "Engine/Core/Scripting/FieldManager.h"
#include "Editor/Filewatcher.h"
#include "Engine/Core/Physics.h"
#include <mutex>
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"

namespace Plaza {
	void Scene::Copy(Scene* baseScene) {
		*this = *baseScene;
		mainSceneEntity = new Entity(*baseScene->mainSceneEntity);
		for (auto& gameObj : baseScene->entities) {
			Entity* newObj = new Entity(gameObj.second);
			const auto& it = baseScene->transformComponents.find(gameObj.second.uuid);
			if (it != baseScene->transformComponents.end()) {
				Transform* transform = new Transform(*gameObj.second.GetComponent<Transform>());
				transform->mUuid = newObj->uuid;
				transform->scene = "Runtime Scene";
				transform->scene = "Runtime Scene";
				newObj->RemoveComponent<Transform>();
				newObj->AddComponent<Transform>(transform);
				newObj->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
			}

			uint64_t uuid = newObj->uuid;
			entities.emplace(uuid, std::move(*newObj));
		}
		rigidBodyComponents.clear();
		colliderComponents.clear();

		for (auto& [key, value] : meshRendererComponents) {
			if (value.mMaterials.size() > 0 && value.mesh) {
				RenderGroup* newRenderGroup = new RenderGroup(value.mesh, value.mMaterials);
				value.renderGroup = AddRenderGroup(newRenderGroup);
			}
		}

		for (auto& [key, value] : baseScene->colliderComponents) {
			if (baseScene->rigidBodyComponents.find(key) == baseScene->rigidBodyComponents.end()) {
				Collider* collider = new Collider(value);
				collider->Init(nullptr);
				colliderComponents.emplace(key, *collider);
			}
			else {
				colliderComponents.emplace(key, value);
			}
		}

		for (auto& [key, value] : baseScene->rigidBodyComponents) {
			RigidBody* rigidBody = new RigidBody(value);
			rigidBody->mUuid = key;
			rigidBodyComponents.emplace(key, *rigidBody);
		}

		RegisterMaps();
	}

	void Scene::NewRuntimeScene(Scene* baseScene) {
		
	}

	Scene::Scene() {
		Scene* oldActiveScene = Scene::GetActiveScene();
		Scene::SetActiveScene(this);
		this->mAssetUuid = Plaza::UUID::NewUUID();
		this->RegisterMaps();


		Material* defaultMaterial = new Material();
		defaultMaterial->mAssetUuid = 0;
		defaultMaterial->mAssetName = "Default Material";
		defaultMaterial->mAssetUuid = 0;
		AssetsManager::AddMaterial(defaultMaterial);

		//mainSceneEntity = new Entity("Scene");
		//mainSceneEntity->parentUuid = mainSceneEntity->uuid;
		//mainSceneEntityUuid = mainSceneEntity->uuid;

		 Scene::SetActiveScene(oldActiveScene);
	}

	void Scene::RemoveMeshRenderer(uint64_t uuid) {
		auto it = std::find_if(Scene::GetEditorScene()->meshRenderers.begin(), Scene::GetEditorScene()->meshRenderers.end(), [uuid](MeshRenderer* meshRenderer) {
			return meshRenderer->mUuid == uuid;
			});

		if (it != Scene::GetEditorScene()->meshRenderers.end()) {
			Scene::GetEditorScene()->meshRenderers.erase(it);
		}
	}

	void Scene::Play() {
		bool scriptDllExists = std::filesystem::exists(Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->mAssetName }.stem().string() + ".dll");
		/* Get fields values */
		std::map<uint64_t, std::map<std::string, std::map<std::string, Field*>>> allFields;
		if (scriptDllExists) {
			allFields = FieldManager::GetAllScritpsFields();
			Editor::lastSavedScriptsFields = allFields;
		}
		/* Stop mono */
		/* Load the new domain */
		mono_set_assemblies_path("lib/mono");
		//mono_set_assemblies_path((Application::Get()->editorPath + "/lib/mono").c_str());
		if (Mono::mMonoRootDomain == nullptr)
			Mono::mMonoRootDomain = mono_jit_init("MyScriptRuntime");
		if (Mono::mMonoRootDomain == nullptr)
		{
			// Maybe log some error here
			return;
		}
		// Create an App Domain
		char appDomainName[] = "PlazaAppDomain";
		MonoDomain* newDomain = mono_domain_create_appdomain(appDomainName, nullptr);
		mono_domain_set(newDomain, true);
		Mono::ReloadAppDomain();
		Mono::mAppDomain = newDomain;

		mono_domain_set(Mono::mAppDomain, true);
		/* Copy the script dll */
		if (scriptDllExists) {
			std::string dllPath = (Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->mAssetName }.stem().string() + ".dll");
			std::string newPath = (Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->mAssetName }.stem().string() + "copy.dll");
			std::filesystem::copy_file(dllPath, newPath, filesystem::copy_options::overwrite_existing);
		}

		/* Restart physics */
		Physics::m_scene->release();
		Physics::InitPhysics();
		Physics::InitScene();


		Scene::sRuntimeScene.reset();
		Scene::sRuntimeScene = std::make_unique<Scene>();
		Application::Get()->copyingScene = true;
		Scene::sRuntimeScene->Copy(Scene::GetEditorScene());
		Scene::SetActiveScene(Scene::GetRuntimeScene());
		Application::Get()->copyingScene = false;
		Application::Get()->runningScene = true;
		Scene::GetActiveScene()->RecalculateAddedComponents();
		for (auto& [key, collider] : Scene::GetActiveScene()->colliderComponents) {
			collider.UpdateShapeScale(Scene::GetActiveScene()->transformComponents.at(collider.mUuid).GetWorldScale());;
			collider.UpdatePose();
		}
#ifdef EDITOR_MODE
		ImGui::SetWindowFocus("Scene");
#endif
		int width, height;
		glfwGetWindowSize(Application::Get()->mWindow->glfwWindow, &width, &height);
		glfwSetCursorPos(Application::Get()->mWindow->glfwWindow, width / 2, height / 2);
		Input::Cursor::lastX = 0;
		Input::Cursor::lastY = 0;
		Input::Cursor::deltaX = 0;
		Input::Cursor::deltaY = 0;
		// Init Rigid Bodies
		for (auto& [key, value] : Scene::GetActiveScene()->rigidBodyComponents) {
			value.Init();
		}

		for (auto& [key, value] : Scene::GetActiveScene()->characterControllerComponents) {
			value.Init();
		}

		if (scriptDllExists) {
			Mono::OnStartAll(false);
			//for (auto [key, value] : Scene::GetActiveScene()->csScriptComponents) {
			//	value.Init();
			//}
			FieldManager::ApplyAllScritpsFields(allFields);
			for (auto [key, value] : Scene::GetActiveScene()->csScriptComponents) {
				for (auto& [className, classScript] : value.scriptClasses) {
					Mono::CallMethod(classScript->monoObject, classScript->onStartMethod);
				}
			}
		}
	}

	void Scene::Stop() {
		//mono_jit_cleanup(Mono::mAppDomain);
		/* Clear the queue on the main thread */
		std::lock_guard<std::mutex> lock(Editor::Filewatcher::queueMutex);
		while (!Editor::Filewatcher::taskQueue.empty()) {
			Editor::Filewatcher::taskQueue.pop();
		}


		for (auto [key, value] : Scene::GetActiveScene()->audioSourceComponents) {
			value.Stop();
		}

		mono_set_assemblies_path("lib/mono");
		//mono_set_assemblies_path((Application::Get()->editorPath + "/lib/mono").c_str());
		if (Mono::mMonoRootDomain == nullptr)
			Mono::mMonoRootDomain = mono_jit_init("MyScriptRuntime");
		if (Mono::mMonoRootDomain == nullptr)
		{
			// Maybe log some error here
			return;
		}
		// Create an App Domain
		char appDomainName[] = "PlazaAppDomain";
		MonoDomain* newDomain = mono_domain_create_appdomain(appDomainName, nullptr);
		mono_domain_set(newDomain, true);
		Mono::ReloadAppDomain();
		Mono::mAppDomain = newDomain;

		mono_domain_set(Mono::mAppDomain, true);

		// Change active scene, update the selected object scene, delete runtime and set running to false.
		Editor::selectedGameObject = nullptr;
		delete(Application::Get()->runtimeScene);
		Application::Get()->runningScene = false;
		Scene::SetActiveScene(Scene::GetEditorScene());
		Application::Get()->activeCamera = Application::Get()->editorCamera;



		bool scriptDllExists = std::filesystem::exists(Application::Get()->projectPath + "\\Binaries\\" + std::filesystem::path{ Application::Get()->activeProject->mAssetName }.stem().string() + ".dll");
		if (scriptDllExists) {
			Editor::ScriptManager::ReloadScriptsAssembly();
			FieldManager::ApplyAllScritpsFields(Editor::lastSavedScriptsFields);
		}
	}
	void Scene::Pause() {

	}

	void Scene::RemoveEntity(uint64_t uuid) {
		Entity* entity = this->GetEntity(uuid);
		std::vector<uint64_t> children = entity->childrenUuid;
		for (uint64_t child : children) {
			if (Scene::GetActiveScene()->entities.find(child) != Scene::GetActiveScene()->entities.end())
				Scene::GetActiveScene()->RemoveEntity(child);
			//Scene::GetActiveScene()->entities.at(child).~Entity();
		}
		if (entity->HasComponent<Transform>())
			entity->RemoveComponent<Transform>();
		if (entity->HasComponent<MeshRenderer>())
			entity->RemoveComponent<MeshRenderer>();
		if (entity->HasComponent<Collider>())
			entity->RemoveComponent<Collider>();
		if (entity->HasComponent<RigidBody>())
			entity->RemoveComponent<RigidBody>();
		if (entity->HasComponent<Camera>())
			entity->RemoveComponent<Camera>();
		if (entity->HasComponent<CsScriptComponent>())
			entity->RemoveComponent<CsScriptComponent>();
		if (entity->HasComponent<Plaza::Drawing::UI::TextRenderer>())
			entity->RemoveComponent<Plaza::Drawing::UI::TextRenderer>();
		if (entity->HasComponent<AudioSource>())
			entity->RemoveComponent<AudioSource>();
		if (entity->HasComponent<AudioListener>())
			entity->RemoveComponent<AudioListener>();
		if (entity->HasComponent<Light>())
			entity->RemoveComponent<Light>();

		if (Editor::selectedGameObject && Editor::selectedGameObject->uuid == entity->uuid)
			Editor::selectedGameObject = nullptr;

		entity->GetParent().childrenUuid.erase(std::remove(entity->GetParent().childrenUuid.begin(), entity->GetParent().childrenUuid.end(), entity->uuid), entity->GetParent().childrenUuid.end());
		if (Scene::GetActiveScene()->entitiesNames.find(entity->name) != Scene::GetActiveScene()->entitiesNames.end())
			Scene::GetActiveScene()->entitiesNames.erase(Scene::GetActiveScene()->entitiesNames.find(entity->name));

		Scene::GetActiveScene()->entities.extract(entity->uuid);
	}

	Entity* Scene::GetEntity(uint64_t uuid) {
		auto it = entities.find(uuid);
		if (it != entities.end())
			return &it->second;
		return nullptr;
	}
	Entity* Scene::GetEntityByName(std::string name) {
		if (Scene::GetActiveScene()->entitiesNames.find(name) != Scene::GetActiveScene()->entitiesNames.end()) {
			for (const auto& element : Scene::GetActiveScene()->entitiesNames.at(name)) {
				if (Scene::GetActiveScene()->entities.find(element) != Scene::GetActiveScene()->entities.end())
					return &Scene::GetActiveScene()->entities.at(element);
			}
		}
		return nullptr;
	}

	void Scene::RecalculateAddedComponents() {
		for (auto& [key, value] : AssetsManager::mAssets) {
			if (value->GetExtension() == Standards::materialExtName) {
				//AssetsLoader::LoadMaterial(value, this);
			}
		}
		for (auto& [key, value] : meshRendererComponents) {
			if(value.mesh)
			value.mMeshUuid = value.mesh->uuid;
			value.mesh = AssetsManager::GetMesh(value.mMeshUuid);
			value.renderGroup = this->AddRenderGroup(AssetsManager::GetMesh(value.mMeshUuid), AssetsManager::GetMaterialsVector(value.mMaterialsUuids));
		}
		this->mainSceneEntity = this->GetEntity(this->mainSceneEntityUuid);
		this->mainSceneEntity->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();

	}
}