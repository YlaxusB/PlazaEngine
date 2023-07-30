#include "Engine/Core/PreCompiledHeaders.h"
#include "Scene.h"
namespace Engine {
	Scene* Scene::Copy(Scene* copyScn) {
		Scene* newScn = new Scene();
		for (auto& gameObj : copyScn->gameObjects) {
			GameObject* newObj = new GameObject(*gameObj);

			newObj->transform = new Transform(*gameObj->transform);
			newObj->transform->gameObject = newObj;
			newObj->transform->scene = "Runtime Scene";
			Transform& newTransform = *newObj->GetComponent<Transform>();
			newTransform = *newObj->transform;
			newTransform = *newObj->transform;
			newTransform = *newObj->GetComponent<Transform>();
			newTransform = *newObj->GetComponent<Transform>();
			newObj->ReplaceComponent<Transform>(newObj->GetComponent<Transform>(), newObj->transform);
			newScn->gameObjects.push_back(newObj);
			newScn->gameObjectsMap.emplace(newObj->name, newObj);
			if (gameObj->GetComponent<MeshRenderer>()) {
				MeshRenderer* newMeshRenderer = new MeshRenderer(*newObj->GetComponent<MeshRenderer>());
				newMeshRenderer->gameObject = newObj;
				MeshRenderer* meshR = newObj->GetComponent<MeshRenderer>();
				meshR = newMeshRenderer;
				newScn->meshRenderers.emplace_back(newMeshRenderer);
			}
			newObj->transform->UpdateChildrenTransform();
		}
		for (auto& gameObj : copyScn->gameObjects) {
			if (gameObj->parent != nullptr)
				Application->actScn->gameObjects.find(gameObj->name)->parent = copyScn->gameObjects.find(gameObj->parent->name);
		}
		return newScn;
	}
}