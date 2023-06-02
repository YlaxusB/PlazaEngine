#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H


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

class GameObjectList : public std::list<GameObject*> {
public:
	void push_back(GameObject* obj);
	GameObject* find(std::string findName);
};

extern GameObjectList gameObjects;

extern std::unordered_map<std::string, GameObject*> gameObjectsMap;
extern GameObject* sceneObject;

class GameObject {
public:
	std::list<GameObject*> children;
	GameObject* parent = nullptr;
	Transform* transform = nullptr;// = new Transform();
	std::string name = "";
	int id;


	GameObject(std::string objName, GameObject* parent = sceneObject) {
		this->transform = this->AddComponent<Transform>(new Transform());
		//transform->gameObject = this;
		//this->AddComponent<Transform>(this->transform);
		UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
		name = objName;
		id = gameObjects.size() > 0 ? gameObjects.back()->id + 1 : 1; // IT WILL PROBABLY BREAK IN THE NEAR FUTURE

		// Set the new parent
		this->parent = parent;
		if (parent != nullptr) {
			parent->children.push_back(this);
		}
		// Add to the gameobjects list
		gameObjects.push_back(this);
	}
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
	Mesh mesh;
	MeshRenderer(const Mesh& initialMesh) : mesh(initialMesh) {
	}
};

#endif