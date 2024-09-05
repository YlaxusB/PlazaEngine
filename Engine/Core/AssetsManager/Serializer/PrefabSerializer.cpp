#include "AssetsSerializer.h"
#include "ComponentsConverter/ComponentsConverter.h"
#include "Engine/Core/AssetsManager/Serializer/ComponentsSerializer/ComponentsSerializer.h"
#include <fstream>
#include <ThirdParty/cereal/cereal/archives/binary.hpp>
#include <ThirdParty/cereal/cereal/types/polymorphic.hpp>

namespace Plaza {
	SerializableTransform* ConvertTransform(Transform* transform) {
		SerializableTransform* serializableTransform = new SerializableTransform();
		serializableTransform->uuid = transform->mUuid;
		serializableTransform->type = SerializableComponentType::TRANSFORM;
		serializableTransform->position = transform->relativePosition;
		serializableTransform->rotation = transform->rotation;
		serializableTransform->scale = transform->scale;
		return serializableTransform;
	}

	SerializableEntity ConvertEntity(Entity* entity, uint32_t& entityCount, std::vector<SerializableEntity>& entitiesVector) {
		SerializableEntity serializableEntity{};
		serializableEntity.name = entity->name;
		serializableEntity.entityUuid = entity->uuid;
		serializableEntity.parentUuid = entity->parentUuid;
		serializableEntity.childrenCount = entity->childrenUuid.size();
		serializableEntity.childrenUuid = entity->childrenUuid;

		unsigned int componentCount = 0;
		if (entity->HasComponent<Transform>()) {
			serializableEntity.components.push_back(std::shared_ptr<SerializableComponents>(ConvertTransform(entity->GetComponent<Transform>())));
			componentCount++;
		}
		if (entity->HasComponent<MeshRenderer>()) {
			serializableEntity.components.push_back(std::shared_ptr<SerializableComponents>(ComponentsConverter::ConvertMeshRenderer(entity->GetComponent<MeshRenderer>())));
			componentCount++;
		}
		if (entity->HasComponent<Collider>()) {
			serializableEntity.components.push_back(std::shared_ptr<SerializableComponents>(ComponentsConverter::ConvertCollider(entity->GetComponent<Collider>())));
			componentCount++;
		}

		serializableEntity.componentsCount = componentCount;
		entityCount++;
		for (unsigned int i = 0; i < entity->childrenUuid.size(); ++i) {
			entitiesVector.push_back(ConvertEntity(Application->activeScene->GetEntity(entity->childrenUuid[i]), entityCount, entitiesVector));
		}

		return serializableEntity;
	}
	SerializablePrefab AssetsSerializer::SerializePrefab(Entity* entity, std::filesystem::path outPath) {
		SerializablePrefab serializablePrefab{};

		std::ofstream outfile(outPath.string(), std::ios::binary);
		if (!outfile.is_open()) {
			std::cerr << "Error opening file for writing!" << std::endl;
			return serializablePrefab;
		}

		uint32_t entityCount = 0;

		serializablePrefab.assetUuid = Plaza::UUID::NewUUID();
		serializablePrefab.name = entity->name;
		serializablePrefab.entities.push_back(ConvertEntity(entity, entityCount, serializablePrefab.entities));
		serializablePrefab.entitiesCount = entityCount;

		std::ofstream os(outPath.string(), std::ios::binary);
		cereal::BinaryOutputArchive archive(os);
		archive(serializablePrefab);
		os.close();

		return serializablePrefab;

		//{
		//	Serp loadedPrefab;
		//	std::ifstream is(outPath.string(), std::ios::binary);
		//	cereal::BinaryInputArchive archive2(is);
		//	archive2(loadedPrefab);
		//	std::cout << loadedPrefab.data.assetUuid << "\n";
		//}

	}
}