#pragma once
#include "Engine/Components/Core/GameObject.h"
#include <vector>
namespace Engine {
	class GameObjectList : public std::vector<GameObject*> {
	public:
		void push_back(GameObject* obj);
		GameObject* find(std::string findName);
	};
	class Scene {
	public:
		GameObjectList gameObjects;
		std::unordered_map<std::string, GameObject*> gameObjectsMap;
		std::vector<MeshRenderer*> meshRenderers;

		static Scene* Copy(Scene* copyScn);
	};

}