#pragma once

namespace Plaza {
	class PLAZA_API ECSManager {
	public:

	};

	struct PLAZA_API ComponentPool {
		ComponentPool(size_t elementsSize, int componentId) {
			mElementSize = elementsSize;
			mComponentId = componentId;
		}

		~ComponentPool() {
			delete[] mData;
		}

		inline void* Get(size_t index) {
			return mData + index * mElementSize;
		}

		char* mData{ nullptr };
		size_t mElementSize{ 0 };
		int mComponentId = 0;
	};

	class PLAZA_API ECS {
	public:
		class EntitySystem {
		public:
			static void Delete(Scene* scene, uint64_t uuid);
			static uint64_t Instantiate(Scene* scene, uint64_t uuidToInstantiate);
		};
		class TransformSystem {
		public:
			static void OnInstantiate(Component* componentToInstantiate, TransformComponent& transform);
			static const glm::vec3& GetWorldPosition(const TransformComponent& transform);
			static const glm::vec3& GetWorldRotation(const TransformComponent& transform);
			static const glm::vec3& GetWorldScale(const TransformComponent& transform);
			static void UpdateWorldMatrix(TransformComponent& transform, Scene* scene);
			static const glm::quat& GetLocalQuaternion(const TransformComponent& transform);
			static const glm::quat& GetWorldQuaternion(const TransformComponent& transform);
			static void UpdateLocalMatrix(TransformComponent& transform);
			static const glm::mat4& GetLocalMatrix(const TransformComponent& transform);
			static void UpdateObjectTransform(TransformComponent& transform);
			static void UpdateChildrenTransform(Entity* entity, Scene* scene);
			static void UpdateSelfAndChildrenTransform(TransformComponent& transform, Scene* scene);
			static void SetRelativePosition(TransformComponent& transform, const glm::vec3& vector);
			static void SetRelativeRotation(TransformComponent& transform, const glm::quat& quat);
			static void SetRelativeScale(TransformComponent& transform, const glm::vec3& vector, Scene* scene);

			// FIX: Move these functions to CppHelper
			// 
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


			// FIX: Figure out why it is here
			//static void UpdatePhysics() {
			//	PLAZA_PROFILE_SECTION("Transform: Update Physics");
			//	auto it = Scene::GetActiveScene()->colliderComponents.find(this->mUuid);
			//	if (it != Scene::GetActiveScene()->colliderComponents.end()) {
			//		it->second.UpdateShapeScale(this->GetWorldScale());
			//		it->second.UpdatePose(this);
			//	}
			//}
		};
	};

};
