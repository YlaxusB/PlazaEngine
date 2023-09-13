#pragma once
#include "Engine/Vendor/physx/PxPhysicsAPI.h"
#include "Engine/Components/Component.h"
namespace Plaza {
	enum ColliderShapeEnum {
		BOX,
		SPHERE,
		CAPSULE,
		PLANE,
		CYLINDER,
		MESH
	};
	struct ColliderShape {
		ColliderShape(physx::PxShape* shape, ColliderShapeEnum newEnum = ColliderShapeEnum::BOX, uint64_t meshUuid = 0) : mPxShape(shape), mEnum(newEnum), mMeshUuid(meshUuid){}
		physx::PxShape* mPxShape;
		ColliderShapeEnum mEnum;
		uint64_t mMeshUuid;
	};
	class Collider : public Component {
	public:
		vector<ColliderShape*> mShapes;
		physx::PxRigidActor* mRigidActor;
		physx::PxRigidBody* mStaticPxRigidBody;
		physx::PxRigidBody* pxRigidBody;
		bool mDynamic = false;
		Collider(std::uint64_t uuid, RigidBody* rigidBody = nullptr);
		~Collider();
		void RemoveActor();
		void Init(RigidBody* rigidBody);
		void InitDynamic(RigidBody* rigidBody = nullptr);
		void InitStatic();
		void InitCollider(RigidBody* rigidBody = nullptr);
		void Update();

		void CreateShape(ColliderShapeEnum shapeEnum, Transform* transform, Mesh* mesh = nullptr);
		void AddShape(ColliderShape* shape);
		void AddConvexMeshShape(Mesh* mesh);
		void AddMeshShape(Mesh* mesh);
		void UpdateShapeScale(glm::vec3 scale);
		void UpdateAllShapesScale();
		template <typename EnumType>
		void SetSelfAndChildrenFlag(EnumType flag, bool value);

		void SetFlag(physx::PxRigidBody* rigidBody, physx::PxRigidBodyFlag::Enum flag, bool value);
		void SetFlag(Collider* collider, physx::PxRigidBodyFlag::Enum flag, bool value);
		void SetFlag(physx::PxRigidActor* actor, physx::PxActorFlag::Enum flag, bool value);

		void UpdatePose();
		void UpdatePose(Transform* transform);
	};
}