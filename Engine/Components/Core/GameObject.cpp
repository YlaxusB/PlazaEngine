#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Core/GameObject.h"
#include "Engine/Core/Scene.h"
#include "Engine/Components/Core/Transform.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
namespace Engine {
	GameObject::GameObject(std::string objName, GameObject* parent, bool addToScene) {
		uuid = Engine::UUID::NewUUID();
		this->AddComponent<Transform>(new Transform(), addToScene);
		name = objName;
		//id = Application->activeScene->gameObjects.size() > 0 ? Application->activeScene->gameObjects.back()->id + 1 : 1; // IT WILL PROBABLY BREAK IN THE NEAR FUTURE

		// Set the new parent
		this->parentUuid = parent->uuid;
		if (parent != nullptr) {
			Application->activeScene->entities[parentUuid].childrenUuid.push_back(this->uuid);
		}	// Add to the gameobjects list
		if (addToScene)
			Application->activeScene->entities.emplace(this->uuid, std::unique_ptr<GameObject>(this));
		//Application->activeScene->gameObjects.push_back(std::unique_ptr<GameObject>(this));
	}


	template<typename T>
	T* GetComponent() {
		if (typeid(T) == typeid(Engine::Transform*)) {
			return Application->activeScene->transformComponents[this->uuid];
		}
		else if (typeid(T) == typeid(MeshRenderer*)) {
			return Application->activeScene->meshRendererComponents[this->uuid];
		}
		return nullptr;
	}


	template<typename T>
	T* GameObject::AddComponent(T* component, bool addToScene) {
		component->uuid = this->uuid;
		if (typeid(component) == typeid(Tranform*)) {
			Application->activeScene->transformComponents.emplace({ component->uuid, component })
		}
		else if (typeid(component) == typeid(MeshRenderer*)) {
			Application->activeScene->meshRendererComponents.emplace({ component->uuid, component })
		}
		return nullptr;
	}

	GameObject& GameObject::GetParent() {
		return Application->activeScene->entities[parentUuid];
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