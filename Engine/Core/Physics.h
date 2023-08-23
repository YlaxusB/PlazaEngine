#pragma once
namespace Engine {
	class Physics {
	public:
		static physx::PxDefaultAllocator m_defaultAllocatorCallback;
		static physx::PxDefaultErrorCallback m_defaultErrorCallback;
		static physx::PxDefaultCpuDispatcher* m_dispatcher;
		static physx::PxTolerancesScale m_toleranceScale;
		static physx::PxFoundation* m_foundation;
		static physx::PxPhysics* m_physics;

		static physx::PxMaterial* defaultMaterial;

		static physx::PxScene* m_scene;
		static physx::PxMaterial* m_material;
		static physx::PxPvd* m_pvd;
		static bool m_canRun;

		static const float maxFrameAdvance;
		static float accumulatedTime;
		static float stepSize;

		static bool Advance(float dt);
		static void Init();
		static void Update();

		static physx::PxTransform* GetPxTransform(Transform& transform);
	};
}