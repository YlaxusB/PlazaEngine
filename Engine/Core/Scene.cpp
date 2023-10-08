#include "Engine/Core/PreCompiledHeaders.h"
#include "Scene.h"
#include "Engine/Core/Scripting/Mono.h"
#include "Engine/Components/Core/Entity.h"
#include "Editor/ScriptManager/ScriptManager.h"
#include "Engine/Core/Input/Input.h"
#include "Engine/Core/Input/Cursor.h"

#include "Engine/Core/Physics.h"

namespace Plaza {
	Scene* Scene::Copy(Scene* newScene, Scene* copyScene) {
		newScene->mainSceneEntity = new Entity(*copyScene->mainSceneEntity);
		for (auto& gameObj : copyScene->entities) {
			Entity* newObj = new Entity(gameObj.second);
			const auto& it = copyScene->transformComponents.find(gameObj.second.uuid);
			if (it != copyScene->transformComponents.end()) {
				Transform* transform = new Transform(*gameObj.second.GetComponent<Transform>());
				transform->uuid = newObj->uuid;
				transform->scene = "Runtime Scene";
				transform->scene = "Runtime Scene";
				newObj->RemoveComponent<Transform>();
				newObj->AddComponent<Transform>(transform);
				newObj->GetComponent<Transform>()->UpdateChildrenTransform();
			}
			//newObj->ReplaceComponent<Transform>(newObj->GetComponent<Transform>(), newObj->transform);
			MeshRenderer* meshRenderer = gameObj.second.GetComponent<MeshRenderer>();
			if (gameObj.second.GetComponent<MeshRenderer>()) {
				MeshRenderer* newMeshRenderer = new MeshRenderer(*(gameObj.second.GetComponent<MeshRenderer>()->mesh));
				newMeshRenderer->uuid = newObj->uuid;
				//newObj->RemoveComponent<MeshRenderer>();
				newObj->AddComponent<MeshRenderer>(newMeshRenderer);
				//newObj->ReplaceComponent<MeshRenderer>(newObj->GetComponent<MeshRenderer>(), newMeshRenderer);
				//newScene->meshRenderers.push_back(newMeshRenderer);
			}




			//newScene->gameObjectsMap.emplace(newObj->uuid, newObj.get());
			uint64_t uuid = newObj->uuid;
			newScene->entities.emplace(uuid, std::move(*newObj));
			//newScene->gameObjects.push_back(std::make_unique<Entity>(newObj.get()));
			//delete(newObj);
		}
		newScene->meshes = unordered_map<uint64_t, shared_ptr<Mesh>>(copyScene->meshes);
		newScene->transformComponents = std::unordered_map<uint64_t, Transform>(copyScene->transformComponents);
		newScene->cameraComponents = std::unordered_map<uint64_t, Camera>(copyScene->cameraComponents);
		newScene->meshRendererComponents = std::unordered_map<uint64_t, MeshRenderer>(copyScene->meshRendererComponents);
		newScene->csScriptComponents = std::unordered_multimap<uint64_t, CsScriptComponent>(copyScene->csScriptComponents);
		newScene->UITextRendererComponents = std::unordered_multimap<uint64_t, Plaza::Drawing::UI::TextRenderer>(copyScene->UITextRendererComponents);
		newScene->entitiesNames = std::unordered_map<std::string, std::unordered_set<uint64_t>>(copyScene->entitiesNames);


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
		for (auto& gameObj : copyScene->gameObjects) {
			if (gameObj->parentUuid != 0 && copyScene->entities.find(gameObj->parentUuid) != copyScene->entities.end())
				Application->activeScene->entities.find(gameObj->uuid)->second.parentUuid = copyScene->entities.find(gameObj->parentUuid)->second.uuid;
			else if (gameObj->parentUuid == 0)
				Application->activeScene->entities.find(gameObj->uuid)->second.parentUuid = copyScene->mainSceneEntity->uuid;
		}

		for (auto& [key, value] : copyScene->rigidBodyComponents) {
			RigidBody* rigidBody = new RigidBody(value);
			rigidBody->uuid = key;
			newScene->rigidBodyComponents.emplace(key, *rigidBody);
		}

		return newScene;
	}

	Scene::Scene() {

		//componentMaps.emplace("class Plaza::Transform", transformComponents);
		//componentMaps.emplace("class Plaza::MeshRenderer", meshRendererComponents);
		//componentMaps.emplace("class Plaza::RigidBody", rigidBodyComponents);
		//componentMaps.emplace("class Plaza::Collider", colliderComponents);
		//componentMaps.emplace("class Plaza::Camera", cameraComponents);
		//componentMaps.emplace("class Plaza::CsScriptComponent", csScriptComponents);

	}

	void Scene::RemoveMeshRenderer(uint64_t uuid) {
		auto it = std::find_if(meshRenderers.begin(), meshRenderers.end(), [uuid](MeshRenderer* meshRenderer) {
			return meshRenderer->uuid == uuid;
			});

		if (it != meshRenderers.end()) {
			meshRenderers.erase(it);
		}
	}

	void Scene::Play() {
		// Create a new empty Scene, change active scene to runtime, copy the contents of editor scene into runtime scene and update the selected object scene

		/* Stop mono */
			/* Load the new domain */
		mono_set_assemblies_path("lib/mono");
		//mono_set_assemblies_path((Application->editorPath + "/lib/mono").c_str());
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
		std::string dllPath = (Application->projectPath + "\\Binaries\\" + std::filesystem::path{ Application->activeProject->name }.stem().string() + ".dll");
		std::string newPath = (Application->projectPath + "\\Binaries\\" + std::filesystem::path{ Application->activeProject->name }.stem().string() + "copy.dll");
		std::filesystem::copy_file(dllPath, newPath, filesystem::copy_options::overwrite_existing);

		Application->runtimeScene = new Scene();
		Application->copyingScene = true;
		Application->runtimeScene = Scene::Copy(Application->runtimeScene, Application->editorScene);
		Application->activeScene = Application->runtimeScene;
		Application->copyingScene = false;
		Application->runningScene = true;
#ifndef GAME_REL
		ImGui::SetWindowFocus("Scene");
#endif
		int width, height;
		glfwGetWindowSize(Application->Window->glfwWindow, &width, &height);
		glfwSetCursorPos(Application->Window->glfwWindow, width / 2, height / 2);
		Input::Cursor::lastX = 0;
		Input::Cursor::lastY = 0;
		Input::Cursor::deltaX = 0;
		Input::Cursor::deltaY = 0;
		// Init Rigid Bodies
		for (auto& [key, value] : Application->activeScene->rigidBodyComponents) {
			value.Init();
		}
		Mono::OnStartAll();
	}
	void Scene::Stop() {
		// Change active scene, update the selected object scene, delete runtime and set running to false.
		Editor::selectedGameObject = nullptr;
		delete(Application->runtimeScene);
		Application->runningScene = false;
		Application->activeScene = Application->editorScene;
		Application->activeCamera = Application->editorCamera;
		Editor::ScriptManager::ReloadScriptsAssembly();
	}
	void Scene::Pause() {

	}

	Entity* Scene::GetEntity(uint64_t uuid) {
		auto it = Application->activeScene->entities.find(uuid);
		if (it != Application->activeScene->entities.end())
			return &it->second;
	}
	Entity* Scene::GetEntityByName(std::string name) {
		if (Application->activeScene->entitiesNames.find(name) != Application->activeScene->entitiesNames.end()) {
			for (const auto& element : Application->activeScene->entitiesNames.at(name)) {
				return &Application->activeScene->entities.at(element);
			}
		}
		return nullptr;
	}
}