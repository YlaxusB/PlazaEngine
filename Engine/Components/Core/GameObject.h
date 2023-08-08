#pragma  once

#include <iostream>
#include <random>
#include <filesystem>
#include <typeinfo>
#include <list>
#include <string>
#include "Engine/Vendor/glm/glm.hpp"
#include "Engine/Vendor/glm/gtc/matrix_transform.hpp"
#include "Engine/Vendor/glm/gtc/type_ptr.hpp"
#include "Engine/Components/Core/Mesh.h"
#include "Engine/Vendor/uuid_v4/uuid_v4.h"
#include "Engine/Utils/glmUtils.h"
#include "Engine/Components/Core/Component.h"
#include "Engine/Components/Core/Transform.h"
#include <random>
#include <unordered_map>
//UUIDv4::UUID uuid = uuidGenerator.getUUID();
class Transform;
class GameObject;


extern GameObject* sceneObject;
class GameObject {
public:
	std::vector<GameObject*> children;
	GameObject* parent = nullptr;
	Transform* transform = nullptr;// = new Transform();
	std::string name = "";
	int id;
	uint64_t uuid;
	uint64_t modelUuid = 0;
	std::string scene = "";

	void DeleteChildren(GameObject* gameObject) {
		for (GameObject* gameObject : gameObject->children) {
			DeleteChildren(gameObject);
		}

		delete(gameObject);
	}

	GameObject(std::string objName, GameObject* parent = sceneObject, bool addToScene = true);
	GameObject(const GameObject&) = default;
	~GameObject() = default;
	void Delete();




	vector<shared_ptr<Component>> components;
	template<typename T>
	T* AddComponent(T* component);
	Component* AddComponent(Component* component);

	Component* cachedComponent = nullptr;
	template<typename T>
	T* GetComponent() {
		const std::type_info& type = typeid(T);
		if (cachedComponent && type == typeid(*cachedComponent))
			return static_cast<T*>(cachedComponent);
		for (const std::shared_ptr<Component>& component : components) {
			if (typeid(*component) == type) {
				cachedComponent = component.get();
				return static_cast<T*>(cachedComponent);
			}
		}
		return nullptr;
	}

	template<typename T>
	T* ReplaceComponent(T* oldComponent, T* newComponent) {
		for (size_t i = 0; i < components.size(); i++) {
			if (components[i].get() == oldComponent) {
				components.erase(components.begin() + i);
				newComponent->gameObject = this;
				components.push_back(shared_ptr<Component>(newComponent));
				return newComponent;
			}
		}
		return nullptr; // Old component not found in the list
	}
};

//extern std::list<GameObject*> gameObjects;

class MeshRenderer;
class MeshRenderer : public Component {
public:
	string aiMeshName;
	uint64_t uuid;
	std::string meshName;
	std::shared_ptr<Engine::Mesh> mesh;
	Transform* transform;
	bool instanced = false;
	MeshRenderer(Mesh initialMesh, bool addToScene = false);
	MeshRenderer(const MeshRenderer&) = default;
	~MeshRenderer();
	MeshRenderer() {
		uuid = Engine::UUID::NewUUID();
	}
};