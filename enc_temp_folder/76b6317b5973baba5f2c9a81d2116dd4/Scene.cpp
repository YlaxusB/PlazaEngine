#include "Engine/Core/PreCompiledHeaders.h"
#include "Scene.h"
namespace Engine {
	Scene* Scene::Copy(Scene* newScene, Scene* copyScene) {
		int ind = 0;
		for (auto& gameObj : copyScene->gameObjects) {
			std::unique_ptr<GameObject> newObj = make_unique<GameObject>(*gameObj);
			newObj->transform = new Transform(*gameObj->transform);
			newObj->transform->gameObject = newObj.get();
			newObj->transform->scene = "Runtime Scene";
			newObj->scene = "Runtime Scene";
			newObj->ReplaceComponent<Transform>(newObj->GetComponent<Transform>(), newObj->transform);
			MeshRenderer* meshRenderer = gameObj->GetComponent<MeshRenderer>();
			if (gameObj->GetComponent<MeshRenderer>()) {
				MeshRenderer* newMeshRenderer = new MeshRenderer(*(gameObj->GetComponent<MeshRenderer>()->mesh));
				newMeshRenderer->gameObject = newObj.get();
				newMeshRenderer->gameObjectUUID = gameObj->uuid;
				newObj->ReplaceComponent<MeshRenderer>(newObj->GetComponent<MeshRenderer>(), newMeshRenderer);
				//newScene->meshRenderers.push_back(newMeshRenderer);
			}


			newObj->transform->UpdateChildrenTransform();

			newScene->gameObjectsMap.emplace(newObj->uuid, newObj.get());
			newScene->gameObjects.push_back(std::move(newObj));
			//newScene->gameObjects.push_back(std::make_unique<GameObject>(newObj.get()));
			ind++;
			//delete(newObj);
		}
		for (auto& gameObj : copyScene->gameObjects) {
			if (gameObj->parent != nullptr && copyScene->gameObjects.find(gameObj->parent->uuid) != nullptr)
				Application->activeScene->gameObjects.find(gameObj->uuid)->parent = copyScene->gameObjects.find(gameObj->parent->uuid);
			else if (gameObj->parent == nullptr)
				Application->activeScene->gameObjects.find(gameObj->uuid)->parent = copyScene->gameObjects.front().get();
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