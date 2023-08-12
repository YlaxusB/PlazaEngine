#pragma once
#include "Engine/Components/Core/GameObject.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
#include <vector>
#include <variant>
#include "Engine/Components/Core/Entity.h"
#include <unordered_map>
using namespace std;
namespace Engine {
	struct Uint64Hash {
		size_t operator()(uint64_t key) const {
			uint64_t v = key * 3935559000370003845 + 2691343689449507681;

			v ^= v >> 21;
			v ^= v << 37;
			v ^= v >> 4;

			v *= 4768777513237032717;

			v ^= v << 20;
			v ^= v >> 41;
			v ^= v << 5;

			return v;
		}
	};

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

		GameObject* mainSceneEntity;




		std::unordered_map<uint64_t, GameObject> entities;
		std::unordered_map<uint64_t, Transform> transformComponents;
		std::unordered_map<uint64_t, MeshRenderer> meshRendererComponents;
		std::vector<MeshRenderer*> meshRenderers;
		std::vector<shared_ptr<Mesh>> meshes;

		//unordered_map<uint64_t, Transform*> meshRenderersComponents;

		static Scene* Copy(Scene* newScene, Scene* copyScn);
		~Scene() = default;
		Scene() {

		}

		void RemoveMeshRenderer(uint64_t uuid);
	};
}