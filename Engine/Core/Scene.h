#pragma once
#include <vector>
#include <variant>
#include <unordered_map>

#include "Engine/Components/Core/Entity.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
#include "Engine/Components/Physics/RigidBody.h"
#include "Engine/Components/Physics/Collider.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
#include "Engine/Components/Drawing/UI/TextRenderer.h"


#include <unordered_set>
#include "Engine/Core/Standards.h"
using namespace std;
namespace Plaza {
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

	class GameObjectList : public std::vector<std::unique_ptr<Entity>> {
	public:
		void push_back(std::unique_ptr<Entity> obj);
		Entity* find(std::string findName);
		Entity* find(uint64_t findUuid);
		~GameObjectList() = default;
	};
	class Scene {
	public:
		std::string filePath = "mainScene.plzscn";
		GameObjectList gameObjects;
		std::unordered_map<std::variant<uint64_t, std::string>, Entity*> gameObjectsMap;

		Entity* mainSceneEntity;

		std::unordered_map<std::string, std::any> componentMaps;

		std::unordered_map<uint64_t, Entity> entities;
		std::unordered_map<uint64_t, Transform> transformComponents;
		std::unordered_map<uint64_t, Camera> cameraComponents;
		std::unordered_map<uint64_t, MeshRenderer> meshRendererComponents;
		std::unordered_map<uint64_t, RigidBody> rigidBodyComponents;
		std::unordered_map<uint64_t, Collider> colliderComponents;
		std::unordered_multimap<uint64_t, CsScriptComponent> csScriptComponents;
		std::unordered_multimap<uint64_t, Plaza::Drawing::UI::TextRenderer> UITextRendererComponents;

		std::vector<MeshRenderer*> meshRenderers;
		std::unordered_map<uint64_t, shared_ptr<Mesh>> meshes;

		std::unordered_map<std::string, std::unordered_set<uint64_t>> entitiesNames;
		Scene();

		//unordered_map<uint64_t, Transform*> meshRenderersComponents;

		static Scene* Copy(Scene* newScene, Scene* copyScn);
		static void Play(); // Starts the game
		static void Stop(); // Finishes the game
		static void Pause(); // Just pauses the game
		~Scene() = default;

		Entity* GetEntity(uint64_t uuid);
		Entity* GetEntityByName(std::string name);
		template<typename T>
		T* GetComponent(uint64_t uuid);

		void RemoveMeshRenderer(uint64_t uuid);
	};
}