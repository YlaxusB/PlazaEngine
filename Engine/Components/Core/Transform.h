#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Engine/Components/Core/GameObject.h"
#include "Engine/Components/Core/Component.h"

class GameObject;

class Transform : public Component {
public:
	GameObject* gameObject;
	glm::vec3 position = { 0,0,0 };
	glm::vec3 worldPosition = { 0, 0, 0 };
	glm::vec3 relativePosition = { 0, 0, 0 };
	glm::vec3 rotation = { 0.0f,0.0f,0.0f };
	glm::vec3 worldRotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1,1,1, };
	glm::vec3 worldScale = {1, 1, 1};

	Transform();

	glm::mat4 GetTransform(glm::vec3 pos);

	void UpdateChildrenTransform(GameObject* gameObject);
	void UpdateChildrenTransform();
	void UpdateChildrenScale(GameObject* gameObject);
	void UpdateChildrenScale();
};