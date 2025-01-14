#pragma once
#include "Engine/Core/Engine.h"
#include "Engine/Components/Physics/PhysicsMaterial.h"

namespace Plaza {
	class PhysicsMaterial;
	class PLAZA_API MyQueryFilterCallback : public physx::PxQueryFilterCallback {
	public:
		virtual physx::PxQueryHitType::Enum preFilter(
			const physx::PxFilterData& filterData,
			const physx::PxShape* shape,
			const physx::PxRigidActor* actor,
			physx::PxHitFlags& queryFlags) override
		{
			if (actor && actor->userData == mEntityToIgnore) {
				return physx::PxQueryHitType::eNONE;
			}
			return physx::PxQueryHitType::eBLOCK;
		}

		// Set the entity to ignore
		void setEntityToIgnore(void* entityToIgnore) {
			mEntityToIgnore = entityToIgnore;
		}

		physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit, const physx::PxShape* shape, const physx::PxRigidActor* actor) override {
			return physx::PxQueryHitType::eTOUCH;
		}

	private:
		void* mEntityToIgnore = nullptr;
	};

	struct PLAZA_API RaycastHit {
		uint64_t hitUuid;
		glm::vec3 point;
		glm::vec3 normal;
		bool missed = false;
	};

	class ColliderShape;
	class PLAZA_API Physics {
	public:
		static inline physx::PxDefaultAllocator m_defaultAllocatorCallback;
		static inline physx::PxDefaultErrorCallback m_defaultErrorCallback;
		static inline physx::PxDefaultCpuDispatcher* m_dispatcher = nullptr;
		static inline physx::PxTolerancesScale m_toleranceScale;
		static inline physx::PxFoundation* m_foundation = nullptr;
		static inline physx::PxPhysics* m_physics = nullptr;

		static inline std::unordered_map<uint64_t, physx::PxGeometry*> sCookedGeometries = std::unordered_map<uint64_t, physx::PxGeometry*>();
		static inline std::unordered_map<uint64_t, std::unordered_map<PhysicsMaterial, physx::PxShape*>> sShapes = std::unordered_map<uint64_t, std::unordered_map<PhysicsMaterial, physx::PxShape*>>(); // Keys: Mesh UUID, Physics Material hash map

		static inline physx::PxMaterial* defaultMaterial = nullptr;

		static physx::PxMaterial* InitializePhysicsMaterial(float staticFriction, float dynamicFriction, float restitution);
		static PhysicsMaterial& GetDefaultPhysicsMaterial();

		static inline physx::PxScene* m_scene = nullptr;
		static inline physx::PxMaterial* m_material = nullptr;
		static inline physx::PxPvd* m_pvd = nullptr;
		static inline bool m_canRun = true;

		static inline const float maxFrameAdvance = 0.1f;
		static inline float accumulatedTime = 0.0f;
		static inline float stepSize = 1 / 60.0f;

		static physx::PxSceneDesc GetSceneDesc();

		static bool Advance(float dt);
		static void Init();
		static void InitScene();
		static void InitPhysics();
		static void Update(Scene* scene);

		static physx::PxTransform GetPxTransform(TransformComponent& transform);
		static physx::PxTransform* ConvertMat4ToPxTransform(const glm::mat4& mat);

		static physx::PxShape* GetPhysXShape(ColliderShape* colliderShape, PhysicsMaterial* material);
		static void InitDefaultGeometries();
		static physx::PxGeometry* CookMeshGeometry(Mesh* mesh);
		static physx::PxGeometry* CookConvexMeshGeometry(Mesh* mesh);
		static physx::PxGeometry* GetGeometry(uint64_t uuid);
		static physx::PxGeometry* GetCubeGeometry();
		static physx::PxGeometry* GetPlaneGeometry();
		static physx::PxGeometry* GetSphereGeometry();
		static physx::PxGeometry* GetCapsuleGeometry();
		static void DeleteShape();

		static void Raycast(glm::vec3 origin, glm::vec3 direction, RaycastHit& hit);
	private:
		static physx::PxVec3 GlmToPhysX(const glm::vec3& vector);
	};
}