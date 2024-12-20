#pragma once
#include "ThirdParty/PhysX/physx/include/PxPhysicsAPI.h"
//#include <PhysX/physx/include/PxPhysicsAPI.h>
#include "Engine/Components/Core/Transform.h"
#include "Engine/Components/Physics/RigidBody.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
#include "Engine/Core/Engine.h"

namespace Plaza {

	struct ColliderShape {
		enum ColliderShapeEnum {
			BOX,
			SPHERE,
			CAPSULE,
			PLANE,
			CYLINDER,
			MESH,
			CONVEX_MESH,
			HEIGHT_FIELD
		};

		ColliderShape() {};
		ColliderShape(physx::PxShape* shape, ColliderShapeEnum newEnum = ColliderShapeEnum::BOX, uint64_t meshUuid = 0) : mPxShape(shape), mEnum(newEnum), mMeshUuid(meshUuid) {}
		physx::PxShape* mPxShape = nullptr;
		ColliderShapeEnum mEnum = ColliderShapeEnum::BOX;
		uint64_t mMeshUuid = 0;

		template <class Archive>
		void serialize(Archive& archive) {
			archive(PL_SER(mEnum), PL_SER(mMeshUuid));
		}
	};
	class PLAZA_API Collider : public Component {
	public:
		glm::vec3 lastScale = glm::vec3(1.0f);
		Collider() {};
		std::vector<std::shared_ptr<ColliderShape>> mShapes = std::vector<std::shared_ptr<ColliderShape>>();
		physx::PxRigidActor* mRigidActor = nullptr;
		physx::PxRigidBody* mStaticPxRigidBody = nullptr;
		physx::PxRigidBody* pxRigidBody = nullptr;
		physx::PxMaterial* material = nullptr;
		bool mDynamic = false;
		virtual void OnInstantiate(Component* componentToInstantiate) override;
		Collider(std::uint64_t uuid, RigidBody* rigidBody = nullptr);
		~Collider() override;
		void RemoveActor();
		void Init(RigidBody* rigidBody);
		void InitDynamic(RigidBody* rigidBody = nullptr) {};
		void InitStatic() {};
		void InitCollider(RigidBody* rigidBody = nullptr);
		void RemoveCollider();
		void Update() {};

		void CreateShape(ColliderShape::ColliderShapeEnum shapeEnum, TransformComponent* transform, Mesh* mesh = nullptr);
		void AddShape(ColliderShape* shape);
		void AddConvexMeshShape(Mesh* mesh);
		void AddMeshShape(Mesh* mesh);
		void AddHeightShape(float** heightData, int size);
		void UpdateShapeScale(glm::vec3 scale);
		void UpdateAllShapesScale();
		template <typename EnumType>
		void SetSelfAndChildrenFlag(EnumType flag, bool value);

		void SetFlag(physx::PxRigidBody* rigidBody, physx::PxRigidBodyFlag::Enum flag, bool value);
		void SetFlag(Collider* collider, physx::PxRigidBodyFlag::Enum flag, bool value);
		void SetFlag(physx::PxRigidActor* actor, physx::PxActorFlag::Enum flag, bool value);
		void SetFlags(Collider* collider, physx::PxRigidDynamicLockFlags flags);

		void UpdatePose();
		void UpdatePose(TransformComponent* transform);

		template <class Archive>
		void serialize(Archive& archive) {
			archive(cereal::base_class<Component>(this), mDynamic, mShapes);

			//			archive(cereal::base_class<Component>(this), CEREAL_NVP(mDynamic), cereal::make_optional(CEREAL_NVP(mShapes)));
		}
	};
}