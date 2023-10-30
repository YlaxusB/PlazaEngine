#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Core/Entity.h"
#include "Engine/Components/Core/Transform.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
#include "Engine/Components/Drawing/UI/TextRenderer.h"
#include "Engine/Core/Scene.h"



namespace Plaza {
	Entity::Entity() {
		// Default constructor implementation
		uuid = Plaza::UUID::NewUUID();
		this->AddComponent<Transform>(new Transform(), true);
		this->name = "Entity";
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
		// Set the new parent
		if (parent != nullptr && addToScene) {
			this->parentUuid = parent->uuid;
			Application->activeScene->entities[parentUuid].childrenUuid.push_back(this->uuid);
		}
		// Add to the gameobjects list
		if (addToScene) {
			Application->activeScene->entities.emplace(this->uuid, *this);
			Application->activeScene->entitiesNames[this->name].insert(this->uuid);
		}

	}

	std::unordered_map<std::string, void*> Entity::GetAllComponentsMaps() {
		return Application->activeScene->componentsMap;
	}

	Component* Entity::GetComponentByName(std::string className) {
		Component* component = nullptr;
		auto& components = *static_cast<ComponentMultiMap<uint64_t, Component>*>(Application->activeScene->componentsMap[className]);
		auto it = components.find(this->uuid);
		if (it != components.end()) {
			component = &(it->second);
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
			std::vector<uint64_t> children = this->childrenUuid;
			for (uint64_t child : children) {
				if (Application->activeScene->entities.find(child) != Application->activeScene->entities.end())
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
			if (this->HasComponent<AudioSource>())
				this->RemoveComponent<AudioSource>();
			if (this->HasComponent<AudioListener>())
				this->RemoveComponent<AudioListener>();

			if (Editor::selectedGameObject && Editor::selectedGameObject->uuid == this->uuid)
				Editor::selectedGameObject = nullptr;

			this->GetParent().childrenUuid.erase(std::remove(this->GetParent().childrenUuid.begin(), this->GetParent().childrenUuid.end(), this->uuid), this->GetParent().childrenUuid.end());
			if (Application->activeScene->entitiesNames.find(this->name) != Application->activeScene->entitiesNames.end())
				Application->activeScene->entitiesNames.erase(Application->activeScene->entitiesNames.find(this->name));
			Application->activeScene->entities.erase(Application->activeScene->entities.find(this->uuid));
			//Application->activeScene->entities.
			//Application->activeScene->entities.erase(Application->activeScene->entities.find(this->uuid));
		}
	}

	void Entity::Delete() {
		if (Application->runningScene) {
			std::vector<uint64_t> children = this->childrenUuid;
			for (uint64_t child : children) {
				if (Application->activeScene->entities.find(child) != Application->activeScene->entities.end())
					Application->activeScene->entities.at(child).Delete();
			}
			if (this->HasComponent<Transform>())
				this->RemoveComponent<Transform>();
			if (this->HasComponent<MeshRenderer>())
				this->RemoveComponent<MeshRenderer>();
			if (this->HasComponent<Collider>()) {
				this->GetComponent<Collider>()->RemoveActor();
				this->RemoveComponent<Collider>();
			}
			if (this->HasComponent<RigidBody>())
				this->RemoveComponent<RigidBody>();
			if (this->HasComponent<Camera>())
				this->RemoveComponent<Camera>();
			if (this->HasComponent<CsScriptComponent>()) {
				this->RemoveComponent<CsScriptComponent>();
				Application->activeScene->csScriptComponents.erase(this->uuid);
			}
			if (this->HasComponent<Plaza::Drawing::UI::TextRenderer>())
				this->RemoveComponent<Plaza::Drawing::UI::TextRenderer>();
			if (this->HasComponent<AudioSource>())
				this->RemoveComponent<AudioSource>();
			if (this->HasComponent<AudioListener>())
				this->RemoveComponent<AudioListener>();

			/*
						MeshRenderer* meshRendererToInstantiate = entityToInstantiate->GetComponent<MeshRenderer>();
			MeshRenderer* newMeshRenderer = new MeshRenderer();
			newMeshRenderer->uuid = instantiatedEntity->uuid;
			newMeshRenderer->instanced = true;
			newMeshRenderer->mesh = shared_ptr<Mesh>(meshRendererToInstantiate->mesh);
			instantiatedEntity->AddComponent<MeshRenderer>(newMeshRenderer);
			*/

			if (Editor::selectedGameObject && Editor::selectedGameObject->uuid == this->uuid)
				Editor::selectedGameObject = nullptr;

			this->GetParent().childrenUuid.erase(std::remove(this->GetParent().childrenUuid.begin(), this->GetParent().childrenUuid.end(), this->uuid), this->GetParent().childrenUuid.end());
			if (Application->activeScene->entitiesNames.find(this->name) != Application->activeScene->entitiesNames.end())
				Application->activeScene->entitiesNames.erase(Application->activeScene->entitiesNames.find(this->name));
			Application->activeScene->entities.erase(Application->activeScene->entities.find(this->uuid));
		}
		//this->~Entity();
	}

	template Transform* Entity::AddComp<Transform>();
	template Camera* Entity::AddComp<Camera>();
	template MeshRenderer* Entity::AddComp<MeshRenderer>();
	template RigidBody* Entity::AddComp<RigidBody>();
	template Collider* Entity::AddComp<Collider>();
	template CsScriptComponent* Entity::AddComp<CsScriptComponent>();
	template Plaza::Drawing::UI::TextRenderer* Entity::AddComp<Plaza::Drawing::UI::TextRenderer>();
	template AudioSource* Entity::AddComp<AudioSource>();
	template AudioListener* Entity::AddComp<AudioListener>();
	template<typename T>
	T* Entity::AddComp() {
		return new T();
	}

	void Entity::RemoveComponent(Component* component) {
		std::string className = typeid(*component).name();
		auto& components = *static_cast<ComponentMultiMap<uint64_t, Component>*>(Application->activeScene->componentsMap[className]);
		if (components.find(this->uuid) != components.end())
			components.erase(components.find(this->uuid));
	}
}