#pragma once
#include "Engine/Core/PreCompiledHeaders.h"



#include "Engine/Components/Core/Transform.h"
#include "Editor/GUI/gizmo.h"
//#include "Editor/GUI/gizmo.h"
//#include "Engine/Components/Core/Entity.h"
namespace Plaza {
	Transform::Transform() {};

	glm::vec3 Transform::GetWorldPosition() {
		return this->modelMatrix[3];
	}

	glm::vec3 Transform::GetWorldRotation() {
		glm::vec3 scale;
		glm::vec3 translation;
		glm::quat rotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(this->modelMatrix, scale, rotation, translation, skew, perspective);
		return glm::eulerAngles(rotation);
	}

	glm::vec3 Transform::GetWorldScale() {
		glm::vec3 scale;

		scale.x = glm::length(modelMatrix[0]);
		scale.y = glm::length(modelMatrix[1]);
		scale.z = glm::length(modelMatrix[2]);
		return scale;
	}

	/// <summary>
	/// Returns a Matrix 4x4 
	/// Order: WorldPosition - WorldRotation - Scale
	/// </summary>
	/// <param name="position"></param>
	/// <param name="vec3"></param>
	/// <returns></returns>
	glm::mat4 Transform::GetTransform(glm::vec3 position, glm::vec3 scale)
	{
		glm::mat4 parentMatrix;
		if (Application->activeScene->entities.at(this->uuid).parentUuid == 0) {
			parentMatrix = glm::mat4(1.0f);
		}
		else {
			parentMatrix = this->GetGameObject()->GetParent().GetComponent<Transform>()->modelMatrix;
		}
		this->modelMatrix = parentMatrix * this->GetLocalMatrix();
		return this->modelMatrix;
	}

	glm::mat4 Transform::GetTransform() {
		return GetTransform(this->worldPosition, this->worldScale);
	}
	glm::mat4 Transform::GetTransform(glm::vec3 position) {
		return GetTransform(position, this->worldScale);
	}

	void Transform::UpdateWorldMatrix() {
		PLAZA_PROFILE_SECTION("Transform: Update World Matrix");
		uint64_t parentUuid = Application->activeScene->entities.find(this->uuid)->second.parentUuid;
		if (parentUuid && Application->activeScene->transformComponents.find(parentUuid) != Application->activeScene->transformComponents.end()) {
			glm::mat4 parentModelMatrix = Application->activeScene->transformComponents.find(parentUuid)->second.modelMatrix;
			if (this->lastParentModelMatrix != parentModelMatrix || this->lastLocalMatrix != this->localMatrix) {
				this->modelMatrix = parentModelMatrix * this->localMatrix;
				this->lastParentModelMatrix = parentModelMatrix;
				this->lastLocalMatrix = this->localMatrix;
			}
		}
		else
			this->modelMatrix = glm::mat4(1.0f) * this->localMatrix;
	}

	/// <summary>
	/// Returns the Quaternion of the Transform Local Rotation in radians
	/// </summary>
	/// <returns></returns>
	glm::quat Transform::GetLocalQuaternion() {
		return glm::normalize(glm::quat_cast(this->localMatrix));
	}

	/// <summary>
	/// Returns the Quaternion of the Transform World Rotation in radians
	/// </summary>
	/// <returns></returns>
	glm::quat Transform::GetWorldQuaternion() {
		return glm::quat(glm::mat3(this->modelMatrix));
		//return glm::normalize(glm::quat_cast(this->modelMatrix));
	}

	void Transform::UpdateLocalMatrix() {
		PLAZA_PROFILE_SECTION("Transform: Update Local Matrix");
		if (this->relativePosition != this->lastRelativePositionLocalMatrix || this->rotation != this->lastRotationLocalMatrix || this->scale != this->lastScaleLocalMatrix) {
			this->localMatrix = glm::translate(glm::mat4(1.0f), this->relativePosition)
				* glm::toMat4(rotation)
				* glm::scale(glm::mat4(1.0f), scale);
			this->lastRelativePositionLocalMatrix = this->relativePosition;
			this->lastRotationLocalMatrix = this->rotation;
			this->lastScaleLocalMatrix = this->scale;
			//return this->localMatrix;
		}
		//return this->localMatrix;
	}

	glm::mat4 Transform::GetLocalMatrix() {
		return this->localMatrix;
	}


	/// <summary>
	/// Rotates around parent, then positionates its relative position based on the rotation and returns this position.
	/// </summary>
	glm::vec3 newWorldPosition(Entity* entity) {
		glm::mat4 rotationMatrix = glm::mat4(1.0f);
		rotationMatrix *= glm::toMat4(glm::quat(Application->activeScene->entities[entity->parentUuid].GetComponent<Transform>()->worldRotation));
		rotationMatrix = glm::scale(rotationMatrix, Application->activeScene->entities[entity->parentUuid].GetComponent<Transform>()->worldScale);
		glm::vec3 transformedPoint = glm::vec3(rotationMatrix * glm::vec4(entity->GetComponent<Transform>()->relativePosition, 1.0f));
		glm::vec3 finalWorldPoint = transformedPoint + Application->activeScene->entities[entity->parentUuid].GetComponent<Transform>()->worldPosition;
		return finalWorldPoint;
	}
	/// <summary>
	/// First rotates with parent rotation and then its own rotation.
	/// </summary>
	glm::vec3 newWorldRotation(Entity* entity) {
		glm::mat4 newRotationMatrix = glm::translate(glm::mat4(1.0f), entity->GetComponent<Transform>()->worldPosition)
			* glm::toMat4(glm::quat(Application->activeScene->entities[entity->parentUuid].GetComponent<Transform>()->worldRotation))
			* glm::toMat4(glm::quat(entity->GetComponent<Transform>()->rotation))
			* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

		glm::vec3 eulerAngles = glm::eulerAngles(glm::quat_cast(newRotationMatrix));
		return eulerAngles;

	}



	void Transform::UpdateObjectTransform(Entity* entity) {
		Transform& transform = *entity->GetComponent<Transform>();
		transform.UpdateWorldMatrix();
		//transform.UpdatePhysics();
	}


	void Transform::UpdateChildrenTransform(Entity* entity) {
		UpdateObjectTransform(entity);

		for (uint64_t child : entity->childrenUuid) {
			UpdateObjectTransform(&Application->activeScene->entities[child]);

			UpdateChildrenTransform(&Application->activeScene->entities[child]);
		}
	}

	void Transform::UpdateSelfAndChildrenTransform() {
		PLAZA_PROFILE_SECTION("Transform: Update Self And Children Transform");
		this->UpdateLocalMatrix();
		this->UpdateWorldMatrix();
		if (Application->runningScene)
			this->UpdatePhysics();
		for (uint64_t child : this->GetGameObject()->childrenUuid) {
			Application->activeScene->transformComponents.at(child).UpdateSelfAndChildrenTransform();
		}
		if (haveCamera) {
			Application->activeCamera->Update();
		}
	}

	void Transform::UpdateChildrenTransform() {
		if (uuid) {
			UpdateChildrenTransform(&Application->activeScene->entities[uuid]);
		}
	}

	void Transform::MoveTowards(glm::vec3 vector) {
		glm::mat4 matrix = this->GetTransform();
		glm::vec3 currentPosition = glm::vec3(matrix[3]);
		// Extract the forward, left, and up vectors from the matrix
		glm::vec3 forwardVector = glm::normalize(glm::vec3(matrix[2]));
		glm::vec3 leftVector = glm::normalize(glm::cross(glm::vec3(matrix[1]), forwardVector));
		glm::vec3 upVector = glm::normalize(glm::vec3(matrix[1]));
		this->relativePosition += forwardVector * vector.x + leftVector * vector.z + upVector * vector.y;
		this->UpdateSelfAndChildrenTransform();
	}

	// Set Functions
	void Transform::SetRelativePosition(glm::vec3 vector) {
		PLAZA_PROFILE_SECTION("Transform: Set Relative Position");
		this->relativePosition = vector;
		this->UpdateSelfAndChildrenTransform();
	}

	void Transform::SetRelativeRotation(glm::quat quat) {
		this->rotation = quat;
		this->UpdateSelfAndChildrenTransform();
	}

	void Transform::SetRelativeScale(glm::vec3 vector) {
		this->scale = vector;
		this->UpdateSelfAndChildrenTransform();
		auto it = Application->activeScene->colliderComponents.find(this->uuid);
		if (it != Application->activeScene->colliderComponents.end()) {
			it->second.UpdateShapeScale(this->worldScale);
		}
	}

	void Transform::UpdatePhysics() {
		PLAZA_PROFILE_SECTION("Transform: Update Physics");
		auto it = Application->activeScene->colliderComponents.find(this->uuid);
		if (it != Application->activeScene->colliderComponents.end()) {
			it->second.UpdatePose(this);
			it->second.UpdateShapeScale(this->GetWorldScale());
		}

		auto characterControllerIt = Application->activeScene->characterControllerComponents.find(this->uuid);
		if (characterControllerIt != Application->activeScene->characterControllerComponents.end() && !characterControllerIt->second.hasMovedInThisFrame) {
			characterControllerIt->second.mCharacterController->setPosition(physx::PxExtendedVec3(this->GetWorldPosition().x, this->GetWorldPosition().y, this->GetWorldPosition().z));
		}
	}

	void Transform::Rotate(glm::vec3 vector) {
		glm::vec3 position, rotation, scale;

		// Convert the rotation component from PxQuat to glm::quat
		glm::mat4 mat4 = glm::translate(glm::mat4(1.0f), this->relativePosition)
			* glm::toMat4(glm::quat(rotation))
			* glm::scale(glm::mat4(1.0f), scale);
		glm::quat glmRotation = glm::normalize(glm::quat_cast(mat4));

		// Create the glm::mat4 matrix
		glm::mat4 gizmoTransform = glm::mat4_cast(glmRotation); // Initialize the matrix with rotation
		gizmoTransform = glm::translate(gizmoTransform, this->relativePosition); // Apply translation

		Editor::Gizmo::DecomposeTransform(gizmoTransform, position, rotation, scale);

		// --- Rotation
		glm::mat4 updatedTransform = glm::inverse(this->GetGameObject()->GetParent().GetComponent<Transform>()->GetTransform()) * glm::toMat4(glm::quat(rotation));

		glm::vec3 updatedPosition, updatedRotation, updatedScale;
		Editor::Gizmo::DecomposeTransform(updatedTransform, updatedPosition, updatedRotation, updatedScale); // The rotation is radians

		//this->rotation += glm::vec3(0.0f, 0.2f, 0.0f);

		this->UpdateLocalMatrix();
		this->UpdateWorldMatrix();
		//this->UpdateSelfAndChildrenTransform();
	}
}