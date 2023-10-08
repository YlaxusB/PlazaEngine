#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Core/Entity.h"
#include "Engine/Components/Core/Transform.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
#include "Engine/Components/Drawing/UI/TextRenderer.h"
#include "Engine/Core/Scene.h"



namespace Plaza {
	template <typename T>
	auto& GetComponentMap() {
		if constexpr (std::is_same_v<T, Transform>) {
			return Application->activeScene->transformComponents;
		}
		else if constexpr (std::is_same_v<T, Camera>) {
			return Application->activeScene->cameraComponents;
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
		else if constexpr (std::is_same_v<T, CsScriptComponent>) {
			return Application->activeScene->csScriptComponents;
		}
		else if constexpr (std::is_same_v<T, Plaza::Drawing::UI::TextRenderer>) {
			return Application->activeScene->UITextRendererComponents;
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
		this->name = "Entity";
		//Application->activeScene->entities.emplace(this->uuid, std::unique_ptr<Entity>(this));
		Application->activeScene->entities.emplace(this->uuid, *this);
		Application->activeScene->mainSceneEntity->childrenUuid.push_back(this->uuid);
		Application->activeScene->entitiesNames[this->name].insert(this->uuid);
	}
	Entity::Entity(std::string objName, Entity* parent, bool addToScene, uint64_t newUuid) {
		if (newUuid)
			uuid = newUuid;
		else
			uuid = Plaza::UUID::NewUUID();

		this->AddComponent<Transform>(new Transform(), addToScene);
		name = objName;
		//id = Application->activeScene->gameObjects.size() > 0 ? Application->activeScene->gameObjects.back()->id + 1 : 1; // IT WILL PROBABLY BREAK IN THE NEAR FUTURE

		// Set the new parent
		if (parent != nullptr && addToScene) {
			this->parentUuid = parent->uuid;
			Application->activeScene->entities[parentUuid].childrenUuid.push_back(this->uuid);
		}
		// Add to the gameobjects list
		//if (addToScene)
			//Application->activeScene->entities.emplace(this->uuid, this);
		//Application->activeScene->gameObjects.push_back(std::unique_ptr<Entity>(this));
		if (addToScene) {
			Application->activeScene->entities.emplace(this->uuid, *this);
			Application->activeScene->entitiesNames[this->name].insert(this->uuid);
		}

	}

	template Transform* Entity::GetComponent<Transform>();
	template Camera* Entity::GetComponent<Camera>();
	template MeshRenderer* Entity::GetComponent<MeshRenderer>();
	template RigidBody* Entity::GetComponent<RigidBody>();
	template Collider* Entity::GetComponent<Collider>();
	template CsScriptComponent* Entity::GetComponent<CsScriptComponent>();
	template Plaza::Drawing::UI::TextRenderer* Entity::GetComponent<Plaza::Drawing::UI::TextRenderer>();
	template<typename T>
	T* Entity::GetComponent() {
		Component* component = nullptr;

		auto& components = GetComponentMap<T>();
		auto it = components.find(this->uuid);
		if (it != components.end()) {
			component = &(it->second);
		}
		else if (std::is_same_v<T, CsScriptComponent>) {
			std::unordered_multimap<uint64_t, CsScriptComponent>& components = Application->activeScene->csScriptComponents;
			auto it = components.find(this->uuid);
			if (it != components.end()) {
				component = &(it->second);
			}
		}

		return dynamic_cast<T*>(component);
	}

	template Transform* Entity::AddComponent<Transform>(Transform* component, bool addToComponentsList);
	template Camera* Entity::AddComponent<Camera>(Camera* component, bool addToComponentsList);
	template MeshRenderer* Entity::AddComponent<MeshRenderer>(MeshRenderer* component, bool addToComponentsList);
	template RigidBody* Entity::AddComponent<RigidBody>(RigidBody* component, bool addToComponentsList);
	template Collider* Entity::AddComponent<Collider>(Collider* component, bool addToComponentsList);
	template CsScriptComponent* Entity::AddComponent<CsScriptComponent>(CsScriptComponent* component, bool addToComponentsList);
	template Plaza::Drawing::UI::TextRenderer* Entity::AddComponent<Plaza::Drawing::UI::TextRenderer>(Plaza::Drawing::UI::TextRenderer* component, bool addToComponentsList);

	template <typename T>
	T* Entity::AddComponent(T* component, bool addToComponentsList) {
		component->uuid = this->uuid;

		if (addToComponentsList) {
			auto& components = GetComponentMap<T>();
			components.emplace(component->uuid, *component);
		}

		return dynamic_cast<T*>(component);
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

	template bool Entity::HasComponent<Transform>();
	template bool Entity::HasComponent<Camera>();
	template bool Entity::HasComponent<MeshRenderer>();
	template bool Entity::HasComponent<RigidBody>();
	template bool Entity::HasComponent<Collider>();
	template bool Entity::HasComponent<CsScriptComponent>();
	template bool Entity::HasComponent<Plaza::Drawing::UI::TextRenderer>();
	template<typename T>
	bool Entity::HasComponent() {
		auto& components = GetComponentMap<T>();
		if (components.find(this->uuid) != components.end())
			return true;
		return false;
	}

	template void Entity::RemoveComponent<Transform>();
	template void Entity::RemoveComponent<Camera>();
	template void Entity::RemoveComponent<MeshRenderer>();
	template void Entity::RemoveComponent<RigidBody>();
	template void Entity::RemoveComponent<Collider>();
	template void Entity::RemoveComponent<CsScriptComponent>();
	template void Entity::RemoveComponent<Plaza::Drawing::UI::TextRenderer>();
	template <typename T>
	void Entity::RemoveComponent() {
		auto& components = GetComponentMap<T>();
		if (components.find(this->uuid) != components.end())
			components.erase(components.find(this->uuid));
	}

	void Entity::Rename(std::string newName) {
		// Erase the old name
		Application->activeScene->entitiesNames.at(this->name).erase(Application->activeScene->entitiesNames.at(this->name).find(this->uuid));
		if (Application->activeScene->entitiesNames.at(this->name).size() <= 0) {
			Application->activeScene->entitiesNames.erase(Application->activeScene->entitiesNames.find(this->name));
		}

		// Emplace the new name
		Application->activeScene->entitiesNames[newName].insert(this->uuid);
		this->name = newName;
	}

	Entity::~Entity() {
		if (!Application->runningScene) {
			for (uint64_t child : this->childrenUuid) {
				Application->activeScene->entities.at(child).~Entity();
			}
			if (this->HasComponent<Transform>())
				this->RemoveComponent<Transform>();
			if (this->HasComponent<MeshRenderer>())
				this->RemoveComponent<MeshRenderer>();
			if (this->HasComponent<Collider>())
				this->RemoveComponent<Collider>();
			if (this->HasComponent<RigidBody>())
				this->RemoveComponent<RigidBody>();
			if (this->HasComponent<Camera>())
				this->RemoveComponent<Camera>();
			if (this->HasComponent<CsScriptComponent>())
				this->RemoveComponent<CsScriptComponent>();
			if (this->HasComponent<Plaza::Drawing::UI::TextRenderer>())
				this->RemoveComponent<Plaza::Drawing::UI::TextRenderer>();
			this->GetParent().childrenUuid.erase(std::remove(this->GetParent().childrenUuid.begin(), this->GetParent().childrenUuid.end(), this->uuid), this->GetParent().childrenUuid.end());
			if (Application->activeScene->entitiesNames.find(this->name) != Application->activeScene->entitiesNames.end())
				Application->activeScene->entitiesNames.erase(Application->activeScene->entitiesNames.find(this->name));
			//Application->activeScene->entities.
			//Application->activeScene->entities.erase(Application->activeScene->entities.find(this->uuid));
		}
	}

	void Entity::Delete() {
		if (Application->runningScene) {
			for (uint64_t child : this->childrenUuid) {
				Application->activeScene->entities.at(child).~Entity();
			}
			if (this->HasComponent<Transform>())
				this->RemoveComponent<Transform>();
			if (this->HasComponent<MeshRenderer>())
				this->RemoveComponent<MeshRenderer>();
			if (this->HasComponent<Collider>())
				this->RemoveComponent<Collider>();
			if (this->HasComponent<RigidBody>())
				this->RemoveComponent<RigidBody>();
			if (this->HasComponent<Camera>())
				this->RemoveComponent<Camera>();
			if (this->HasComponent<CsScriptComponent>())
				this->RemoveComponent<CsScriptComponent>();
			if (this->HasComponent<Plaza::Drawing::UI::TextRenderer>())
				this->RemoveComponent<Plaza::Drawing::UI::TextRenderer>();

			if (Application->activeScene->entitiesNames.find(this->name) != Application->activeScene->entitiesNames.end())
				Application->activeScene->entitiesNames.erase(Application->activeScene->entitiesNames.find(this->name));
			this->GetParent().childrenUuid.erase(std::remove(this->GetParent().childrenUuid.begin(), this->GetParent().childrenUuid.end(), this->uuid), this->GetParent().childrenUuid.end());
		}
		this->~Entity();
	}


	template Transform* Entity::AddComp<Transform>();
	template Camera* Entity::AddComp<Camera>();
	template MeshRenderer* Entity::AddComp<MeshRenderer>();
	template RigidBody* Entity::AddComp<RigidBody>();
	template Collider* Entity::AddComp<Collider>();
	template CsScriptComponent* Entity::AddComp<CsScriptComponent>();
	template Plaza::Drawing::UI::TextRenderer* Entity::AddComp<Plaza::Drawing::UI::TextRenderer>();
	template<typename T>
	T* Entity::AddComp() {
		return new T();
	}
}