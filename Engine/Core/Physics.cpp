#include "Engine/Core/PreCompiledHeaders.h"
#include "Physics.h"
#include <ThirdParty/PhysX/physx/include/cooking/Pxc.h>
#include <ThirdParty/PhysX/physx/include/characterkinematic/PxController.h>
#include <ThirdParty/PhysX/physx/include/characterkinematic/PxControllerManager.h>
#include <ThirdParty/PhysX/physx/include/characterkinematic/PxControllerBehavior.h>
#include <ThirdParty/PhysX/physx/include/characterkinematic/PxBoxController.h>
using namespace physx;
namespace Plaza {
	class CollisionCallback : public physx::PxSimulationEventCallback {
	public:
		virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override {
			// Your collision handling implementation
			for (PxU32 i = 0; i < nbPairs; i++) {
				const PxContactPair& cp = pairs[i];
				PxContactPairPoint contacts[1];
				PxU32 nbContacts = cp.extractContacts(contacts, 1);

				if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND) {
					PxVec3 collisionPoint = contacts[0].position;
					uint64_t uuid1 = (uint64_t)(pairHeader.actors[0]->userData);
					uint64_t uuid2 = (uint64_t)(pairHeader.actors[1]->userData);

					//if (name != "Road")
					//	std::cout << "Found: " << name << "\n";
					auto it1 = Application->activeScene->csScriptComponents.find(uuid1);
					if (it1 != Application->activeScene->csScriptComponents.end()) {
						for (auto [key, value] : it1->second.scriptClasses) {
							void* params[] =
							{
								(void*)(new uint64_t(uuid2)),
								new glm::vec3(collisionPoint.x, collisionPoint.y, collisionPoint.z)
							};
							Mono::CallMethod(value->monoObject, value->methods.find("OnCollide")->second, params);
						}
					}

					auto it2 = Application->activeScene->csScriptComponents.find(uuid2);
					if (it2 != Application->activeScene->csScriptComponents.end()) {
						for (auto [key, value] : it2->second.scriptClasses) {
							void* params[] =
							{
								(void*)(new uint64_t(uuid1)),
								new glm::vec3(collisionPoint.x, collisionPoint.y, collisionPoint.z)
							};
							Mono::CallMethod(value->monoObject, value->methods.find("OnCollide")->second, params);
						}
					}


					// Place your logic for handling collisions here
					// You can call your Mono::CallMethod or perform any other required actions
				}
			}
		}

		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override {
			// Your trigger handling implementation
			for (PxU32 i = 0; i < count; i++) {
				const PxTriggerPair& tp = pairs[i];
				std::cout << "Trigger detected\n";
				// Handle trigger events if necessary
			}
		}

		virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override {
			// Handle constraint break events
			for (physx::PxU32 i = 0; i < count; i++) {
				// Implement the logic for constraint break events
			}
		}

		virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override {
			// Handle wake events
			for (physx::PxU32 i = 0; i < count; i++) {
				// Implement the logic for wake events
			}
		}

		virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override {
			// Handle sleep events
			for (physx::PxU32 i = 0; i < count; i++) {
				// Implement the logic for sleep events
			}
		}

		virtual void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override {
			// Handle advance events
			for (physx::PxU32 i = 0; i < count; i++) {
				// Implement the logic for advance events
			}
		}
	};
	CollisionCallback mCollisionCallback;
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
	physx::PxControllerManager* Physics::m_controllerManager = NULL;

	physx::PxMaterial* Physics::defaultMaterial = nullptr;

	const float Physics::maxFrameAdvance = 0.1f;
	float Physics::accumulatedTime = 0.0f;
	float Physics::stepSize = 1 / 60.0f;

	bool Physics::m_canRun = true;

	bool Physics::Advance(float dt) {
		PLAZA_PROFILE_SECTION("Advance");
		accumulatedTime += dt;
		if (accumulatedTime < stepSize) {
			return false;
		}

		accumulatedTime -= stepSize;
		{
			PLAZA_PROFILE_SECTION("Simulate");
			m_scene->simulate(stepSize);
		}
		{
			PLAZA_PROFILE_SECTION("Fetch Results");
			Physics::m_scene->fetchResults(true);
		}
		return true;
	}

	PxFilterFlags FilterShaderExample(
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		// let triggers through
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}
		// generate contacts for all that were not filtered above
		pairFlags = PxPairFlag::eCONTACT_DEFAULT;

		// trigger the contact callback for pairs (A,B) where 
		// the filtermask of A contains the ID of B and vice versa.
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
		return PxFilterFlag::eDEFAULT;
	}
	class ContactCallBack : public PxSimulationEventCallback
	{
		// PxSimulationEventCallback
		virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {};
		virtual void onWake(PxActor** actors, PxU32 count) override {};
		virtual void onSleep(PxActor** actors, PxU32 count) override {};
		virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override {};
		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override {};
		virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override {};
	};

	class FilterCallback : public PxSimulationFilterCallback {
		virtual		PxFilterFlags	pairFound(PxU64 pairID,
			PxFilterObjectAttributes attributes0, PxFilterData filterData0, const PxActor* a0, const PxShape* s0,
			PxFilterObjectAttributes attributes1, PxFilterData filterData1, const PxActor* a1, const PxShape* s1,
			PxPairFlags& pairFlags) override {
			return PxFilterFlag::eDEFAULT;
		}
		virtual		void			pairLost(PxU64 pairID,
			PxFilterObjectAttributes attributes0, PxFilterData filterData0,
			PxFilterObjectAttributes attributes1, PxFilterData filterData1,
			bool objectRemoved) override {};
		virtual		bool			statusChange(PxU64& pairID, PxPairFlags& pairFlags, PxFilterFlags& filterFlags) override {
			return false;
		}
	};

	void setupFiltering(PxShape* shape, PxU32 filterGroup, PxU32 filterMask)
	{
		PxFilterData filterData;
		filterData.word0 = filterGroup; // word0 = own ID
		filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a contact callback
		shape->setSimulationFilterData(filterData);
	}
	CollisionCallback simulationEventCallback;
	FilterCallback filterCallback;
	physx::PxSceneDesc Physics::GetSceneDesc() {
		physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f); // Set your desired gravity
		sceneDesc.cpuDispatcher = m_dispatcher;
		sceneDesc.filterShader = FilterShaderExample;
		sceneDesc.filterCallback = &filterCallback;
		sceneDesc.simulationEventCallback = &simulationEventCallback;
		//sceneDesc.simulationEventCallback = &collisionCallback;
		//sceneDesc.dynamicTreeRebuildRateHint = 100;
		return sceneDesc;
	}

	void Physics::Init() {
		std::cout << "Initalizing Physics" << std::endl;
		m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocatorCallback, m_defaultErrorCallback);
		if (!m_foundation) {
			std::cerr << "PhysX foundation creation failed!" << std::endl;
		}
		Physics::InitPhysics();
		Physics::InitScene();

		//Physics::m_controllerManager = PxCreateControllerManager(*Physics::m_scene);
	}

	void Physics::InitPhysics() {
		PxTolerancesScale toleranceScale;
		toleranceScale.speed = 9.81f;
		toleranceScale.length = 1;
		m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, toleranceScale, true);
		m_dispatcher = PxDefaultCpuDispatcherCreate(2); // 2 is the number of worker threads
		if (!m_dispatcher) {
			std::cerr << "PhysX CPU dispatcher creation failed!" << std::endl;
		}
	}

	void Physics::InitScene() {
		// Create the PhysX scene
		m_scene = m_physics->createScene(Physics::GetSceneDesc());
		//m_scene->setSimulationEventCallback(&collisionCallback);
		Physics::defaultMaterial = Physics::m_physics->createMaterial(0.0f, 1.0f, 0.5f);
		std::cout << "Physics Initialized" << std::endl;
	}

	void Physics::Update() {
		PLAZA_PROFILE_SECTION("Update");
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