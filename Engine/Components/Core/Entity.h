#pragma  once
#include <variant>
#include "Engine/Components/Component.h"
#include <vector>
#include <string>
namespace Plaza {
	template <typename T>
	auto& GetComponentMap();
	class Entity {
	public:
		uint64_t uuid;
		uint64_t parentUuid;
		std::vector<uint64_t> childrenUuid;
		std::string name;
		bool changingName = false;

		Entity();
		Entity(std::string objName, Entity* parent = nullptr, bool addToScene = true, uint64_t newUuid = 0);
		Entity(const Entity&) = default;
		~Entity();
		void Delete();

		template<typename T>
		T* GetComponent();

		template<typename T>
		T* AddComponent(T* component, bool addToComponentsList = true);

		//Component* AddComponent(Component* component, bool addToComponentsList = true);

		template <typename T>
		void RemoveComponent(); //{
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
		bool HasComponent();

		void ChangeParent(Entity& oldParent, Entity& newParent) {
			this->parentUuid = newParent.uuid;
			auto oldParentIt = std::find(oldParent.childrenUuid.begin(), oldParent.childrenUuid.end(), this->uuid);
			auto newParentIt = std::find(newParent.childrenUuid.begin(), newParent.childrenUuid.end(), this->uuid);
			if (oldParentIt != oldParent.childrenUuid.end() && newParentIt == newParent.childrenUuid.end()) {
				oldParent.childrenUuid.erase(oldParentIt);
				newParent.childrenUuid.push_back(this->uuid);
			}
		}

		void Rename(std::string newName);
	};

}
/*
#include <iostream>
#include <random>
#include <filesystem>
#include <typeinfo>
#include <list>
#include <string>
#include "Engine/Vendor/glm/glm.hpp"
#include "Engine/Vendor/glm/gtc/matrix_transform.hpp"
#include "Engine/Vendor/glm/gtc/type_ptr.hpp"
#include "Engine/Vendor/uuid_v4/uuid_v4.h"
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