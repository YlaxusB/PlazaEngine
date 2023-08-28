#include "Engine/Core/PreCompiledHeaders.h"
#include "Scene.h"
#include "Engine/Core/Scripting/Mono.h"
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
		newScene->meshes = map<uint64_t, shared_ptr<Mesh>>(copyScene->meshes);
		newScene->transformComponents = std::unordered_map<uint64_t, Transform>(copyScene->transformComponents);
		newScene->cameraComponents = std::unordered_map<uint64_t, Camera>(copyScene->cameraComponents);
		newScene->meshRendererComponents = std::unordered_map<uint64_t, MeshRenderer>(copyScene->meshRendererComponents);
		newScene->cppScriptComponents = std::unordered_map<uint64_t, CppScriptComponent>(copyScene->cppScriptComponents);

		for (auto& [key, value] : copyScene->rigidBodyComponents) {
			RigidBody* rigidBody = new RigidBody(value);
			//RigidBody* rigidBody = new RigidBody(value);
			rigidBody->uuid = key;
			newScene->rigidBodyComponents.emplace(key, *rigidBody);
			newScene->rigidBodyComponents.at(key).Init();
		}
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
		return newScene;
	}

	Scene::Scene() {
		componentMaps.emplace("class Plaza::Transform", transformComponents);
		componentMaps.emplace("class Plaza::MeshRenderer", meshRendererComponents);
		componentMaps.emplace("class Plaza::RigidBody", rigidBodyComponents);
		componentMaps.emplace("class Plaza::Collider", colliderComponents);

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
		Application->runtimeScene = new Scene();
		Application->copyingScene = true;
		Application->runtimeScene = Scene::Copy(Application->runtimeScene, Application->editorScene);
		Application->activeScene = Application->runtimeScene;
		Application->copyingScene = false;
		Application->runningScene = true;

		Mono::OnStart();
	}
	void Scene::Stop() {
		// Change active scene, update the selected object scene, delete runtime and set running to false.
		Editor::selectedGameObject = nullptr;
		delete(Application->runtimeScene);
		Application->runningScene = false;
		Application->activeScene = Application->editorScene;
		Application->activeCamera = Application->editorCamera;
	}
	void Scene::Pause() {

	}
}