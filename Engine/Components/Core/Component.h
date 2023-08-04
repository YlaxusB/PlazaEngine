#pragma once
#include "Engine/Components/Core/GameObject.h"
class GameObject;
class Component {
public:
	uint64_t gameObjectUUID;
	GameObject* gameObject;
	virtual ~Component() = default;  // virtual destructor is necessary for derived classes
};