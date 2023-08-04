#pragma once
#include "Engine/Components/Core/GameObject.h"
#include <vector>
#include <variant>
namespace Engine {
	class GameObjectList : public std::vector<std::unique_ptr<GameObject>> {
	public:
		void push_back(std::unique_ptr<GameObject> obj);
		GameObject* find(std::string findName);
		GameObject* find(uint64_t findUuid);
		~GameObjectList() = default;
	};
	class Scene {
	public:
		GameObjectList gameObjects;
		std::unordered_map<std::variant< uint64_t, std::string>, GameObject*> gameObjectsMap;
		std::vector<MeshRenderer*> meshRenderers;

		static Scene* Copy(Scene* newScene, Scene* copyScn);
		~Scene() = default;
		Scene() {}

		void RemoveMeshRenderer(uint64_t uuid);
	};

}