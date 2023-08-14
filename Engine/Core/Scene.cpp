#include "Engine/Core/PreCompiledHeaders.h"
#include "Scene.h"
namespace Engine {
	Scene* Scene::Copy(Scene* newScene, Scene* copyScene) {
		newScene->mainSceneEntity = new GameObject(*copyScene->mainSceneEntity);
		for (auto& gameObj : copyScene->entities) {
			GameObject* newObj = new GameObject(gameObj.second);
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
				newObj->RemoveComponent<MeshRenderer>();
				newObj->AddComponent<MeshRenderer>(newMeshRenderer);
				//newObj->ReplaceComponent<MeshRenderer>(newObj->GetComponent<MeshRenderer>(), newMeshRenderer);
				//newScene->meshRenderers.push_back(newMeshRenderer);
			}




			//newScene->gameObjectsMap.emplace(newObj->uuid, newObj.get());
			uint64_t uuid = newObj->uuid;
			newScene->entities.emplace(uuid, std::move(*newObj));
			//newScene->gameObjects.push_back(std::make_unique<GameObject>(newObj.get()));
			//delete(newObj);
		}
		newScene->meshes = vector<shared_ptr<Mesh>>(copyScene->meshes);
		newScene->transformComponents = std::unordered_map<uint64_t, Transform>(copyScene->transformComponents);
		newScene->meshRendererComponents = std::unordered_map<uint64_t, MeshRenderer>(copyScene->meshRendererComponents);
		for (auto& gameObj : copyScene->gameObjects) {
			if (gameObj->parentUuid != 0 && copyScene->entities.find(gameObj->parentUuid) != copyScene->entities.end())
				Application->activeScene->entities.find(gameObj->uuid)->second.parentUuid = copyScene->entities.find(gameObj->parentUuid)->second.uuid;
			else if (gameObj->parentUuid == 0)
				Application->activeScene->entities.find(gameObj->uuid)->second.parentUuid = copyScene->mainSceneEntity->uuid;
		}
		return newScene;
	}

	void Scene::RemoveMeshRenderer(uint64_t uuid) {
		auto it = std::find_if(meshRenderers.begin(), meshRenderers.end(), [uuid](MeshRenderer* meshRenderer) {
			return meshRenderer->uuid == uuid;
			});

		if (it != meshRenderers.end()) {
			meshRenderers.erase(it);
		}
	}
}