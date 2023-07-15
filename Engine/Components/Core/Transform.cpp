#pragma once
#include "Engine/Core/PreCompiledHeaders.h"



#include "Engine/Components/Core/Transform.h"
//#include "Engine/GUI/gizmo.h"
//#include "Engine/Components/Core/GameObject.h"

Transform::Transform() {};

/// <summary>
/// Returns a Matrix 4x4 
/// Order: WorldPosition - WorldRotation - Scale
/// </summary>
/// <param name="position"></param>
/// <param name="vec3"></param>
/// <returns></returns>
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
/// <summary>
/// Rotates around parent, then positionates its relative position based on the rotation and returns this position.
/// </summary>
glm::vec3 newWorldPosition(GameObject* gameObject) {
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	rotationMatrix *= glm::toMat4(glm::quat(gameObject->parent->transform->worldRotation));
	rotationMatrix = glm::scale(rotationMatrix, gameObject->parent->transform->worldScale);
	glm::vec3 transformedPoint = glm::vec3(rotationMatrix * glm::vec4(gameObject->transform->relativePosition, 1.0f));
	glm::vec3 finalWorldPoint = transformedPoint + gameObject->parent->transform->worldPosition;
	return finalWorldPoint;
}
/// <summary>
/// First rotates with parent rotation and then its own rotation.
/// </summary>
glm::vec3 newWorldRotation(GameObject* gameObject) {
	glm::mat4 newRotationMatrix = glm::translate(glm::mat4(1.0f), gameObject->transform->worldPosition)
		* glm::toMat4(glm::quat(gameObject->parent->transform->worldRotation))
		* glm::toMat4(glm::quat(gameObject->transform->rotation))
		* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

	glm::vec3 eulerAngles = glm::eulerAngles(glm::quat_cast(newRotationMatrix));
	return eulerAngles;

}



void UpdateObjectTransform(GameObject* gameObject) {
	gameObject->transform->worldScale = gameObject->transform->scale * gameObject->parent->transform->worldScale;
	gameObject->transform->worldRotation = newWorldRotation(gameObject);
	gameObject->transform->worldPosition = newWorldPosition(gameObject);
	gameObject->transform->modelMatrix = gameObject->transform->GetTransform();
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
