#include "Engine/Core/PreCompiledHeaders.h"
#include "ECSManager.h"

namespace Plaza {
	void ECS::TransformSystem::OnInstantiate(Component* componentToInstantiate, TransformComponent& transform) {
		TransformComponent* component = static_cast<TransformComponent*>(componentToInstantiate);
		transform.SetRelativePosition(component->relativePosition);
		transform.SetRelativeRotation(glm::eulerAngles(component->rotation));
		transform.scale = component->scale;
	}

	const glm::vec3& ECS::TransformSystem::GetWorldPosition(const TransformComponent& transform) {
		return transform.modelMatrix[3];
	}

	const glm::vec3& ECS::TransformSystem::GetWorldRotation(const TransformComponent& transform) {
		glm::vec3 scale;
		glm::vec3 translation;
		glm::quat rotation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transform.modelMatrix, scale, rotation, translation, skew, perspective);
		return glm::eulerAngles(rotation);
	}

	const glm::vec3& ECS::TransformSystem::GetWorldScale(const TransformComponent& transform) {
		glm::vec3 scale;
		scale.x = glm::length(transform.modelMatrix[0]);
		scale.y = glm::length(transform.modelMatrix[1]);
		scale.z = glm::length(transform.modelMatrix[2]);
		return scale;
	}

	void ECS::TransformSystem::UpdateWorldMatrix(TransformComponent& transform, Scene* scene) {
		uint64_t parentUuid = Scene::GetActiveScene()->entities.find(transform.mUuid)->second.parentUuid;
		if (parentUuid && scene->HasComponent<TransformComponent>(parentUuid)) {
			glm::mat4 parentModelMatrix = scene->GetComponent<TransformComponent>(parentUuid)->modelMatrix;
			transform.modelMatrix = parentModelMatrix * transform.localMatrix;
		}
		else
			transform.modelMatrix = glm::mat4(1.0f) * transform.localMatrix;
	}

	const glm::quat& ECS::TransformSystem::GetLocalQuaternion(const TransformComponent& transform) {
		return glm::normalize(glm::quat_cast(transform.localMatrix)); // radians
	}

	/// <summary>
	/// Returns the Quaternion of the Transform World Rotation in radians
	/// </summary>
	/// <returns></returns>
	const glm::quat& ECS::TransformSystem::GetWorldQuaternion(const TransformComponent& transform) {
		return glm::normalize(glm::quat_cast(transform.modelMatrix));
	}

	void ECS::TransformSystem::UpdateLocalMatrix(TransformComponent& transform) {
		transform.localMatrix = glm::translate(glm::mat4(1.0f), transform.relativePosition)
			* glm::toMat4(transform.rotation)
			* glm::scale(glm::mat4(1.0f), transform.scale);
	}

	const glm::mat4& ECS::TransformSystem::GetLocalMatrix(const TransformComponent& transform) {
		return transform.localMatrix;
	}

	void ECS::TransformSystem::UpdateObjectTransform(TransformComponent& transform) {
		transform.UpdateWorldMatrix();
		//transform.UpdatePhysics();
	}


	void ECS::TransformSystem::UpdateChildrenTransform(Entity* entity, Scene* scene) {
		UpdateObjectTransform(*scene->GetComponent<TransformComponent>(entity->uuid));

		for (uint64_t child : entity->childrenUuid) {
			UpdateObjectTransform(*scene->GetComponent<TransformComponent>(child));

			UpdateChildrenTransform(scene->GetEntity(child), scene);
		}
	}

	void ECS::TransformSystem::UpdateSelfAndChildrenTransform(TransformComponent& transform, Scene* scene) {
		UpdateLocalMatrix(transform);
		UpdateWorldMatrix(transform, scene);
		// FIX: Update physics
		//if (scene->mRunning)
		//	transform.UpdatePhysics();
		for (uint64_t child : scene->GetEntity(transform.mUuid)->childrenUuid) {
			UpdateSelfAndChildrenTransform(*scene->GetComponent<TransformComponent>(child), scene);
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

	void ECS::TransformSystem::SetRelativePosition(TransformComponent& transform, const glm::vec3& vector) {
		transform.relativePosition = vector;
		transform.UpdateSelfAndChildrenTransform();
	}

	void ECS::TransformSystem::SetRelativeRotation(TransformComponent& transform, const glm::quat& quat) {
		transform.rotation = quat;
		transform.UpdateSelfAndChildrenTransform();
	}

	void ECS::TransformSystem::SetRelativeScale(TransformComponent& transform, const glm::vec3& vector, Scene* scene) {
		transform.scale = vector;
		transform.UpdateSelfAndChildrenTransform();
		if (scene->HasComponent<Collider>(transform.mUuid)) {
			scene->GetComponent<Collider>(transform.mUuid)->UpdateShapeScale(transform.worldScale);
		}
	}
}