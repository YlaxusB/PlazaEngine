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
		if (mRigidActor) {
			for (physx::PxShape* shape : mShapes) {
				this->mRigidActor->detachShape(*shape);
			}
			Physics::m_scene->removeActor(*this->mRigidActor);
		}
	}

	void Collider::RemoveActor() {
		if (this->mRigidActor) {
			for (physx::PxShape* shape : mShapes) {
				this->mRigidActor->detachShape(*shape);
			}
			Physics::m_scene->removeActor(*this->mRigidActor);
			mRigidActor = nullptr;
		}
	}

	void Collider::Init(RigidBody* rigidBody) {
		// Check if Rigid Body exists
		this->mDynamic = rigidBody != nullptr;
		if (Application->runningScene || Application->copyingScene)
			InitCollider(rigidBody);
	}

	void Collider::InitCollider(RigidBody* rigidBody) {
		this->RemoveActor();
		this->mRigidActor = Physics::m_physics->createRigidDynamic(*Physics::GetPxTransform(Application->activeScene->transformComponents.at(this->uuid)));
		physx::PxMaterial* material = Physics::defaultMaterial;

		if (this->mDynamic) {
			material = Physics::m_physics->createMaterial(rigidBody->mStaticFriction, rigidBody->mDynamicFriction, rigidBody->mRestitution);
		}
		else {
			this->mRigidActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
		}

		// Attach the shapes with the material to the actor
		for (physx::PxShape* shape : mShapes) {
			shape->setMaterials(&material, 1);
			this->mRigidActor->attachShape(*shape);
		}
		Physics::m_scene->addActor(*this->mRigidActor);
	}

	void Collider::AddShape(physx::PxShape* shape) {
		this->mShapes.push_back(shape);
		Init(nullptr);
	}

	void Collider::UpdateAllShapesScale() {

	}

	void Collider::UpdateShapeScale(glm::vec3 scale) {
		for (int i = 0; i < this->mShapes.size(); ++i) {
			physx::PxShape* shape = this->mShapes[i];
			physx::PxGeometryHolder geometry = this->mShapes[i]->getGeometry();
			physx::PxShape* newShape = this->mShapes[i];
			physx::PxMaterial* material;
			this->mShapes[i]->getMaterials(&material, 1);
			// Scale the geometry parameters by the given factor
			if (geometry.getType() == physx::PxGeometryType::eBOX) {
				physx::PxBoxGeometry boxGeom = geometry.box();
				boxGeom.halfExtents = physx::PxVec3(scale.x / 2, scale.y / 2, scale.z / 2);
				newShape = Physics::m_physics->createShape(boxGeom, *material);
			}
			else if (geometry.getType() == physx::PxGeometryType::eSPHERE) {
				physx::PxSphereGeometry sphereGeometry = geometry.sphere();
				//boxGeom.halfExtents = physx::PxVec3(scale.x / 2, scale.y / 2, scale.z / 2);
				sphereGeometry.radius = (scale.x + scale.y + scale.z) / 3;
				newShape = Physics::m_physics->createShape(sphereGeometry, *material);
				//physx::PxSphereGeometry sphereGeom;
				//sphereGeom.radius *= 3;
				//this->mShapes[i]->setGeometry(physx::PxSphereGeometry(sphereGeom));
			}

			if (Application->runningScene && Application->runtimeScene->rigidBodyComponents.find(this->uuid) != Application->runtimeScene->rigidBodyComponents.end()) {
				// Found RigidBody
				RigidBody* rigidBody = &Application->runtimeScene->rigidBodyComponents.at(this->uuid);

			}
			else {

			}

			if (Application->runningScene) {
				if (this->mDynamic) {
					this->mRigidActor->detachShape(*this->mShapes[i]);
					this->mShapes[i] = newShape;
					this->mRigidActor->attachShape(*this->mShapes[i]);
				}
				else {
					this->mRigidActor->detachShape(*this->mShapes[i]);
					this->mShapes[i] = newShape;
					this->mRigidActor->attachShape(*this->mShapes[i]);
				}
			}
		}
	}

	template <typename EnumType>
	void Collider::SetSelfAndChildrenFlag(EnumType flag, bool value) {
		SetFlag(&Application->activeScene->rigidBodyComponents.at(this->uuid).mRigidActor, flag, value);
		for (uint64_t child : Application->activeScene->entities.at(this->uuid).childrenUuid) {
			SetFlag(&Application->activeScene->rigidBodyComponents.at(child).mRigidActor, flag, value);
		}
	}

	void Collider::SetFlag(physx::PxRigidActor* actor, physx::PxActorFlag::Enum flag, bool value) {
		actor->setActorFlag(flag, value);
	}
	void Collider::SetFlag(physx::PxRigidBody* rigidBody, physx::PxRigidBodyFlag::Enum flag, bool value) {
		rigidBody->setRigidBodyFlag(flag, value);
	}
	void Collider::SetFlag(Collider* collider, physx::PxRigidBodyFlag::Enum flag, bool value) {
		collider->mStaticPxRigidBody->setRigidBodyFlag(flag, value);
	}

	void Collider::UpdatePose(Transform* transform) {
		if (transform) {
			glm::quat quaternion = transform->GetWorldQuaternion();
			physx::PxQuat pxQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

			physx::PxTransform* pxTransform = new physx::PxTransform(
				transform->worldPosition.x, transform->worldPosition.y, transform->worldPosition.z,
				pxQuaternion);
			if (this->mRigidActor) {
				this->mRigidActor->setGlobalPose(*pxTransform);
			}

		}
	}

	void Collider::UpdatePose() {
		this->UpdatePose(this->GetGameObject()->GetComponent<Transform>());
	}
}