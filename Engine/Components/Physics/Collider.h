#pragma once
#include "Engine/Vendor/physx/PxPhysicsAPI.h"
#include "Engine/Components/Component.h"
namespace Engine {
	class Collider : public Component {
	public:
		vector<physx::PxShape*> mShapes;
		physx::PxActor* mActor;
		physx::PxRigidBody* mStaticPxRigidBody;
		bool mDynamic = false;
		Collider(std::uint64_t uuid, RigidBody* rigidBody = nullptr);
		~Collider();
		void Init(RigidBody* rigidBody);
		void InitDynamic(RigidBody* rigidBody = nullptr);
		void InitStatic();
		void Update();

		void AddShape(physx::PxShape* shape);
	};
}