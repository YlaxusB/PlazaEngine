#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Core/GameObject.h"
#include "Engine/Core/Scene.h"
#include "Engine/Components/Core/Transform.h"
#include "Engine/Components/Rendering/MeshRenderer.h"




namespace Engine {
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
		else {
			return Application->activeScene->transformComponents;
		}
	}

	Transform;
	GameObject::GameObject() {
		// Default constructor implementation
		uuid = Engine::UUID::NewUUID();
		this->AddComponent<Transform>(new Transform(), true);
		this->name = "";
		//Application->activeScene->entities.emplace(this->uuid, std::unique_ptr<GameObject>(this));
		Application->activeScene->entities.emplace(this->uuid, *this);
		Application->activeScene->mainSceneEntity->childrenUuid.push_back(this->uuid);
	}
	GameObject::GameObject(std::string objName, GameObject* parent, bool addToScene) {
		uuid = Engine::UUID::NewUUID();
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
		//Application->activeScene->gameObjects.push_back(std::unique_ptr<GameObject>(this));
		Application->activeScene->entities.emplace(this->uuid, *this);
	}

	template Transform* GameObject::GetComponent<Transform>(); // Replace 'Transform' with the actual type
	template Camera* GameObject::GetComponent<Camera>(); // Replace 'Transform' with the actual type
	template MeshRenderer* GameObject::GetComponent<MeshRenderer>(); // Replace 'MeshRenderer' with the actual type
	template RigidBody* GameObject::GetComponent<RigidBody>(); // Replace 'MeshRenderer' with the actual type
	template Collider* GameObject::GetComponent<Collider>(); // Replace 'MeshRenderer' with the actual type
	template<typename T>
	T* GameObject::GetComponent() {
		Component* component = nullptr;

		std::unordered_map<uint64_t, T>& components = GetComponentMap<T>();
		auto it = components.find(this->uuid);
		if (it != components.end()) {
			component = &(it->second);
		}

		return dynamic_cast<T*>(component);
	}

	template Transform* GameObject::AddComponent<Transform>(Transform* component, bool addToComponentsList); // Replace 'Transform' with the actual type
	template Camera* GameObject::AddComponent<Camera>(Camera* component, bool addToComponentsList); // Replace 'Transform' with the actual type
	template MeshRenderer* GameObject::AddComponent<MeshRenderer>(MeshRenderer* component, bool addToComponentsList); // Replace 'MeshRenderer' with the actual type
	template RigidBody* GameObject::AddComponent<RigidBody>(RigidBody* component, bool addToComponentsList); // Replace 'MeshRenderer' with the actual type
	template Collider* GameObject::AddComponent<Collider>(Collider* component, bool addToComponentsList); // Replace 'MeshRenderer' with the actual type
	template <typename T>
	T* GameObject::AddComponent(T* component, bool addToComponentsList) {
		component->uuid = this->uuid;

		if (addToComponentsList) {
			std::unordered_map<uint64_t, T>& components = GetComponentMap<T>();
			components.emplace(component->uuid, *component);
		}

		return component;
	}

	GameObject& GameObject::GetParent() {
		return Application->activeScene->entities[parentUuid];
	}

	template Transform* GameObject::ReplaceComponent<Transform>(Transform* newComponent); // Replace 'Transform' with the actual type
	template MeshRenderer* GameObject::ReplaceComponent<MeshRenderer>(MeshRenderer* newComponent);
	template <typename T>
	T* GameObject::ReplaceComponent(T* newComponent) {
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
	bool GameObject::HasComponent(T* component) {
		std::unordered_map<uint64_t, T>& components = GetComponentMap<T>();
		if (components.find(this->uuid) != components.end())
			return true;
		return false;
	}
}
/*
namespace Engine {
	template<typename T> T* GameObject::AddComponent(T* component, bool addToComponentsList) {
		components.push_back(shared_ptr<Component>(component));
		component->uuid = this->uuid;
		if (addToComponentsList) {
			if (typeid(component) == typeid(MeshRenderer*)) {
				Application->activeScene->meshRendererComponents.emplace(this->uuid, static_cast<MeshRenderer*>(components.back().get()));
			}
			if (typeid(component) == typeid(Transform*)) {
				Application->activeScene->transformComponents.emplace(this->uuid, static_cast<Transform*>(components.back().get()));
			}
		}
		return component;
	}


	template MeshRenderer* GameObject::AddComponent(MeshRenderer* component, bool addToComponentsList);
	template Transform* GameObject::AddComponent(Transform* component, bool addToComponentsList);

	Component* GameObject::AddComponent(Component* component, bool addToComponentsList) {
		components.push_back(shared_ptr<Component>(component));
		component->uuid = this->uuid;
		if (addToComponentsList) {
			if (typeid(component) == typeid(Transform*)) {
				Application->activeScene->transformComponents.emplace(this->uuid, static_cast<Transform*>(components.back().get()));
			}
		}
		return component;
	}

	GameObject::GameObject(std::string objName, GameObject* parent, bool addToScene) {
		uuid = Engine::UUID::NewUUID();
		this->transform = this->AddComponent<Transform>(new Transform(), addToScene);
		transform->uuid = this->uuid;
		//transform->gameObject = this;
		//this->AddComponent<Transform>(this->transform);
		UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
		name = objName;
		id = Application->activeScene->gameObjects.size() > 0 ? Application->activeScene->gameObjects.back()->id + 1 : 1; // IT WILL PROBABLY BREAK IN THE NEAR FUTURE

		// Set the new parent
		this->parent = parent;
		if (parent != nullptr) {
			parent->children.push_back(this);
		}	// Add to the gameobjects list
		if (addToScene)
			Application->activeScene->gameObjects.push_back(std::unique_ptr<GameObject>(this));
	}

	void GameObjectList::push_back(std::unique_ptr<GameObject> obj) {
		std::variant<uint64_t, string> variant; //= { obj.get()->uuid, obj.get()->name };
		variant = obj.get()->uuid;
		Application->activeScene->gameObjectsMap.emplace(variant, obj.get());
		//Application->activeScene->gameObjectsMap.emplace(obj.get()->name, obj.get());
		std::vector<std::unique_ptr<GameObject>>::push_back(std::move(obj));
	}
	GameObject* GameObjectList::find(std::string findName) {
		auto it = Application->activeScene->gameObjectsMap.find(findName);
		if (it != Application->activeScene->gameObjectsMap.end()) {
			GameObject* obj = it->second;
			return obj;
		}
		return nullptr;
	}

	GameObject* GameObjectList::find(uint64_t findUuid) {
		return Application->activeScene->gameObjectsMap[findUuid];
	}

	void GameObject::Delete() {
		uint64_t uuid = this->uuid;

		if (this->children.size() > 0) {
			for (GameObject* child : this->children) {
				child->parent = nullptr;
				child->Delete();
			}
		}
		if (this->parent) {
			auto it = std::find_if(this->parent->children.begin(), this->parent->children.end(), [uuid](GameObject* gameObject) {
				return gameObject->uuid == uuid;
				});

			if (it != this->parent->children.end()) {
				this->parent->children.erase(it);
			}

		}
		// Remove from the active scene game objects vector
		auto it = std::find_if(Application->activeScene->gameObjects.begin(), Application->activeScene->gameObjects.end(), [uuid](const std::unique_ptr<GameObject>& gameObject) {
			if (gameObject != nullptr)
				return gameObject->uuid == uuid;
			});

		if (it != Application->activeScene->gameObjects.end()) {
			Application->activeScene->gameObjectsMap.erase(uuid);
			Application->activeScene->gameObjects.erase(it);
			std::variant<uint64_t, std::string> uuidVariant = uuid;

		}

		//delete(this);
	}

	GameObjectList gameObjects;
	std::unordered_map<std::string, GameObject*> gameObjectsMap;

	std::vector<MeshRenderer*> meshRenderers;

	GameObject* sceneObject = new GameObject("Scene Objects", nullptr);
}
*/