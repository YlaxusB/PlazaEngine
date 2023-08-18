#include "Engine/Core/PreCompiledHeaders.h"
#include "Physics.h"

using namespace physx;
namespace Engine {
	class UserErrorCallback : public physx::PxErrorCallback
	{
	public:
		virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
		{
			std::cout << "shit" << std::endl;
		}
	};
	physx::PxDefaultAllocator Physics::m_defaultAllocatorCallback;
	physx::PxDefaultErrorCallback Physics::m_defaultErrorCallback;
	physx::PxDefaultCpuDispatcher* Physics::m_dispatcher = NULL;
	physx::PxTolerancesScale Physics::m_toleranceScale;
	physx::PxFoundation* Physics::m_foundation = NULL;
	physx::PxPhysics* Physics::m_physics = NULL;
	physx::PxScene* Physics::m_scene = NULL;
	physx::PxMaterial* Physics::m_material = NULL;
	physx::PxPvd* Physics::m_pvd = NULL;

	const float Physics::maxFrameAdvance = 0.1f;
	float Physics::accumulatedTime = 0.0f;
	float Physics::stepSize = 1/60.0f;

	bool Physics::m_canRun = true;

	bool Physics::Advance(float dt) {
		accumulatedTime += dt;
		if (accumulatedTime < stepSize) {
			return false;
		}

		accumulatedTime -= stepSize;
		m_scene->simulate(stepSize);
		Physics::m_scene->fetchResults(true);
		return true;
	}

	void Physics::Init() {
		std::cout << "Initalizing Physics" << std::endl;
		m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocatorCallback, m_defaultErrorCallback);
		if (!m_foundation) {
			std::cerr << "PhysX foundation creation failed!" << std::endl;
		}
		PxTolerancesScale toleranceScale;
		toleranceScale.speed = 9.81f;
		toleranceScale.length = 100;
		m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, toleranceScale, true);
		m_dispatcher = PxDefaultCpuDispatcherCreate(2); // 2 is the number of worker threads
		if (!m_dispatcher) {
			std::cerr << "PhysX CPU dispatcher creation failed!" << std::endl;
		}

		// Create the PhysX scene
		physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f); // Set your desired gravity
		sceneDesc.cpuDispatcher = m_dispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		m_scene = m_physics->createScene(sceneDesc);

		std::cout << "Physics Initialized" << std::endl;
	}

	void Physics::Update() {
		for (auto& [key, value] : Application->runtimeScene->rigidBodyComponents) {
			value.Update();
		}
	}
}