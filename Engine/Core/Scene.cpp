#include "Engine/Core/PreCompiledHeaders.h"
#include "Scene.h"
namespace Engine {
	Scene* Scene::Copy(Scene* newScene, Scene* copyScene) {
		for (auto& gameObj : copyScene->gameObjects) {
			GameObject* newObj = new GameObject(*gameObj);
			newObj->transform = new Transform(*gameObj->transform);
			newObj->transform->gameObject = newObj;
			newObj->transform->scene = "Runtime Scene";
			newObj->ReplaceComponent<Transform>(newObj->GetComponent<Transform>(), newObj->transform);

			if (gameObj->GetComponent<MeshRenderer>()) {
				MeshRenderer* newMeshRenderer = new MeshRenderer(*(gameObj->GetComponent<MeshRenderer>()->mesh));
				newMeshRenderer->gameObject = newObj;
				newMeshRenderer->gameObjectUUID = gameObj->uuid;
				newObj->ReplaceComponent<MeshRenderer>(newObj->GetComponent<MeshRenderer>(), newMeshRenderer);
				newScene->meshRenderers.push_back(newMeshRenderer);
			}


			newObj->transform->UpdateChildrenTransform();

			newScene->gameObjects.push_back(std::make_unique<GameObject>(*newObj));
			newScene->gameObjectsMap.emplace(newObj->uuid, newObj);
		}
		for (auto& gameObj : copyScene->gameObjects) {
			if (gameObj->parent != nullptr && copyScene->gameObjects.find(gameObj->parent->uuid) != nullptr)
				Application->activeScene->gameObjects.find(gameObj->uuid)->parent = copyScene->gameObjects.find(gameObj->parent->uuid);
		}
		return newScene;
	}

	void Scene::RemoveMeshRenderer(uint64_t uuid) {
		auto it = std::find_if(meshRenderers.begin(), meshRenderers.end(), [uuid](MeshRenderer* meshRenderer) {
			return meshRenderer->gameObjectUUID == uuid;
			});

		if (it != meshRenderers.end()) {
			meshRenderers.erase(it);
		}
	}
}