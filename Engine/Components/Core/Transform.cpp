#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/euler_angles.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


#include "Engine/Components/Core/Transform.h"
//#include "Engine/GUI/gizmo.h"
//#include "Engine/Components/Core/GameObject.h"

Transform::Transform() {};

// Get transform
glm::mat4 Transform::GetTransform(glm::vec3 position, glm::vec3 scale)
{	
	if (this->gameObject->parent == nullptr) {
		return glm::mat4(0.0f);
	}
	glm::mat4 rot = glm::toMat4(glm::quat(rotation));

	glm::mat4 gizmoMatrix = glm::translate(glm::mat4(1.0f), this->worldPosition)
		* glm::toMat4(glm::quat(gameObject->transform->worldRotation))
		* glm::scale(glm::mat4(1.0f), scale);

	//gizmoMatrix = glm::scale(gizmoMatrix, gameObject->transform->worldScale);
	return gizmoMatrix;
}

glm::mat4 Transform::GetTransform() {
	return GetTransform(this->worldPosition, this->worldScale);
}

glm::mat4 Transform::GetTransform(glm::vec3 position) {
	return GetTransform(position, this->worldScale);
}

glm::vec3 TransformToWorldSpace(const glm::vec3& localPosition,
	const glm::mat4& transformMatrix)
{
	glm::vec4 homogeneousPos(localPosition.x, localPosition.y, localPosition.z, 1.0f);
	glm::vec4 worldPos = transformMatrix * homogeneousPos;

	return glm::vec3(worldPos);
}

glm::vec3 TransformToLocalSpace(const glm::vec3& worldPosition,
	const glm::mat4& transformMatrix)
{
	glm::mat4 inverseMatrix = glm::inverse(transformMatrix);
	glm::vec4 homogeneousPos(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f);
	glm::vec4 localPos = inverseMatrix * homogeneousPos;

	return glm::vec3(localPos);
}

glm::vec3 test(GameObject* child) {
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	rotationMatrix *= glm::toMat4(glm::quat(child->parent->transform->worldRotation));
	rotationMatrix = glm::scale(rotationMatrix, child->parent->transform->worldScale);
	//rotationMatrix = glm::scale(rotationMatrix, child->parent->transform->worldScale);
	glm::vec3 transformedPoint = glm::vec3(rotationMatrix * glm::vec4(child->transform->relativePosition, 1.0f));
	glm::vec3 finalWorldPoint = transformedPoint + child->parent->transform->worldPosition;
	//finalWorldPoint = (finalWorldPoint * child->parent->transform->worldScale);
	//finalWorldPoint = finalWorldPoint * child->parent->transform->worldScale;
	return finalWorldPoint;
}

glm::vec3 newWorldRotation(GameObject* gameObject) {
	glm::mat4 gizmoMatrix = glm::translate(glm::mat4(1.0f), gameObject->transform->worldPosition)
		* glm::toMat4(glm::quat(gameObject->parent->transform->worldRotation))
		* glm::toMat4(glm::quat(gameObject->transform->rotation))
		* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

	glm::vec3 a = glm::eulerAngles(glm::quat_cast(gizmoMatrix));
	return a;

	//return gameObject->transform->rotation + gameObject->parent->transform->worldRotation;
}

void UpdateObjectTransform(GameObject* gameObject) {
	gameObject->transform->worldScale = gameObject->transform->scale * gameObject->parent->transform->worldScale;
	//gameObject->transform->worldPosition = (gameObject->transform->relativePosition * gameObject->parent->transform->worldScale + gameObject->parent->transform->worldPosition);
	gameObject->transform->worldRotation = newWorldRotation(gameObject);//gameObject->transform->rotation + gameObject->parent->transform->worldRotation;
	gameObject->transform->worldPosition = test(gameObject);
//	gameObject->transform->worldPosition = (gameObject->transform->worldPosition * gameObject->parent->transform->worldScale) + gameObject->parent->transform->worldPosition;
}


void Transform::UpdateChildrenTransform(GameObject* gameObject) {
	if (gameObject->parent != nullptr) {
		UpdateObjectTransform(gameObject);
	}

	for (GameObject* child : gameObject->children) {
		UpdateObjectTransform(child);

		UpdateChildrenTransform(child);
	}

}



void Transform::UpdateChildrenTransform() {
	if (gameObject != nullptr) {
		UpdateChildrenTransform(gameObject);
	}
}


void Transform::UpdateChildrenScale(GameObject* gameObject) {

}

void Transform::UpdateChildrenScale() {
	if (gameObject != nullptr)
		UpdateChildrenScale(gameObject);
}
