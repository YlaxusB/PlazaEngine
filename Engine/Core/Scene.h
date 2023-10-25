#pragma once
#include <vector>
#include <variant>
#include <unordered_map>

#include "Engine/Components/Core/Entity.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
#include "Engine/Components/Rendering/Material.h"
#include "Engine/Components/Physics/RigidBody.h"
#include "Engine/Components/Physics/Collider.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
#include "Engine/Components/Drawing/UI/TextRenderer.h"
#include "Engine/Components/Audio/AudioSource.h"
#include "Engine/Components/Audio/AudioListener.h"

#include "Engine/Core/RenderGroup.h"


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

	struct RenderGroupKey
	{
		uint64_t first;
		uint64_t second;

		bool operator==(const RenderGroupKey& other) const
		{
			return (first == other.first
				&& second == other.second);
		}
	};

	struct PairHash {
		std::size_t operator () (const std::pair<uint64_t, uint64_t>& p) const {
			// Combine the hash values of the two uint64_t values
			std::size_t h1 = std::hash<uint64_t>{}(p.first);
			std::size_t h2 = std::hash<uint64_t>{}(p.second);
			return h1 ^ h2;
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
		std::unordered_map<uint64_t, AudioSource> audioSourceComponents;
		std::unordered_map<uint64_t, AudioListener> audioListenerComponents;

		std::vector<MeshRenderer*> meshRenderers;
		std::unordered_map<uint64_t, shared_ptr<Mesh>> meshes;
		std::unordered_map<uint64_t, Material*> materials;

		std::unordered_map<uint64_t, shared_ptr<RenderGroup>> renderGroups;
		std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, PairHash> renderGroupsFindMap;

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
		T* GetComponent(uint64_t uuid) {
			return GetEntity(uuid)->GetComponent<T>();
		}
		template<typename T>
		bool HasComponent(uint64_t uuid) {
			return GetEntity(uuid)->HasComponent<T>();
			//return map.find(uuid) != map.end();
		}
		template<typename T>
		void RemoveComponent(uint64_t uuid) {
			return GetEntity(uuid)->RemoveComponent<T>();
		}

		void RemoveMeshRenderer(uint64_t uuid);
	};
}