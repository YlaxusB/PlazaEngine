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
		ComponentPool() {}
		ComponentPool(size_t elementsSize, size_t componentMask, const std::string& rawComponentName) {
			mElementSize = elementsSize;
			mComponentMask = componentMask;
			mData.push_back(nullptr);
			//mData.resize(MAX_ENTITIES); //= new char[mElementSize * MAX_ENTITIES];
			hash<std::string> hasher;
			mComponentRawNameHash = hasher(rawComponentName);
		}

		ComponentPool(const ComponentPool& other) {
			mElementSize = other.mElementSize;
			mComponentMask = other.mComponentMask;
			mSize = other.mSize;
			mData.resize(other.mData.size());
			for (size_t i = 0; i < other.mData.size(); ++i) {
				if (other.mData[i]) {
					mData[i] = std::make_shared<Component>(*other.mData[i]);
				}
				else {
					mData[i] = nullptr;
				}
			}
			mInstantiateFactory = other.mInstantiateFactory;
			mSparseMap = other.mSparseMap;
		}

		~ComponentPool() {
			mData.clear();
		}

		inline Component* Get(size_t index) {
			return mData[mSparseMap[index]].get();//mData + mSparseMap[index] * mElementSize;
		}

		template<typename T>
		inline T* New(size_t index) {
			mSize++;
			mSparseMap[index] = mSize;
			std::shared_ptr<T> component = std::make_shared<T>();
			mData.push_back(component);
			return component.get();
		}

		template<typename T>
		T* Add(size_t index, T* component = nullptr) {
			void* storage = Get(index);

			if (component == nullptr)
				component = new T();
			mData.push_back(std::make_shared<T>(component));

			mSparseMap[index] = mSize;
			mSize = std::max(mSize, index + 1);

			return component;
		}

		void Allocate(size_t capacity) {
			mCapacity = capacity;
			//mData = new char[mCapacity];
		}

		std::map<EntityId, EntityId> mSparseMap;
		std::vector<std::shared_ptr<Component>> mData;
		size_t mSize = 0;
		size_t mCapacity = MAX_ENTITIES;
		size_t mElementSize{ 0 };
		size_t mComponentMask;
		size_t mComponentRawNameHash;

		std::function<void* (ComponentPool* pool, void*, uint64_t)> mInstantiateFactory;

		template <typename Archive>
		void serialize(Archive& archive) {
			archive(PL_SER(mSize), PL_SER(mCapacity), PL_SER(mElementSize), PL_SER(mComponentMask), PL_SER(mComponentRawNameHash), PL_SER(mSparseMap), PL_SER(mData));
		}
	};

	class PLAZA_API ECS {
	public:
		static void RegisterComponents();
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
			static void UpdateSelfAndChildrenTransform(TransformComponent& transform, TransformComponent* parentTransform, Scene* scene, bool updateLocal = true, bool forceUpdateLocal = false);
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
