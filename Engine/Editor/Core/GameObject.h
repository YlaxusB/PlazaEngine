#include <iostream>
#include <typeinfo>
#include <list>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include "game"
//#include "../../Application.h"
class Component {
public:
	virtual ~Component() {}  // virtual destructor is necessary for derived classes
};

class GameObject {
public:
	static std::list<GameObject*> gameObjects;


	GameObject(){
		gameObjects.push_back(this);
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



class Transform : public Component {
public:
	glm::vec3 position = { 0,0,0 };
	glm::vec3 eulerAngles = { 0,0,0 };
};

std::list<GameObject*> GameObject::gameObjects;