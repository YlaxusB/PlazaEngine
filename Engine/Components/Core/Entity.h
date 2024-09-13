#pragma  once
#include <variant>
//#include "Engine/Core/Scene.h"
#include "Engine/Components/Component.h"
#include <vector>
#include <string>
#include "Engine/Application/Application.h"
#include "Engine/Core/ComponentMapStructure.h"
//#include "Engine/Core/Scene.h"
namespace Plaza {
	class Scene;

	class Entity {
	public:
		uint64_t uuid;
		uint64_t parentUuid;
		uint64_t prefabUuid; // The uuid of the prefab it belongs to
		uint64_t equivalentPrefabUuid; // The uuid of the entity in the prefab it belongs to
		uint64_t equivalentPrefabParentUuid;
		bool isPrefab = false;
		std::vector<uint64_t> childrenUuid;
		std::string name;
		bool changingName = false;

		Entity();
		Entity(std::string objName, Entity* parent = nullptr, bool addToScene = true, uint64_t newUuid = 0);
		Entity(const Entity&) = default;
		~Entity();
		void Delete();

		static std::unordered_map<std::string, void*> GetAllComponentsMaps();

		template<typename T>
		T* GetComponent() {
			Component* component = nullptr;
			std::string className = typeid(T).name();
			auto& components = *static_cast<ComponentMultiMap<uint64_t, T>*>(GetAllComponentsMaps().find(className)->second);
			auto it = components.find(this->uuid);
			if (it != components.end()) {
				component = &(it->second);
			}
			return (T*)component;
			//return dynamic_cast<T*>(component);
		}

		template<typename T>
		T* AddComponent(T* component, bool addToComponentsList = true) {
			component->mUuid = this->uuid;

			std::string className = typeid(T).name();
			if (addToComponentsList) {
				auto& components = *static_cast<ComponentMultiMap<uint64_t, T>*>(GetAllComponentsMaps()[className]);
				components.emplace(component->mUuid, *component);
			}

			return dynamic_cast<T*>(component);
		}

		template<typename T>
		T* AddComp();

		void RemoveComponent(Component* component);
		template <typename T>
		void RemoveComponent() {
			std::string className = typeid(T).name();
			auto& components = *static_cast<ComponentMultiMap<uint64_t, Component>*>(GetAllComponentsMaps()[className]);
			if (components.find(this->uuid) != components.end()) {
				components.find(this->uuid)->second.~Component();
				components.erase(components.find(this->uuid));
			}
		}
		//{
		//Plaza::GetComponentMap(T);
		//static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");

		//auto it = std::remove_if(this->components.begin(), this->components.end(),
		//	[](const std::shared_ptr<Component>& comp) {
		//		return dynamic_cast<T*>(comp.get()) != nullptr;
		//	});

		//this->components.erase(it, this->components.end());
	//}

		Entity& GetParent();
		template<typename T>
		T* ReplaceComponent(T* newComponent);

		// Check if the Entity haves the Component
		template<typename T>
		bool HasComponent() {
			std::string className = typeid(T).name();
			auto& components = *static_cast<ComponentMultiMap<uint64_t, Component>*>(GetAllComponentsMaps()[className]);
			if (components.find(this->uuid) != components.end())
				return true;
			return false;
		}

		void ChangeParent(Entity& oldParent, Entity& newParent) {
			this->parentUuid = newParent.uuid;
			auto oldParentIt = std::find(oldParent.childrenUuid.begin(), oldParent.childrenUuid.end(), this->uuid);
			auto newParentIt = std::find(newParent.childrenUuid.begin(), newParent.childrenUuid.end(), this->uuid);
			if (oldParentIt != oldParent.childrenUuid.end() && newParentIt == newParent.childrenUuid.end()) {
				oldParent.childrenUuid.erase(oldParentIt);
				newParent.childrenUuid.push_back(this->uuid);
			}
		}

		void ChangeParent(Entity* oldParent, Entity* newParent) {
			this->parentUuid = newParent->uuid;
			auto oldParentIt = std::find(oldParent->childrenUuid.begin(), oldParent->childrenUuid.end(), this->uuid);
			auto newParentIt = std::find(newParent->childrenUuid.begin(), newParent->childrenUuid.end(), this->uuid);
			if (oldParentIt != oldParent->childrenUuid.end() && newParentIt == newParent->childrenUuid.end()) {
				oldParent->childrenUuid.erase(oldParentIt);
				newParent->childrenUuid.push_back(this->uuid);
			}
		}

		void Rename(std::string newName);

		static uint64_t Instantiate(uint64_t uuid);

		template <class Archive>
		void serialize(Archive& archive) {
			archive(uuid, parentUuid, prefabUuid, equivalentPrefabUuid, childrenUuid, name);
		}

	private:
		Scene* mScene = nullptr;
	};

}
/*
#include <iostream>
#include <random>
#include <filesystem>
#include <typeinfo>
#include <list>
#include <string>
#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtc/matrix_transform.hpp"
#include "ThirdParty/glm/gtc/type_ptr.hpp"
#include "ThirdParty/uuid_v4/uuid_v4.h"
#include "Engine/Utils/glmUtils.h"
#include "Engine/Components/Component.h"
#include "Engine/Components/Core/Transform.h"
#include <random>
#include <unordered_map>
#include "Engine/Components/Rendering/Mesh.h"
//UUIDv4::UUID uuid = uuidGenerator.getUUID();
#include "Engine/Components/Component.h"

namespace Plaza {
	class Transform;
	class Entity;


	extern Entity* sceneObject;
	class Entity {
	public:
		std::vector<Entity*> children;
		Entity* parent = nullptr;
		Transform* transform = nullptr;// = new Transform();
		std::string name = "";
		int id;
		uint64_t uuid;
		uint64_t modelUuid = 0;
		std::string scene = "";

		void DeleteChildren(Entity* entity) {
			for (Entity* entity : entity->children) {
				DeleteChildren(entity);
			}

			delete(entity);
		}

		Entity(std::string objName, Entity* parent = sceneObject, bool addToScene = true);
		Entity(const Entity&) = default;
		~Entity() = default;
		void Delete();




		vector<shared_ptr<Component>> components;
		template<typename T>
		T* AddComponent(T* component, bool addToComponentsList = true);
		Component* AddComponent(Component* component, bool addToComponentsList = true);

		Component* cachedComponent = nullptr;
		template<typename T>
		T* GetComponent() {
			for (shared_ptr<Component> component : components) {
				if (typeid(*component) == typeid(T)) {
					return static_cast<T*>(component.get());
				}
			}
			return nullptr;
		}

		template<typename T>
		T* ReplaceComponent(T* oldComponent, T* newComponent) {
			for (size_t i = 0; i < components.size(); i++) {
				if (components[i].get() == oldComponent) {
					components.erase(components.begin() + i);
					newComponent->uuid = this->uuid;
					components.push_back(shared_ptr<Component>(newComponent));
					return newComponent;
				}
			}
			return nullptr; // Old component not found in the list
		}

		template <typename T>
		void RemoveComponent() {
			static_assert(std::is_base_of<Component, T>::value, "T must be a subclass of Component");

			auto it = std::remove_if(this->components.begin(), this->components.end(),
				[](const std::shared_ptr<Component>& comp) {
					return dynamic_cast<T*>(comp.get()) != nullptr;
				});

			this->components.erase(it, this->components.end());
		}
	};
}
//extern std::list<Entity*> gameObjects;

*/