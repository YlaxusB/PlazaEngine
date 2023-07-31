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
				newObj->ReplaceComponent<MeshRenderer>(newObj->GetComponent<MeshRenderer>(), newMeshRenderer);
				newScene->meshRenderers.push_back(newMeshRenderer);
			}


			newObj->transform->UpdateChildrenTransform();

			newScene->gameObjects.push_back(std::make_unique<GameObject>(*newObj));
			newScene->gameObjectsMap.emplace(newObj->name, newObj);
		}
		for (auto& gameObj : copyScene->gameObjects) {
			if (gameObj->parent != nullptr && copyScene->gameObjects.find(gameObj->parent->name) != nullptr)
				Application->activeScene->gameObjects.find(gameObj->name)->parent = copyScene->gameObjects.find(gameObj->parent->name);
		}
		return newScene;
	}
}