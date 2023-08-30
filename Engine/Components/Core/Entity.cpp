#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Core/Entity.h"
#include "Engine/Components/Core/Transform.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
#include "Engine/Core/Scene.h"



namespace Plaza {
	template <typename T>
	std::unordered_map<uint64_t, T>& GetComponentMap() {
		if constexpr (std::is_same_v<T, Transform>) {
			return Application->activeScene->transformComponents;
		}
		else if constexpr (std::is_same_v<T, MeshRenderer>) {
			return Application->activeScene->meshRendererComponents;
		}
		else if constexpr (std::is_same_v<T, RigidBody>) {
			return Application->activeScene->rigidBodyComponents;
		}
		else if constexpr (std::is_same_v<T, Collider>) {
			return Application->activeScene->colliderComponents;
		}
		else if constexpr (std::is_same_v<T, Camera>) {
			return Application->activeScene->cameraComponents;
		}
		else if constexpr (std::is_same_v<T, CppScriptComponent>) {
			return Application->activeScene->cppScriptComponents;
		}
		else {
			return Application->activeScene->transformComponents;
		}
	}

	Transform;
	Entity::Entity() {
		// Default constructor implementation
		uuid = Plaza::UUID::NewUUID();
		this->AddComponent<Transform>(new Transform(), true);
		this->name = "";
		//Application->activeScene->entities.emplace(this->uuid, std::unique_ptr<Entity>(this));
		Application->activeScene->entities.emplace(this->uuid, *this);
		Application->activeScene->mainSceneEntity->childrenUuid.push_back(this->uuid);
	}
	Entity::Entity(std::string objName, Entity* parent, bool addToScene) {
		uuid = Plaza::UUID::NewUUID();
		this->AddComponent<Transform>(new Transform(), addToScene);
		name = objName;
		//id = Application->activeScene->gameObjects.size() > 0 ? Application->activeScene->gameObjects.back()->id + 1 : 1; // IT WILL PROBABLY BREAK IN THE NEAR FUTURE

		// Set the new parent
		if (parent != nullptr) {
			this->parentUuid = parent->uuid;
			Application->activeScene->entities[parentUuid].childrenUuid.push_back(this->uuid);
		}
		// Add to the gameobjects list
		//if (addToScene)
			//Application->activeScene->entities.emplace(this->uuid, this);
		//Application->activeScene->gameObjects.push_back(std::unique_ptr<Entity>(this));
		Application->activeScene->entities.emplace(this->uuid, *this);
	}

	template Transform* Entity::GetComponent<Transform>(); // Replace 'Transform' with the actual type
	template Camera* Entity::GetComponent<Camera>(); // Replace 'Transform' with the actual type
	template MeshRenderer* Entity::GetComponent<MeshRenderer>(); // Replace 'MeshRenderer' with the actual type
	template RigidBody* Entity::GetComponent<RigidBody>(); // Replace 'MeshRenderer' with the actual type
	template Collider* Entity::GetComponent<Collider>(); // Replace 'MeshRenderer' with the actual type
	template CppScriptComponent* Entity::GetComponent<CppScriptComponent>(); // Replace 'MeshRenderer' with the actual type
	template<typename T>
	T* Entity::GetComponent() {
		Component* component = nullptr;

		std::unordered_map<uint64_t, T>& components = GetComponentMap<T>();
		auto it = components.find(this->uuid);
		if (it != components.end()) {
			component = &(it->second);
		}

		return dynamic_cast<T*>(component);
	}

	template Transform* Entity::AddComponent<Transform>(Transform* component, bool addToComponentsList); // Replace 'Transform' with the actual type
	template Camera* Entity::AddComponent<Camera>(Camera* component, bool addToComponentsList); // Replace 'Transform' with the actual type
	template MeshRenderer* Entity::AddComponent<MeshRenderer>(MeshRenderer* component, bool addToComponentsList); // Replace 'MeshRenderer' with the actual type
	template RigidBody* Entity::AddComponent<RigidBody>(RigidBody* component, bool addToComponentsList); // Replace 'MeshRenderer' with the actual type
	template Collider* Entity::AddComponent<Collider>(Collider* component, bool addToComponentsList); // Replace 'MeshRenderer' with the actual type
	template CppScriptComponent* Entity::AddComponent<CppScriptComponent>(CppScriptComponent* component, bool addToComponentsList); // Replace 'MeshRenderer' with the actual type
	template <typename T>
	T* Entity::AddComponent(T* component, bool addToComponentsList) {
		component->uuid = this->uuid;

		if (addToComponentsList) {
			std::unordered_map<uint64_t, T>& components = GetComponentMap<T>();
			components.emplace(component->uuid, *component);
		}

		return component;
	}

	Entity& Entity::GetParent() {
		if (!parentUuid)
			return Application->activeScene->entities.at(Application->activeScene->mainSceneEntity->uuid);
		auto parentIt = Application->activeScene->entities.find(parentUuid);
		if (parentIt != Application->activeScene->entities.end())
			return parentIt->second;
	}

	template Transform* Entity::ReplaceComponent<Transform>(Transform* newComponent); // Replace 'Transform' with the actual type
	template MeshRenderer* Entity::ReplaceComponent<MeshRenderer>(MeshRenderer* newComponent);
	template <typename T>
	T* Entity::ReplaceComponent(T* newComponent) {
		T* existingComponent = GetComponent<T>();

		if (existingComponent) {
			// Copy over the UUID from the existing component to the new one
			newComponent->uuid = existingComponent->uuid;

			// Replace the component in the appropriate components list
			if constexpr (std::is_same_v<T, Transform>) {
				Application->activeScene->transformComponents[newComponent->uuid] = *newComponent;
			}
			else if constexpr (std::is_same_v<T, MeshRenderer>) {
				Application->activeScene->meshRendererComponents[newComponent->uuid] = *newComponent;
			}

			// Clean up and delete the old component
			if constexpr (std::is_base_of_v<Component, T>) {
				if (existingComponent->uuid == this->uuid) {
					existingComponent->uuid = UUID::NewUUID();
				}

				if constexpr (std::is_base_of_v<Transform, T> || std::is_base_of_v<MeshRenderer, T>) {
					if (existingComponent->uuid != newComponent->uuid) {
						if constexpr (std::is_base_of_v<Transform, T>) {
							Application->activeScene->transformComponents.erase(existingComponent->uuid);
						}
						else if constexpr (std::is_base_of_v<MeshRenderer, T>) {
							Application->activeScene->meshRendererComponents.erase(existingComponent->uuid);
						}
					}
				}
			}

			//delete existingComponent;
			return newComponent;
		}

		return nullptr; // Component to replace not found
	}

	template <typename T>
	bool Entity::HasComponent(T* component) {
		std::unordered_map<uint64_t, T>& components = GetComponentMap<T>();
		if (components.find(this->uuid) != components.end())
			return true;
		return false;
	}

	template void Entity::RemoveComponent<Transform>(); // Replace 'Transform' with the actual type
	template void Entity::RemoveComponent<Camera>(); // Replace 'Transform' with the actual type
	template void Entity::RemoveComponent<MeshRenderer>(); // Replace 'MeshRenderer' with the actual type
	template void Entity::RemoveComponent<RigidBody>(); // Replace 'MeshRenderer' with the actual type
	template void Entity::RemoveComponent<Collider>(); // Replace 'MeshRenderer' with the actual type
	template void Entity::RemoveComponent<CppScriptComponent>(); // Replace 'MeshRenderer' with the actual type
	template <typename T>
	void Entity::RemoveComponent() {
		std::unordered_map<uint64_t, T>& components = GetComponentMap<T>();
		if (components.find(this->uuid) != components.end())
			components.erase(components.find(this->uuid));
	}
}