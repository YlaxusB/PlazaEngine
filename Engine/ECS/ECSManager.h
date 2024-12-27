#pragma once

namespace Plaza {
	class PLAZA_API ECSManager {
	public:

	};
#define MAX_ENTITIES 65536*4
	struct PLAZA_API ComponentPool {
		ComponentPool(size_t elementsSize, size_t componentMask) {
			mElementSize = elementsSize;
			mComponentMask = componentMask;
			mData = new char[mElementSize * MAX_ENTITIES];
		}

		~ComponentPool() {
			delete[] mData;
		}

		inline void* Get(size_t index) {
			return mData + index * mElementSize;
		}

		template<typename T>
		inline T* Add(size_t index) {
			T* component = new (Get(index)) T();
			mSize = std::max(mSize, index + 1);
			return component;
		}

		char* mData{ nullptr };
		size_t mSize = 0;
		size_t mElementSize{ 0 };
		size_t mComponentMask;
	};

	class PLAZA_API ECS {
	public:
		class EntitySystem {
		public:
			static void SetParent(Scene* scene, Entity* child, Entity* newParent);
			static void Delete(Scene* scene, uint64_t uuid);
			static uint64_t Instantiate(Scene* scene, uint64_t uuidToInstantiate);
		};
		class TransformSystem {
		public:
			static void OnInstantiate(Component* componentToInstantiate, TransformComponent& transform);
			static void UpdateTransform(TransformComponent& transform, Scene* scene);
			static void UpdateLocalMatrix(TransformComponent& transform, Scene* scene);
			static void UpdateWorldMatrix(TransformComponent& transform, const glm::mat4& parentWorldMatrix, Scene* scene);
			static void UpdateChildrenTransform(TransformComponent& transform, Scene* scene);
			static void SetLocalPosition(TransformComponent& transform, Scene* scene, const glm::vec3& vector, bool updateWorldMatrix = true);
			static void SetLocalRotation(TransformComponent& transform, Scene* scene, const glm::quat& quat, bool updateWorldMatrix = true);
			static void SetLocalScale(TransformComponent& transform, Scene* scene, const glm::vec3& vector, bool updateWorldMatrix = true);
			static void SetWorldPosition(TransformComponent& transform, Scene* scene, const glm::vec3& vector, bool updateWorldMatrix = true);
			static void SetWorldRotation(TransformComponent& transform, Scene* scene, const glm::vec3& vector, bool updateWorldMatrix = true);
			static void SetWorldScale(TransformComponent& transform, Scene* scene, const glm::vec3& vector, bool updateWorldMatrix = true);
			static void UpdateSelfAndChildrenTransform(TransformComponent& transform, TransformComponent* parentTransform, Scene* scene, bool updateLocal = true);

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
