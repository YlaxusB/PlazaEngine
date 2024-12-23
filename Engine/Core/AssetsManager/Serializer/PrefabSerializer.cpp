#include "AssetsSerializer.h"
#include "ComponentsConverter/ComponentsConverter.h"
#include "Engine/Core/AssetsManager/Serializer/ComponentsSerializer/ComponentsSerializer.h"
#include <fstream>
#include <ThirdParty/cereal/cereal/archives/binary.hpp>
#include <ThirdParty/cereal/cereal/types/polymorphic.hpp>

namespace Plaza {
	SerializableTransform* ConvertTransform(TransformComponent* transform) {
		// FIX: Completely remove these serializable prefabs to replace with cereal
		SerializableTransform* serializableTransform = new SerializableTransform();
		//serializableTransform->uuid = transform->mUuid;
		//serializableTransform->type = SerializableComponentType::TRANSFORM;
		//serializableTransform->position = transform->relativePosition;
		//serializableTransform->rotation = transform->rotation;
		//serializableTransform->scale = transform->scale;
		return serializableTransform;
	}

	SerializableEntity ConvertEntity(Entity* entity, uint32_t& entityCount, std::vector<SerializableEntity>& entitiesVector) {
		// FIX: Completely remove these serializable prefabs to replace with cereal
		SerializableEntity serializableEntity{};

		return serializableEntity;
	}
	SerializablePrefab AssetsSerializer::SerializePrefab(Entity* entity, std::filesystem::path outPath, SerializationMode serializationMode) {
		// FIX: Completely remove these serializable prefabs to replace with cereal
		SerializablePrefab serializablePrefab{};

		//uint32_t entityCount = 0;
		//
		//serializablePrefab.assetUuid = Plaza::UUID::NewUUID();
		//serializablePrefab.name = entity->name;
		//serializablePrefab.entities.push_back(ConvertEntity(entity, entityCount, serializablePrefab.entities));
		//serializablePrefab.entitiesCount = entityCount;
		//
		//AssetsSerializer::SerializeFile<SerializablePrefab>(serializablePrefab, outPath.string(), serializationMode);

		return serializablePrefab;
	}
}