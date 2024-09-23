#pragma once
namespace Plaza {
	class Physics {
	public:
		static physx::PxDefaultAllocator m_defaultAllocatorCallback;
		static physx::PxDefaultErrorCallback m_defaultErrorCallback;
		static physx::PxDefaultCpuDispatcher* m_dispatcher;
		static physx::PxTolerancesScale m_toleranceScale;
		static physx::PxFoundation* m_foundation;
		static physx::PxPhysics* m_physics;

		static inline std::unordered_map<uint64_t, physx::PxGeometry*> sCookedGeometries = std::unordered_map<uint64_t, physx::PxGeometry*>();
		static inline std::unordered_map<uint64_t, std::unordered_map<PhysicsMaterial, physx::PxShape*>> sShapes = std::unordered_map<uint64_t, std::unordered_map<PhysicsMaterial, physx::PxShape*>>(); // Keys: Mesh UUID, Physics Material hash map

		static physx::PxMaterial* defaultMaterial;

		static physx::PxMaterial* InitializePhysicsMaterial(float staticFriction, float dynamicFriction, float restitution);
		static PhysicsMaterial& GetDefaultPhysicsMaterial() {
			return AssetsManager::GetPhysicsMaterial(0.5f, 0.5f, 0.5f);
		}

		static physx::PxScene* m_scene;
		static physx::PxMaterial* m_material;
		static physx::PxPvd* m_pvd;
		static bool m_canRun;

		static const float maxFrameAdvance;
		static float accumulatedTime;
		static float stepSize;

		static physx::PxSceneDesc GetSceneDesc();

		static bool Advance(float dt);
		static void Init();
		static void InitScene();
		static void InitPhysics();
		static void Update();

		static physx::PxTransform GetPxTransform(Transform& transform);
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
	};
}