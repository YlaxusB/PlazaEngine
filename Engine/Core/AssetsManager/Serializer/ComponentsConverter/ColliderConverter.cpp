#include "Engine/Core/PreCompiledHeaders.h"
#include "ComponentsConverter.h"

namespace Plaza {
	SerializableColliderShape ConvertShape(ColliderShape* shape) {
		SerializableColliderShape serializedShape = SerializableColliderShape();
		serializedShape.scale = glm::vec3(1.0f);
		serializedShape.shape = shape->mEnum;
		serializedShape.meshUuid = shape->mMeshUuid;
		return serializedShape;
	}

	SerializableCollider* ComponentsConverter::ConvertCollider(Collider* collider) {
		SerializableCollider* serializedCollider = new SerializableCollider();
		serializedCollider->uuid = collider->uuid;
		serializedCollider->type = SerializableComponentType::COLLIDER;

		serializedCollider->shapesCount = collider->mShapes.size();
		for (ColliderShape* shape : collider->mShapes) {
			serializedCollider->shapes.push_back(ConvertShape(shape));
		}

		return serializedCollider;
	}
}