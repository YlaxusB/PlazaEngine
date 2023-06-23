#pragma  once

#include <iostream>
#include <random>
#include <filesystem>
#include <typeinfo>
#include <list>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

class GameObjectList : public std::vector<GameObject*> {
public:
	void push_back(GameObject* obj);
	GameObject* find(std::string findName);
};

extern GameObjectList gameObjects;

extern std::unordered_map<std::string, GameObject*> gameObjectsMap;
extern GameObject* sceneObject;

class GameObject {
public:
	std::vector<GameObject*> children;
	GameObject* parent = nullptr;
	Transform* transform = nullptr;// = new Transform();
	std::string name = "";
	int id;

	GameObject(std::string objName, GameObject* parent = sceneObject);

	std::list<Component*> components;
	template<typename T>
	T* AddComponent(T* component) {
		components.push_back(component);
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
};

//extern std::list<GameObject*> gameObjects;


class MeshRenderer : public Component {
public:
	Engine::Mesh mesh;
	MeshRenderer(const Engine::Mesh& initialMesh) : mesh(initialMesh) {
	}
};
