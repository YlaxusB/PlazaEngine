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

	GameObject(std::string objName, GameObject* parent = sceneObject);
	GameObject(const GameObject&) = default;
	~GameObject() {
		for (Component* component : components) {
			delete(component);
		}
	};
	std::vector<Component*> components;
	template<typename T>
	T* AddComponent(T* component) {
		components.push_back(component);
		component->gameObject = this;
		return component;
	}

	template<typename T>
	T* GetComponent() {
		for (Component* component : components) {
			if (typeid(*component) == typeid(T)) {
				return static_cast<T*>(component);
			}
		}
		return nullptr;
	}

	template<typename T>
	T* ReplaceComponent(T* oldComponent, T* newComponent) {
		for (size_t i = 0; i < components.size(); i++) {
			if (components[i] == oldComponent) {
				components.erase(components.begin() + i);
				newComponent->gameObject = this;
				components.push_back(newComponent);
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
	std::unique_ptr<Engine::Mesh> mesh;
	Transform* transform;
	MeshRenderer(Mesh initialMesh);
	MeshRenderer(const MeshRenderer&) = default;
	~MeshRenderer() = default;
};
