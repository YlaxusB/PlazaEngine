#pragma once
#include <functional>

namespace Plaza {
	typedef uint64_t EntityId;
	class PLAZA_API ECSManager {
	public:

	};

	//template<typename T>
	//class PLAZA_API SparseSet {
	//public:
	//	void Add(T* component, EntityId id) {
	//		mDenseArray.push_back(component);
	//		mSparseMap[id] = 
	//	}
	//
	//private:
	//	std::map<EntityId, T> mSparseMap;
	//	std::vector<T> mDenseArray;
	//};

#define MAX_ENTITIES 65536*4
	struct PLAZA_API ComponentPool {
		ComponentPool(size_t elementsSize, size_t componentMask) {
			mElementSize = elementsSize;
			mComponentMask = componentMask;
			mData = new char[mElementSize * MAX_ENTITIES];
		}

		ComponentPool(const ComponentPool& other) {
			mElementSize = other.mElementSize;
			mComponentMask = other.mComponentMask;
			mSize = other.mSize;
			mData = new char[mElementSize * MAX_ENTITIES];
			std::memcpy(mData, other.mData, mElementSize * MAX_ENTITIES);
			mInstantiateFactory = other.mInstantiateFactory;
			mSparseMap = other.mSparseMap;
		}

		~ComponentPool() {
			delete[] mData;
		}

		inline void* Get(size_t index) {
			return mData + mSparseMap[index] * mElementSize;
		}

		template<typename T>
		inline T* New(size_t index) {
			mSparseMap[index] = mSize;
			T* component = new (Get(index)) T();
			mSize = std::max(mSize, index + 1);
			return component;
		}

		template<typename T>
		T* Add(size_t index, T* component = nullptr) {
			void* storage = Get(index);

			if (component == nullptr) {
				component = new (storage) T();
			}
			else {
				*reinterpret_cast<T**>(storage) = component;
			}
			mSparseMap[index] = mSize;
			mSize = std::max(mSize, index + 1);

			return component;
		}

		std::map<EntityId, EntityId> mSparseMap;
		char* mData{ nullptr };
		size_t mSize = 0;
		size_t mElementSize{ 0 };
		size_t mComponentMask;

		std::function<void* (ComponentPool* pool, void*, uint64_t)> mInstantiateFactory;
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
		};
		class ColliderSystem {
		public:
			static void RemoveActor(Collider* collider);
			static void Init(RigidBody* rigidBody);
			static void InitDynamic(RigidBody* rigidBody = nullptr) {};
			static void InitStatic() {};
			static void InitCollider(Collider* collider, TransformComponent* transform, RigidBody* rigidBody);
			static void InitCollider(Scene* scene, uint64_t uuid);
			static void RemoveCollider();
			static void Update() {};

			static void CreateShape(Collider* collider, TransformComponent* transform, ColliderShape::ColliderShapeEnum shapeEnum, Mesh* mesh = nullptr);
			static void AddShape(ColliderShape* shape);
			static void AddConvexMeshShape(Mesh* mesh);
			static void AddMeshShape(Mesh* mesh);
			static void AddHeightShape(float** heightData, int size);
			static void UpdateShapeScale(Scene* scene, Collider* collider, const glm::vec3& scale);
			static void UpdateAllShapesScale(Scene* scene, uint64_t uuid);

			static void UpdatePose(Collider* collider, TransformComponent* transform);
		};

		class RigidBodySystem {
		public:
			static void Init(Scene* scene, uint64_t uuid);
			static void Update(Scene* scene, uint64_t uuid);
			static void UpdateGlobalPose(Scene* scene, uint64_t uuid);
			static void AddCollidersOfChildren(Scene* scene, uint64_t parent);
		};
	};

};
