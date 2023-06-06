#include <list>
#include <string>
#include "Engine/Components/Core/GameObject.h"

GameObject::GameObject(std::string objName, GameObject* parent) {
	this->transform = this->AddComponent<Transform>(new Transform());
	transform->gameObject = this;
	//transform->gameObject = this;
	//this->AddComponent<Transform>(this->transform);
	UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
	name = objName;
	id = gameObjects.size() > 0 ? gameObjects.back()->id + 1 : 1; // IT WILL PROBABLY BREAK IN THE NEAR FUTURE

	// Set the new parent
	this->parent = parent;
	if (parent != nullptr) {
		parent->children.push_back(this);
	}
	// Add to the gameobjects list
	gameObjects.push_back(this);
}

void GameObjectList::push_back(GameObject* obj) {
	gameObjectsMap.emplace(obj->name, obj);
	std::list<GameObject*>::push_back(obj);
}
GameObject* GameObjectList::find(std::string findName) {
	std::cout << "a" << std::endl;
	auto it = gameObjectsMap.find(findName);
	std::cout << "b" << std::endl;
	if (it != gameObjectsMap.end()) {
		std::cout << "c" << std::endl;
		GameObject* obj = it->second;
		std::cout << "d" << std::endl;
		return obj;
	}
	std::cout << "e" << std::endl;
	return nullptr;
}


GameObjectList gameObjects;
std::unordered_map<std::string, GameObject*> gameObjectsMap;

GameObject* sceneObject = new GameObject("Scene Objects", nullptr);