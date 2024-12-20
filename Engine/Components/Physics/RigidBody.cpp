#include "Engine/Core/PreCompiledHeaders.h"
#include "RigidBody.h"
#include "Engine/Core/Physics.h"
#include "Collider.h"
#include "Editor/GUI/gizmo.h"
using namespace physx;
namespace Plaza {
	void RigidBody::OnInstantiate(Component* componentToInstantiate) {
		this->Init();
	}

	RigidBody::RigidBody(uint64_t uuid, bool initWithPhysics, bool dynamic) {
		if (initWithPhysics) {
			this->mUuid = uuid;
			this->dynamic = dynamic;
			Init();
		}
	}

	/// <summary>
	/// Called when the scene starts
	/// </summary>
	void RigidBody::Init() {
		Collider* collider = this->GetGameObject()->GetComponent<Collider>();
		//AddCollidersOfChildren(this->uuid);
		if (collider) {
			if (Scene::GetActiveScene()->colliderComponents.find(this->mUuid) != Scene::GetActiveScene()->colliderComponents.end())
				Scene::GetActiveScene()->colliderComponents.at(this->mUuid).Init(this);
			mRigidActor = collider->mRigidActor;
			physx::PxRigidBodyExt::setMassAndUpdateInertia(*this->mRigidActor->is<physx::PxRigidDynamic>(), physx::PxReal(this->density));
			physx::PxRigidBodyExt::updateMassAndInertia(*this->mRigidActor->is<physx::PxRigidDynamic>(), physx::PxReal(this->density));
			collider->SetFlags(collider, this->rigidDynamicLockFlags);
		}
		else {
			this->mRigidActor = Physics::m_physics->createRigidDynamic(*new physx::PxTransform(physx::PxIdentity(1.0f)));
		}

		this->SetRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, this->continuousDetection);
		this->mRigidActor->is<PxRigidDynamic>()->setLinearDamping(0.0f);
		this->mRigidActor->is<PxRigidDynamic>()->setAngularDamping(0.0f);
	}

	void RigidBody::AddCollidersOfChildren(uint64_t parent) {
		for (uint64_t child : Scene::GetActiveScene()->entities.at(parent).childrenUuid) {
			if (Scene::GetActiveScene()->entities.at(child).HasComponent<Collider>() && !Scene::GetActiveScene()->entities.at(child).HasComponent<RigidBody>()) {
				Collider* rigidBodyCollider = Scene::GetActiveScene()->entities.at(this->mUuid).GetComponent<Collider>();
				Collider* collider = Scene::GetActiveScene()->entities.at(child).GetComponent<Collider>();
				for (auto& colliderShape : collider->mShapes) {
					rigidBodyCollider->mShapes.push_back(colliderShape);
				}
				collider->RemoveActor();
				AddCollidersOfChildren(child);
			}
		}
	}

	void RigidBody::UpdateRigidBody() {
		if (this->mRigidActor) {
			Physics::m_scene->removeActor(*mRigidActor);
			this->Init();
		}
	}

	RigidBody::~RigidBody() {
		//if (mRigidActor) 
		//	Physics::m_scene->removeActor(*mRigidActor);

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
			TransformComponent& transform = *Scene::GetActiveScene()->entities.at(this->mUuid).GetComponent<TransformComponent>();
			TransformComponent& parentTransform = Scene::GetActiveScene()->transformComponents.at(this->GetGameObject()->parentUuid);
			// Convert Px to Glm
			physx::PxTransform pxTransform = mRigidActor->getGlobalPose();
			PxQuat rotationQuaternion = pxTransform.q;
			glm::quat glmQuaternion(rotationQuaternion.w, rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z);
			glm::vec3 eulerAngles = glm::eulerAngles(glm::normalize(glmQuaternion));

			// Transform the world rotation of the PxTransform to local rotation
			glm::quat transformedRotation = glm::normalize(glm::quat_cast(glm::inverse(parentTransform.GetTransform()) * glm::toMat4(glmQuaternion)));

			// Apply the delta rotation to prevent gimbal lock
			transform.rotation = transformedRotation;//+= transformedRotation - glm::quat_cast(transform.localMatrix);
			transform.relativePosition = glm::vec3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
			transform.UpdateSelfAndChildrenTransform();
		}
	}
	void RigidBody::UpdateGlobalPose() {
		if (this->mRigidActor) {
			TransformComponent& transform = *Scene::GetActiveScene()->entities.at(this->mUuid).GetComponent<TransformComponent>();
			physx::PxTransform* pxTransform = Physics::ConvertMat4ToPxTransform(transform.modelMatrix);
			this->mRigidActor->setGlobalPose(*pxTransform);
		}
	}

	void RigidBody::ApplyForce(glm::vec3 force) {
		if (this->mRigidActor) {
			//this->mRigidActor->is<PxRigidDynamic>()->addForce(PxVec3(force.x, force.y, force.z), physx::PxForceMode::eFORCE);
			this->mRigidActor->is<PxRigidDynamic>()->setLinearVelocity(PxVec3(force.x, force.y, force.z));
			//this->mRigidActor->is<PxRigidDynamic>()->addForce(PxVec3(force.x, force.y, force.z), physx::PxForceMode::eFORCE);
		}
	};

	void RigidBody::AddForce(glm::vec3 force, physx::PxForceMode::Enum mode, bool autowake) {
		if (this->mRigidActor) {
			this->mRigidActor->is<PxRigidDynamic>()->addForce(PxVec3(force.x, force.y, force.z), mode, autowake);
		}
	}

	void RigidBody::AddTorque(glm::vec3 torque, physx::PxForceMode::Enum mode, bool autowake) {
		if (this->mRigidActor) {
			this->mRigidActor->is<PxRigidDynamic>()->addTorque(PxVec3(torque.x, torque.y, torque.z), mode, autowake);
		}
	}

	float RigidBody::GetDrag() {
		if (this->mRigidActor) {
			return this->mRigidActor->is<PxRigidDynamic>()->getLinearDamping();
		}
	}

	void RigidBody::SetDrag(float drag) {
		if (this->mRigidActor) {
			this->mRigidActor->is<PxRigidDynamic>()->setLinearDamping(drag);
		}
	}

	glm::vec3 RigidBody::GetVelocity() {
		if (this->mRigidActor) {
			physx::PxVec3 pxVec = this->mRigidActor->is<PxRigidDynamic>()->getLinearVelocity();
			return glm::vec3(pxVec.x, pxVec.y, pxVec.z);
		}
	}

	void RigidBody::SetVelocity(glm::vec3 vector) {
		if (this->mRigidActor) {
			this->mRigidActor->is<PxRigidDynamic>()->setLinearVelocity(physx::PxVec3(vector.x, vector.y, vector.z));
		}
	}

	void RigidBody::SetRigidBodyFlag(physx::PxRigidBodyFlag::Enum flag, bool value) {
		this->mRigidActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	}

	void RigidBody::SetRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::Enum flag, bool value) {
		if (value)
			this->rigidDynamicLockFlags.raise(flag);
		else
			this->rigidDynamicLockFlags.clear(flag);
		if (Application::Get()->runningScene)
			this->mRigidActor->is<physx::PxRigidDynamic>()->setRigidDynamicLockFlag(flag, value);
	}
}
