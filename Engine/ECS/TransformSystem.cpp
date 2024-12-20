#include "Engine/Core/PreCompiledHeaders.h"
#include "ECSManager.h"

namespace Plaza {
	void Transform::OnInstantiate(Component* componentToInstantiate, TransformComponent& transform) {
		TransformComponent* component = static_cast<TransformComponent*>(componentToInstantiate);
		transform.SetRelativePosition(component->relativePosition);
		transform.SetRelativeRotation(glm::eulerAngles(component->rotation));
		transform.scale = component->scale;
	}

	const Transform::glm::vec3& GetWorldPosition(const TransformComponent& transform) {
		return transform.modelMatrix[3];
	}

	const Transform::glm::vec3& GetWorldRotation(const TransformComponent& transform) {
		glm::vec3 scale;
		glm::vec3 translation;
		glm::quat rotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transform.modelMatrix, scale, rotation, translation, skew, perspective);
		return glm::eulerAngles(rotation);
	}

	const Transform::glm::vec3& GetWorldScale(const TransformComponent& transform) {
		glm::vec3 scale;
		scale.x = glm::length(transform.modelMatrix[0]);
		scale.y = glm::length(transform.modelMatrix[1]);
		scale.z = glm::length(transform.modelMatrix[2]);
		return scale;
	}

	void Transform::UpdateWorldMatrix(TransformComponent& transform, Scene* scene) {
		uint64_t parentUuid = Scene::GetActiveScene()->entities.find(transform.mUuid)->second.parentUuid;
		if (parentUuid && Scene::GetActiveScene()->transformComponents.find(parentUuid) != Scene::GetActiveScene()->transformComponents.end()) {
			glm::mat4 parentModelMatrix = scene->GetComponent<Transform>(parentUuid)->modelMatrix;
			if (transform.lastParentModelMatrix != parentModelMatrix || transform.lastLocalMatrix != transform.localMatrix) {
				transform.modelMatrix = parentModelMatrix * transform.localMatrix;
				transform.lastParentModelMatrix = parentModelMatrix;
				transform.lastLocalMatrix = transform.localMatrix;
			}
		}
		else
			transform.modelMatrix = glm::mat4(1.0f) * transform.localMatrix;
	}

	const Transform::glm::quat& GetLocalQuaternion(const TransformComponent& transform) {
		return glm::normalize(glm::quat_cast(transform.localMatrix)); // radians
	}

	/// <summary>
	/// Returns the Quaternion of the Transform World Rotation in radians
	/// </summary>
	/// <returns></returns>
	const Transform::glm::quat& GetWorldQuaternion(const TransformComponent& transform) {
		return glm::normalize(glm::quat_cast(transform.modelMatrix));
	}

	void Transform::UpdateLocalMatrix(const TransformComponent& transform) {
		transform.localMatrix = glm::translate(glm::mat4(1.0f), transform.relativePosition)
			* glm::toMat4(rotation)
			* glm::scale(glm::mat4(1.0f), scale);
	}

	const Transform::glm::mat4& GetLocalMatrix(const TransformComponent& transform) {
		return transform.localMatrix;
	}

	void Transform::UpdateObjectTransform(const TransformComponent& transform) {
		transform.UpdateWorldMatrix();
		//transform.UpdatePhysics();
	}


	void Transform::UpdateChildrenTransform(Entity* entity, Scene* scene) {
		UpdateObjectTransform(entity);

		for (uint64_t child : entity->childrenUuid) {
			UpdateObjectTransform(*scene->GetComponent<TransformComponent>(child));

			UpdateChildrenTransform(scene->GetEntity(child), scene);
		}
	}

	void Transform::UpdateSelfAndChildrenTransform(const TransformComponent& transform, Scene* scene) {
		UpdateLocalMatrix(transform);
		UpdateWorldMatrix(transform, scene);
		if (scene->runningScene)
			transform.UpdatePhysics();
		for (uint64_t child : scene->GetEntity(transform.mUuid)->childrenUuid) {
			UpdateSelfAndChildrenTransform(*scene->GetComponent<Transform>(child), scene);
		}
	}

	void Transform::UpdateChildrenTransform(const TransformComponent& transform, Scene* scene) {
		if (mUuid) {
			UpdateChildrenTransform(scene->GetEntity(transform.mUuid), scene);
		}
	}

	// FIX: Move these functions to CppHelper
	//void TransformComponent::MoveTowards(glm::vec3 vector) {
	//	glm::mat4 matrix = this->GetTransform();
	//	glm::vec3 currentPosition = glm::vec3(matrix[3]);
	//	glm::vec3 forwardVector = glm::normalize(glm::vec3(matrix[2]));
	//	glm::vec3 leftVector = glm::normalize(glm::cross(glm::vec3(matrix[1]), forwardVector));
	//	glm::vec3 upVector = glm::normalize(glm::vec3(matrix[1]));
	//	this->relativePosition += leftVector * vector.x + forwardVector * vector.z + upVector * vector.y;
	//	this->UpdateSelfAndChildrenTransform();
	//}
	//
	//const glm::vec3& TransformComponent::MoveTowardsReturn(glm::vec3 vector) {
	//	glm::mat4 matrix = this->GetTransform();
	//	glm::vec3 currentPosition = glm::vec3(matrix[3]);
	//	glm::vec3 forwardVector = glm::normalize(glm::vec3(matrix[2]));
	//	glm::vec3 leftVector = glm::normalize(glm::cross(glm::vec3(matrix[1]), forwardVector));
	//	glm::vec3 upVector = glm::normalize(glm::vec3(matrix[1]));
	//	glm::vec3 outVector = leftVector * vector.x + forwardVector * vector.z + upVector * vector.y;
	//	return outVector;
	//}

	void Transform::SetRelativePosition(TransformComponent& transform, const glm::vec3& vector) {
		transform.relativePosition = vector;
		transform.UpdateSelfAndChildrenTransform();
	}

	void Transform::SetRelativeRotation(TransformComponent& transform, const glm::quat& quat) {
		transform.rotation = quat;
		transform.UpdateSelfAndChildrenTransform();
	}

	void Transform::SetRelativeScale(TransformComponent& transform, const glm::vec3& vector, Scene* scene) {
		transform.scale = vector;
		transform.UpdateSelfAndChildrenTransform();
		if (scene->HasComponent<Collider>(transform.mUuid)) {
			scene->GetComponent<Collider>()->UpdateShapeScale(transform.worldScale);
		}
	}
}