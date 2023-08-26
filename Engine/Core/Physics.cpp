#include "Engine/Core/PreCompiledHeaders.h"
#include "Physics.h"
#include <physx/cooking/Pxc.h>
using namespace physx;
namespace Plaza {
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

	physx::PxMaterial* Physics::defaultMaterial = nullptr;

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
		toleranceScale.speed = 981;
		toleranceScale.length = 1;
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

		Physics::defaultMaterial = Physics::m_physics->createMaterial(0.0f, 1.0f, 0.5f);
	}

	void Physics::Update() {
		for (auto& [key, value] : Application->runtimeScene->rigidBodyComponents) {
			value.Update();
		}
		//for (auto& [key, value] : Application->runtimeScene->colliderComponents) {
		//	if (Application->runtimeScene->rigidBodyComponents.find(key) == Application->runtimeScene->rigidBodyComponents.end()) {
		//		value.Update();
		//	}
		//}
	}

	physx::PxTransform* Physics::GetPxTransform(Transform& transform) {
		//// Create a dynamic rigid body
		glm::quat quaternion = transform.GetWorldQuaternion();
		physx::PxQuat pxQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
		glm::vec3 worldPosition = transform.GetWorldPosition();
		physx::PxTransform* pxTransform = new physx::PxTransform(
			worldPosition.x, worldPosition.y, worldPosition.z,
			pxQuaternion);
		return pxTransform;
	}

	physx::PxTransform* Physics::ConvertMat4ToPxTransform(const glm::mat4& mat) {
		// Extract translation from the glm::mat4
		glm::vec3 translation = mat[3];

		// Extract rotation quaternion from the upper-left 3x3 submatrix
		glm::quat rotationQuaternion = glm::quat_cast(glm::mat3(mat));

		// Create a physx::PxQuat from the rotation quaternion
		physx::PxQuat pxQuaternion(rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z, rotationQuaternion.w);

		// Create a physx::PxTransform using the translation and quaternion

		return new physx::PxTransform(physx::PxVec3(translation.x, translation.y, translation.z), pxQuaternion);
	}
}