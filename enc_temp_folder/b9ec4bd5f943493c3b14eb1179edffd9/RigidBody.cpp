#include "Engine/Core/PreCompiledHeaders.h"
#include "RigidBody.h"
#include "Engine/Core/Physics.h"
#include "Collider.h"
using namespace physx;
namespace Plaza {

	RigidBody::RigidBody(uint64_t uuid, bool initWithPhysics, bool dynamic) {
		if (initWithPhysics) {
			this->uuid = uuid;
			this->dynamic = dynamic;
			Init();
		}
	}

	void RigidBody::Init() {
		Collider* collider = this->GetGameObject()->GetComponent<Collider>();
		if (collider) {
			if (Application->activeScene->colliderComponents.find(this->uuid) != Application->activeScene->colliderComponents.end())
				Application->activeScene->colliderComponents.at(this->uuid).Init(this);
			mRigidActor = collider->mRigidActor;
		}
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
			glm::vec3 eulerAngles = glm::eulerAngles(glm::normalize(glmQuaternion));
			transform.relativePosition = glm::vec3(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);

			//glm::mat4 inverseTransform = glm::inverse(transform.modelMatrix);
			glm::mat4 inverseTransform = glm::inverse(Application->activeScene->transformComponents.at(this->GetGameObject()->parentUuid).modelMatrix);
			glm::mat4 worldRotationMatrix = glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);
			glm::mat4 localRotationMatrix = inverseTransform * worldRotationMatrix;
//			glm::vec3 localEulerAngles = glm::extractEulerAngleXYZ(glm::quat_cast(localRotationMatrix));

			// Given world Euler angles
			glm::vec3 worldEulerAngles = eulerAngles; // Replace with your values

			// Given local rotation quaternion (already calculated or provided)
			glm::quat localRotationQuaternion = glm::quat(transform.modelMatrix); // Replace with your matrix

			// 1. Convert world Euler angles to quaternion
			glm::quat worldRotationQuaternion = glm::quat(worldEulerAngles);

			// 2. Invert local rotation quaternion
			glm::quat invertedLocalRotation = glm::inverse(localRotationQuaternion);

			// 3. Multiply inverted local rotation quaternion by world rotation quaternion
			glm::quat localRotationInWorldSpace = worldRotationQuaternion * invertedLocalRotation;

			// 4. Convert resulting quaternion back to Euler angles in local coordinate system
			glm::vec3 localEulerAngles = glm::eulerAngles(localRotationInWorldSpace);

			// Ensure that the resulting angles are within the appropriate range (-π to π)
			localEulerAngles.x = glm::mod(localEulerAngles.x + glm::pi<float>(), glm::pi<float>() * 2.0f) - glm::pi<float>();
			localEulerAngles.y = glm::mod(localEulerAngles.y + glm::pi<float>(), glm::pi<float>() * 2.0f) - glm::pi<float>();
			localEulerAngles.z = glm::mod(localEulerAngles.z + glm::pi<float>(), glm::pi<float>() * 2.0f) - glm::pi<float>();

			if (Time::frameCount % 100 == 0) {
				std::cout << "X: " << localEulerAngles.x << std::endl;
				std::cout << "Y: " << localEulerAngles.y << std::endl;
				std::cout << "Z: " << localEulerAngles.z << std::endl;
			}
			//transform.rotation = localEulerAngles;
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
	}

}