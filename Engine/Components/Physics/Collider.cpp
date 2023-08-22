#include "Engine/Core/PreCompiledHeaders.h"
#include "Collider.h"
#include "Engine/Core/Physics.h"
namespace Engine {
	Collider::Collider(std::uint64_t uuid, RigidBody* rigidBody) {
		this->uuid = uuid;
		if (Application->runningScene)
			this->Init(rigidBody);
	}

	Collider::~Collider() {
		if (!mDynamic && this->mStaticPxRigidBody)
			Physics::m_scene->removeActor(*mStaticPxRigidBody);
		if (mDynamic) {
			for (physx::PxShape* shape : mShapes) {
				RigidBody* rigidBody = &Application->runtimeScene->rigidBodyComponents.at(this->uuid);
				rigidBody->mRigidActor->detachShape(*shape);
			}
		}
	}

	void Collider::Init(RigidBody* rigidBody) {
		// Check if Rigid Body exists
		if (rigidBody) {
			this->mDynamic = true;
			InitDynamic(rigidBody);
		}
		else {
			this->mDynamic = false;
			InitStatic();
		}
	}

	void Collider::InitDynamic(RigidBody* rigidBody) {
		if (!rigidBody)
			rigidBody = &Application->runtimeScene->rigidBodyComponents.at(this->uuid);
		for (physx::PxShape* shape : mShapes) {
			physx::PxMaterial* material = Physics::m_physics->createMaterial(rigidBody->mStaticFriction, rigidBody->mDynamicFriction, rigidBody->mRestitution);
			shape->setMaterials(&material, 1);
			rigidBody->mRigidActor->attachShape(*shape);
		}
		if (mStaticPxRigidBody) {
			Physics::m_scene->removeActor(*this->mStaticPxRigidBody);
			mStaticPxRigidBody = nullptr;
		}
	}

	void Collider::InitStatic() {
		for (physx::PxShape* shape : mShapes) {
			Transform& transform = *Application->activeScene->entities.at(this->uuid).GetComponent<Transform>();
			// Create a dynamic rigid body
			glm::quat quaternion = transform.GetWorldQuaternion();
			physx::PxQuat pxQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

			physx::PxTransform* pxTransform = new physx::PxTransform(
				transform.worldPosition.x, transform.worldPosition.y, transform.worldPosition.z,
				pxQuaternion);
			physx::PxMaterial* shapeMaterial;
			shape->getMaterials(&shapeMaterial, 1, 0);
			physx::PxRigidBody* pxRigidBody = physx::PxCreateDynamic(*Physics::m_physics, *pxTransform, shape->getGeometry(), *shapeMaterial, 5);
			pxRigidBody->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
			this->mStaticPxRigidBody = pxRigidBody;
			Physics::m_scene->addActor(*pxRigidBody);
		}
	}

	void Collider::AddShape(physx::PxShape* shape) {
		this->mShapes.push_back(shape);
		Init(nullptr);
	}
}