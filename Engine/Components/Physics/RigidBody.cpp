#include "Engine/Core/PreCompiledHeaders.h"
#include "RigidBody.h"
#include "Engine/Core/Physics.h"
#include "Collider.h"
#include "Editor/GUI/gizmo.h"
using namespace physx;
namespace Plaza {
	RigidBody::RigidBody(uint64_t uuid, bool initWithPhysics, bool dynamic) {
		if (initWithPhysics) {
			this->uuid = uuid;
			this->dynamic = dynamic;
			Init();
		}
	}

	/// <summary>
	/// Called when the scene starts
	/// </summary>
	void RigidBody::Init() {
		Collider* collider = this->GetGameObject()->GetComponent<Collider>();
		if (collider) {
			if (Application->activeScene->colliderComponents.find(this->uuid) != Application->activeScene->colliderComponents.end())
				Application->activeScene->colliderComponents.at(this->uuid).Init(this);
			mRigidActor = collider->mRigidActor;
		}
		collider->SetFlags(collider, this->rigidDynamicLockFlags);
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

	glm::vec3 QuaternionToEulerAngles(const physx::PxQuat& quat) {
		float yaw, pitch, roll;
		quat.toRadiansAndUnitAxis(yaw, *new physx::PxVec3(1.0f, 0.0f, 0.0f)); // Extract yaw
		quat.toRadiansAndUnitAxis(pitch, *new physx::PxVec3(0.0f, 1.0f, 0.0f)); // Extract pitch
		quat.toRadiansAndUnitAxis(roll, *new physx::PxVec3(0.0f, 0.0f, 1.0f)); // Extract roll

		return glm::vec3(pitch, yaw, roll); // Return Euler angles as a glm::Vec3
	}

	void RigidBody::Update() {
		if (canUpdate) {
			Transform& transform = *Application->activeScene->entities.at(this->uuid).GetComponent<Transform>();
			Transform& parentTransform = Application->activeScene->transformComponents.at(this->GetGameObject()->parentUuid);
			// Convert Px to Glm
			physx::PxTransform pxTransform = mRigidActor->getGlobalPose();
			PxQuat rotationQuaternion = pxTransform.q;
			glm::quat glmQuaternion(rotationQuaternion.w, rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z);
			glm::vec3 eulerAngles = glm::eulerAngles(glm::normalize(glmQuaternion));

			// Transform the world rotation of the PxTransform to local rotation
			glm::vec3 transformedRotation = glm::eulerAngles(glm::quat_cast(glm::inverse(parentTransform.GetTransform()) * glm::toMat4(glm::quat(eulerAngles))));

			// Apply the delta rotation to prevent gimbal lock
			transform.rotation += transformedRotation - glm::eulerAngles(transform.GetLocalQuaternion());
			transform.relativePosition = glm::vec3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
			transform.UpdateSelfAndChildrenTransform();
		}
	}
	void RigidBody::UpdateGlobalPose() {
		if (this->mRigidActor) {
			Transform& transform = *Application->activeScene->entities.at(this->uuid).GetComponent<Transform>();
			physx::PxTransform* pxTransform = Physics::ConvertMat4ToPxTransform(transform.modelMatrix);
			this->mRigidActor->setGlobalPose(*pxTransform);
		}
	}

	void RigidBody::ApplyForce(glm::vec3 force) {
		if (this->mRigidActor) {
			this->mRigidActor->is<PxRigidDynamic>()->addForce(PxVec3(force.x, force.y, force.z), physx::PxForceMode::eFORCE);
		}
	};



	void RigidBody::SetRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::Enum flag, bool value) {
		if (value)
			this->rigidDynamicLockFlags.raise(flag);
		else
			this->rigidDynamicLockFlags.clear(flag);
		if (Application->runningScene)
			this->mRigidActor->is<physx::PxRigidDynamic>()->setRigidDynamicLockFlag(flag, value);
	}
}
