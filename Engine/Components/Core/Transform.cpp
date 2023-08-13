#pragma once
#include "Engine/Core/PreCompiledHeaders.h"



#include "Engine/Components/Core/Transform.h"
//#include "Editor/GUI/gizmo.h"
//#include "Engine/Components/Core/GameObject.h"
namespace Engine {
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
		if (Application->activeScene->entities[this->uuid].parentUuid == 0) {
			return glm::mat4(0.0f);
		}
		glm::mat4 rot = glm::toMat4(glm::quat(rotation));

		glm::mat4 gizmoMatrix = glm::translate(glm::mat4(1.0f), this->worldPosition)
			* glm::toMat4(glm::quat(Application->activeScene->entities[this->uuid].GetComponent<Transform>()->worldRotation))
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
		rotationMatrix *= glm::toMat4(glm::quat(Application->activeScene->entities[gameObject->parentUuid].GetComponent<Transform>()->worldRotation));
		rotationMatrix = glm::scale(rotationMatrix, Application->activeScene->entities[gameObject->parentUuid].GetComponent<Transform>()->worldScale);
		glm::vec3 transformedPoint = glm::vec3(rotationMatrix * glm::vec4(gameObject->GetComponent<Transform>()->relativePosition, 1.0f));
		glm::vec3 finalWorldPoint = transformedPoint + Application->activeScene->entities[gameObject->parentUuid].GetComponent<Transform>()->worldPosition;
		return finalWorldPoint;
	}
	/// <summary>
	/// First rotates with parent rotation and then its own rotation.
	/// </summary>
	glm::vec3 newWorldRotation(GameObject* gameObject) {
		glm::mat4 newRotationMatrix = glm::translate(glm::mat4(1.0f), gameObject->GetComponent<Transform>()->worldPosition)
			* glm::toMat4(glm::quat(Application->activeScene->entities[gameObject->parentUuid].GetComponent<Transform>()->worldRotation))
			* glm::toMat4(glm::quat(gameObject->GetComponent<Transform>()->rotation))
			* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

		glm::vec3 eulerAngles = glm::eulerAngles(glm::quat_cast(newRotationMatrix));
		return eulerAngles;

	}



	void Transform::UpdateObjectTransform(GameObject* gameObject) {
		if (gameObject->parentUuid) {
			Transform& transform = *gameObject->GetComponent<Transform>();
			transform.worldScale = gameObject->GetComponent<Transform>()->scale * Application->activeScene->entities[gameObject->parentUuid].GetComponent<Transform>()->worldScale;
			transform.worldRotation = newWorldRotation(gameObject);
			transform.worldPosition = newWorldPosition(gameObject);
			transform.modelMatrix = gameObject->GetComponent<Transform>()->GetTransform();
		}
	}


	void Transform::UpdateChildrenTransform(GameObject* gameObject) {
		if (Application->activeScene->entities[gameObject->parentUuid].uuid) {
			UpdateObjectTransform(gameObject);
		}

		for (uint64_t child : gameObject->childrenUuid) {
			UpdateObjectTransform(&Application->activeScene->entities[child]);

			UpdateChildrenTransform(&Application->activeScene->entities[child]);
		}

	}



	void Transform::UpdateChildrenTransform() {
		if (uuid) {
			UpdateChildrenTransform(&Application->activeScene->entities[uuid]);
		}
	}
}