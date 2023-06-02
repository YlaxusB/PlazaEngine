#include <list>
#include <string>
#include "Engine/Components/Core/GameObject.h"


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