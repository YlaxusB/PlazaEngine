#include "Engine/Core/PreCompiledHeaders.h"
#include "RigidBody.h"
#include "Engine/Core/Physics.h"
#include "Collider.h"
using namespace physx;
namespace Engine {

	RigidBody::RigidBody(uint64_t uuid, bool initWithPhysics, bool dynamic) {
		if (initWithPhysics) {
			this->uuid = uuid;
			this->dynamic = dynamic;
			Init();
		}
	}

	void RigidBody::Init() {
		Transform& transform = *Application->activeScene->entities.at(uuid).GetComponent<Transform>();
		this->transform = make_shared<Transform>(*Application->activeScene->entities.at(uuid).GetComponent<Transform>());
		// Create a dynamic rigid body
		glm::quat quaternion = transform.GetWorldQuaternion();
		physx::PxQuat pxQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

		physx::PxTransform* pxTransform = new PxTransform(
			transform.worldPosition.x, transform.worldPosition.y, transform.worldPosition.z,
			pxQuaternion);
		physx::PxMaterial* defaultMaterial = Physics::m_physics->createMaterial(mStaticFriction, mDynamicFriction, mRestitution);
		physx::PxRigidActor* rigidActor = Physics::m_physics->createRigidDynamic(*pxTransform);
		if (!dynamic)
			rigidActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

		// Add the rigid body to the scene
		Physics::m_scene->addActor(*rigidActor);
		// Store the PhysX rigid body reference
		mRigidActor = rigidActor;
		if (Application->activeScene->colliderComponents.find(this->uuid) != Application->activeScene->colliderComponents.end())
			Application->activeScene->colliderComponents.at(this->uuid).Init(this);
	}

	void RigidBody::UpdateRigidBody() {
		if (this->mRigidActor) {
			Physics::m_scene->removeActor(*mRigidActor);
			this->Init();
		}
	}

	RigidBody::~RigidBody() {
		if (mRigidActor)
			Physics::m_scene->removeActor(*mRigidActor);
	}

	void RigidBody::Update() {
		if (canUpdate) {
			Transform& transform = *Application->activeScene->entities.at(this->uuid).GetComponent<Transform>();
			physx::PxTransform pxTransform = mRigidActor->getGlobalPose();
			PxQuat rotationQuaternion = pxTransform.q;
			glm::quat glmQuaternion(rotationQuaternion.w, rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z);
			glm::vec3 eulerAngles = glm::eulerAngles(glmQuaternion);
			transform.relativePosition = glm::vec3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
			transform.rotation = glm::vec3(eulerAngles.x, eulerAngles.y, eulerAngles.z);
			transform.UpdateChildrenTransform();
		}
	}
	void RigidBody::UpdateGlobalPose() {
		if (this->mRigidActor) {
			Transform& transform = *Application->activeScene->entities.at(this->uuid).GetComponent<Transform>();
			glm::quat quaternion = transform.GetWorldQuaternion();
			physx::PxQuat pxQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

			physx::PxTransform* pxTransform = new physx::PxTransform(
				transform.worldPosition.x, transform.worldPosition.y, transform.worldPosition.z,
				pxQuaternion);

			this->mRigidActor->setGlobalPose(*pxTransform);
		}
	}

	void RigidBody::ApplyForce(glm::vec3 force) {
		if (this->mRigidActor) {
			this->mRigidActor->is<PxRigidDynamic>()->addForce(PxVec3(force.x, force.y, force.z), physx::PxForceMode::eFORCE);
		}
	}

}