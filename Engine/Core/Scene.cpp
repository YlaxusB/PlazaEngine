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
	Scene* Scene::Copy(Scene* newScene, Scene* copyScene) {
		newScene = new Scene(*copyScene);
		newScene->mainSceneEntity = new Entity(*copyScene->mainSceneEntity);
		for (auto& gameObj : copyScene->entities) {
			Entity* newObj = new Entity(gameObj.second);
			const auto& it = copyScene->transformComponents.find(gameObj.second.uuid);
			if (it != copyScene->transformComponents.end()) {
				Transform* transform = new Transform(*gameObj.second.GetComponent<Transform>());
				transform->mUuid = newObj->uuid;
				transform->scene = "Runtime Scene";
				transform->scene = "Runtime Scene";
				newObj->RemoveComponent<Transform>();
				newObj->AddComponent<Transform>(transform);
				newObj->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
			}
			//newObj->ReplaceComponent<Transform>(newObj->GetComponent<Transform>(), newObj->transform);
			//MeshRenderer* meshRenderer = gameObj.second.GetComponent<MeshRenderer>();
			//if (gameObj.second.GetComponent<MeshRenderer>()) {
			//	MeshRenderer* newMeshRenderer;
			//	if(gameObj.second.GetComponent<MeshRenderer>()->mesh.get())
			//		newMeshRenderer = new MeshRenderer(*(gameObj.second.GetComponent<MeshRenderer>()->mesh));
			//	else {
			//		newMeshRenderer = new MeshRenderer();
			//		newMeshRenderer->uuid = Plaza::UUID::NewUUID();
			//		Application::Get()->activeScene->meshRenderers.emplace_back(newMeshRenderer);
			//	}

			//	newMeshRenderer->uuid = newObj->uuid;
			//	//newObj->RemoveComponent<MeshRenderer>();
			//	newScene->meshRendererComponents.emplace()
			//	//newObj->AddComponent<MeshRenderer>(newMeshRenderer);
			//	//newObj->ReplaceComponent<MeshRenderer>(newObj->GetComponent<MeshRenderer>(), newMeshRenderer);
			//	//newScene->meshRenderers.push_back(newMeshRenderer);
			//}




			//newScene->gameObjectsMap.emplace(newObj->uuid, newObj.get());
			uint64_t uuid = newObj->uuid;
			newScene->entities.emplace(uuid, std::move(*newObj));
			//newScene->gameObjects.push_back(std::make_unique<Entity>(newObj.get()));
			//delete(newObj);
		}
		newScene->rigidBodyComponents.clear();
		newScene->colliderComponents.clear();
		//newScene->transformComponents = ComponentMultiMap<uint64_t, Transform>(copyScene->transformComponents);
		//newScene->cameraComponents = ComponentMultiMap<uint64_t, Camera>(copyScene->cameraComponents);
		//newScene->meshRendererComponents = ComponentMultiMap<uint64_t, MeshRenderer>(copyScene->meshRendererComponents);
		//newScene->csScriptComponents = ComponentMultiMap<uint64_t, CsScriptComponent>(copyScene->csScriptComponents);
		//newScene->UITextRendererComponents = ComponentMultiMap<uint64_t, Plaza::Drawing::UI::TextRenderer>(copyScene->UITextRendererComponents);
		//newScene->audioSourceComponents = ComponentMultiMap<uint64_t, AudioSource>(copyScene->audioSourceComponents);
		//newScene->audioListenerComponents = ComponentMultiMap<uint64_t, AudioListener>(copyScene->audioListenerComponents);
		//newScene->lightComponents = ComponentMultiMap<uint64_t, Light>(copyScene->lightComponents);
		//newScene->characterControllerComponents = ComponentMultiMap<uint64_t, CharacterController>(copyScene->characterControllerComponents);
		//newScene->entitiesNames = std::unordered_map<std::string, std::unordered_set<uint64_t>>(copyScene->entitiesNames);
		//
		////newScene->meshes = unordered_map<uint64_t, shared_ptr<Mesh>>(copyScene->meshes);
		//newScene->materials = std::unordered_map<uint64_t, std::shared_ptr<Material>>(copyScene->materials);
		//newScene->materialsNames = std::unordered_map<std::string, uint64_t>(copyScene->materialsNames);
		//
		//newScene->renderGroups = std::unordered_map<uint64_t, RenderGroup*>();
		//newScene->renderGroupsFindMap = std::unordered_map<std::pair<uint64_t, std::vector<uint64_t>>, uint64_t, PairHash>();
		//newScene->renderGroupsFindMapWithMeshUuid = std::unordered_map<uint64_t, uint64_t>(); 
		//newScene->renderGroupsFindMapWithMaterialUuid = std::unordered_map<std::vector<uint64_t>, uint64_t, VectorHash, VectorEqual<uint64_t>>();

		for (auto& [key, value] : newScene->meshRendererComponents) {
			if (value.mMaterials.size() > 0 && value.mesh) {
				RenderGroup* newRenderGroup = new RenderGroup(value.mesh, value.mMaterials);
				//Application::Get()->activeScene->entities.at(uuid).GetComponent<MeshRenderer>()->renderGroup = Application::Get()->activeScene->AddRenderGroup(std::shared_ptr<RenderGroup>(newRenderGroup));
				value.renderGroup = newScene->AddRenderGroup(newRenderGroup);
			}
		}
		//newScene->renderGroups = std::unordered_map<uint64_t, shared_ptr<RenderGroup>>(copyScene->renderGroups);
		//newScene->renderGroupsFindMap = std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, PairHash>(copyScene->renderGroupsFindMap);
		//newScene->rederGroupsFindMapWithMeshUuid = std::unordered_map<uint64_t, uint64_t>(copyScene->rederGroupsFindMapWithMeshUuid);
		//newScene->rederGroupsFindMapWithMaterialUuid = std::unordered_map<uint64_t, uint64_t>(copyScene->rederGroupsFindMapWithMaterialUuid);

		for (auto& [key, value] : copyScene->colliderComponents) {
			// RigidBody* rig = &copyScene->rigidBodyComponents.at(key);
			if (copyScene->rigidBodyComponents.find(key) == copyScene->rigidBodyComponents.end()) {
				Collider* collider = new Collider(value);
				collider->Init(nullptr);
				newScene->colliderComponents.emplace(key, *collider);
			}
			else {
				newScene->colliderComponents.emplace(key, value);
			}
		}


		//newScene->colliderComponents = std::unordered_map<uint64_t, Collider>(copyScene->colliderComponents);
		//newScene->rigidBodyComponents = std::unordered_map<uint64_t, RigidBody>(copyScene->rigidBodyComponents);
		//for (auto& gameObj : copyScene->gameObjects) {
		//	if (gameObj->parentUuid != 0 && copyScene->entities.find(gameObj->parentUuid) != copyScene->entities.end())
		//		Application::Get()->activeScene->entities.find(gameObj->uuid)->second.parentUuid = copyScene->entities.find(gameObj->parentUuid)->second.uuid;
		//	else if (gameObj->parentUuid == 0)
		//		Application::Get()->activeScene->entities.find(gameObj->uuid)->second.parentUuid = copyScene->mainSceneEntity->uuid;
		//}

		for (auto& [key, value] : copyScene->rigidBodyComponents) {
			RigidBody* rigidBody = new RigidBody(value);
			rigidBody->mUuid = key;
			newScene->rigidBodyComponents.emplace(key, *rigidBody);
		}

		newScene->RegisterMaps();
		return newScene;
	}

	Scene::Scene() {
		this->mAssetUuid = Plaza::UUID::NewUUID();
		this->RegisterMaps();


		Material* defaultMaterial = new Material();
		defaultMaterial->mAssetUuid = 0;
		//		defaultMaterial->diffuse->rgba = glm::vec4(1.0f);
		//		defaultMaterial->shininess = 3.0f;
		defaultMaterial->mAssetName = "Default Material";
		defaultMaterial->mAssetUuid = 0;
		AssetsManager::AddMaterial(defaultMaterial);
	}

	void Scene::RemoveMeshRenderer(uint64_t uuid) {
		auto it = std::find_if(Application::Get()->editorScene->meshRenderers.begin(), Application::Get()->editorScene->meshRenderers.end(), [uuid](MeshRenderer* meshRenderer) {
			return meshRenderer->mUuid == uuid;
			});

		if (it != Application::Get()->editorScene->meshRenderers.end()) {
			Application::Get()->editorScene->meshRenderers.erase(it);
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

		Application::Get()->runtimeScene = new Scene();
		Application::Get()->copyingScene = true;
		Application::Get()->runtimeScene = Scene::Copy(Application::Get()->runtimeScene, Application::Get()->editorScene);
		Application::Get()->activeScene = Application::Get()->runtimeScene;
		Application::Get()->copyingScene = false;
		Application::Get()->runningScene = true;
		for (auto& [key, collider] : Application::Get()->activeScene->colliderComponents) {
			collider.UpdateShapeScale(Application::Get()->activeScene->transformComponents.at(collider.mUuid).GetWorldScale());;
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
		for (auto& [key, value] : Application::Get()->activeScene->rigidBodyComponents) {
			value.Init();
		}

		for (auto& [key, value] : Application::Get()->activeScene->characterControllerComponents) {
			value.Init();
		}

		if (scriptDllExists) {
			Mono::OnStartAll(false);
			FieldManager::ApplyAllScritpsFields(allFields);
			for (auto [key, value] : Application::Get()->activeScene->csScriptComponents) {
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


		for (auto [key, value] : Application::Get()->activeScene->audioSourceComponents) {
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
		Application::Get()->activeScene = Application::Get()->editorScene;
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
			if (Application::Get()->activeScene->entities.find(child) != Application::Get()->activeScene->entities.end())
				Application::Get()->activeScene->RemoveEntity(child);
			//Application::Get()->activeScene->entities.at(child).~Entity();
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
		if (Application::Get()->activeScene->entitiesNames.find(entity->name) != Application::Get()->activeScene->entitiesNames.end())
			Application::Get()->activeScene->entitiesNames.erase(Application::Get()->activeScene->entitiesNames.find(entity->name));

		Application::Get()->activeScene->entities.extract(entity->uuid);
	}

	Entity* Scene::GetEntity(uint64_t uuid) {
		auto it = entities.find(uuid);
		if (it != entities.end())
			return &it->second;
		return nullptr;
	}
	Entity* Scene::GetEntityByName(std::string name) {
		if (Application::Get()->activeScene->entitiesNames.find(name) != Application::Get()->activeScene->entitiesNames.end()) {
			for (const auto& element : Application::Get()->activeScene->entitiesNames.at(name)) {
				if (Application::Get()->activeScene->entities.find(element) != Application::Get()->activeScene->entities.end())
					return &Application::Get()->activeScene->entities.at(element);
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
			value.mesh = AssetsManager::GetMesh(value.mMeshUuid);
			value.renderGroup = this->AddRenderGroup(AssetsManager::GetMesh(value.mMeshUuid), AssetsManager::GetMaterialsVector(value.mMaterialsUuids));
		}
		this->mainSceneEntity = this->GetEntity(this->mainSceneEntityUuid);
		this->mainSceneEntity->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();

	}
}