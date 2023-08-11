#pragma once
#include "Engine/Components/Core/GameObject.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
#include <vector>
#include <variant>
#include "Engine/Components/Core/Entity.h"
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
		std::unordered_map<std::variant<uint64_t, std::string>, GameObject*> gameObjectsMap;

		unordered_map<uint64_t, GameObject> entities;
		unordered_map<uint64_t, Transform> transformComponents;
		unordered_map<uint64_t, MeshRenderer> meshRendererComponents;
		std::vector<MeshRenderer*> meshRenderers;
		std::vector<shared_ptr<Mesh>> meshes;

		//unordered_map<uint64_t, Transform*> meshRenderersComponents;

		static Scene* Copy(Scene* newScene, Scene* copyScn);
		~Scene() = default;
		Scene() {}

		void RemoveMeshRenderer(uint64_t uuid);
	};

}