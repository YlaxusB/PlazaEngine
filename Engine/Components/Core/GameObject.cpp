#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Core/GameObject.h"

GameObject::GameObject(std::string objName, GameObject* parent) {
	this->transform = this->AddComponent<Transform>(new Transform());
	transform->gameObject = this;
	//transform->gameObject = this;
	//this->AddComponent<Transform>(this->transform);
	UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
	name = objName;
	id = Application->activeScene->gameObjects.size() > 0 ? Application->activeScene->gameObjects.back()->id + 1 : 1; // IT WILL PROBABLY BREAK IN THE NEAR FUTURE

	// Set the new parent
	this->parent = parent;
	if (parent != nullptr) {
		parent->children.push_back(this);
	}
	// Add to the gameobjects list
	Application->activeScene->gameObjects.push_back(std::unique_ptr<GameObject>(this));
}

void GameObjectList::push_back(std::unique_ptr<GameObject> obj) {
	Application->activeScene->gameObjectsMap.emplace(obj->name, obj.get());
	std::vector<std::unique_ptr<GameObject>>::push_back(std::move(obj));
}
GameObject* GameObjectList::find(std::string findName) {;
	auto it = Application->activeScene->gameObjectsMap.find(findName);
	if (it != Application->activeScene->gameObjectsMap.end()) {
		GameObject* obj = it->second;
		return obj;
	}
	return nullptr;
}


GameObjectList gameObjects;
std::unordered_map<std::string, GameObject*> gameObjectsMap;

std::vector<MeshRenderer*> meshRenderers;

GameObject* sceneObject = new GameObject("Scene Objects", nullptr);

MeshRenderer::MeshRenderer(Engine::Mesh initialMesh) {
	this->mesh = std::make_unique<Mesh>(initialMesh);
	Application->activeScene->meshRenderers.emplace_back(this);
}