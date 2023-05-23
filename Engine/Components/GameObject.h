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
#include "Mesh.h"
#include "Engine/Vendor/uuid_v4/uuid_v4.h"

//UUIDv4::UUID uuid = uuidGenerator.getUUID();
class Component {
public:
	virtual ~Component() {}  // virtual destructor is necessary for derived classes
};
class GameObject;
extern std::list<GameObject*> gameObjects;

class GameObject {
public:
	std::string name = "";
	std::string id = "";
	GameObject(std::string objName) {
		UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
		name = objName;
		gameObjects.push_back(this);
		id = uuidGenerator.getUUID().str();
	}
	std::list<Component*> components;

	void  AddComponent(Component* component) {
		components.push_back(component);
	}
	template<typename T>

	T* GetComponent() {
		for (Component* component : components) {
			if (typeid(*component) == typeid(T)) {
				return static_cast<T*>(component);
			}
		}
	}
};

//extern std::list<GameObject*> gameObjects;


class Transform : public Component {
public:
	glm::vec3 position = { 0,0,0 };
	glm::vec3 rotation = { 0,0,0 };
	glm::vec3 scale = { 1,1,1, };

	glm::mat4 GetTransform() const
	{
		glm::mat4 rotation = glm::mat4(glm::quat(rotation));

		return glm::translate(glm::mat4(1.0f), position)
			* rotation
			* glm::scale(glm::mat4(1.0f), scale);
	}
};

class MeshRenderer : public Component {
public:
	Mesh mesh;
	MeshRenderer(const Mesh& initialMesh) : mesh(initialMesh) {

	}
};

#endif