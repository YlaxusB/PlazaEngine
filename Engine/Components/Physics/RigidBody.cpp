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
		physx::PxBoxGeometry boxGeometry(transform.scale.x / 2.1, transform.scale.y / 2.1, transform.scale.z / 2.1);
		physx::PxMaterial* defaultMaterial = Physics::m_physics->createMaterial(mStaticFriction, mDynamicFriction, mRestitution);
		physx::PxRigidBody* pxRigidBody;
		if (dynamic)
			pxRigidBody = physx::PxCreateDynamic(*Physics::m_physics, *pxTransform, boxGeometry, *defaultMaterial, density);
		else
			pxRigidBody = physx::PxCreateKinematic(*Physics::m_physics, *pxTransform, boxGeometry, *defaultMaterial, density);

		if (Application->activeScene->colliderComponents.find(this->uuid) != Application->activeScene->colliderComponents.end())
			pxRigidBody->attachShape(*Application->activeScene->colliderComponents.at(this->uuid).m_shape);
		// Add the rigid body to the scene
		Physics::m_scene->addActor(*pxRigidBody);
		//Physics::m_scene->setGravity(PxVec3(gravity.x, gravity.y, gravity.z));
		//pxRigidBody->addForce(PxVec3(0.0f, 30000.0f, 0.0f));
		// Store the PhysX rigid body reference
		m_rigidBody = pxRigidBody;
	}

	void RigidBody::UpdateRigidBody() {
		if (this->m_rigidBody) {
			Physics::m_scene->removeActor(*m_rigidBody);
			this->Init();
		}
	}

	RigidBody::~RigidBody() {
		if (m_rigidBody)
			Physics::m_scene->removeActor(*m_rigidBody);
	}

	void RigidBody::Update() {
		if (canUpdate) {
			Transform& transform = *Application->activeScene->entities.at(this->uuid).GetComponent<Transform>();
			physx::PxTransform pxTransform = m_rigidBody->getGlobalPose();
			PxQuat rotationQuaternion = pxTransform.q;
			glm::quat glmQuaternion(rotationQuaternion.w, rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z);
			glm::vec3 eulerAngles = glm::eulerAngles(glmQuaternion);
			transform.relativePosition = glm::vec3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);
			transform.rotation = glm::vec3(eulerAngles.x, eulerAngles.y, eulerAngles.z);
			transform.UpdateChildrenTransform();
		}
	}
	void RigidBody::UpdateGlobalPose() {
		if (this->m_rigidBody) {
			Transform& transform = *Application->activeScene->entities.at(this->uuid).GetComponent<Transform>();
			glm::quat quaternion = transform.GetWorldQuaternion();
			physx::PxQuat pxQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

			physx::PxTransform* pxTransform = new physx::PxTransform(
				transform.worldPosition.x, transform.worldPosition.y, transform.worldPosition.z,
				pxQuaternion);

			this->m_rigidBody->setGlobalPose(*pxTransform);
		}
	}

	void RigidBody::ApplyForce(glm::vec3 force) {
		if (this->m_rigidBody) {
			this->m_rigidBody->addForce(PxVec3(force.x, force.y, force.z), physx::PxForceMode::eFORCE);
		}
	}

}