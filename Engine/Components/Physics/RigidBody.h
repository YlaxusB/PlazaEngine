#pragma once
#include "Engine/Components/Component.h"

namespace Engine {
	class RigidBody : public Component {
	public:
		//physx::PxRigidStatic* rb;
		//physx::PxMaterial* mMaterial;
		RigidBody(uint64_t uuid, bool initWithPhysics, bool dynamic = true);
		~RigidBody();

		void Init();

		void Update();
		void UpdateGlobalPose();
		void UpdateRigidBody();

		void ApplyForce(glm::vec3 force);


		bool canUpdate = true;

		bool dynamic = true;
		float mStaticFriction = 0.0f;
		float mDynamicFriction = 1.0f;
		float mRestitution = 0.5f;

		float density = 50.0f;
		glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);

		physx::PxRigidBody* m_rigidBody;

		void CopyValuesFrom(const RigidBody& other) {
			dynamic = other.dynamic;
			mStaticFriction = other.mStaticFriction;
			mDynamicFriction = other.mDynamicFriction;
			mRestitution = other.mRestitution;
			density = other.density;
			gravity = other.gravity;
		}
	private:
		shared_ptr<Transform> transform;
	};
}