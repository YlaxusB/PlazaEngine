#include "AssetsSerializer.h"
#include "ComponentsConverter/ComponentsConverter.h"
#include "Engine/Core/AssetsManager/Serializer/ComponentsSerializer/ComponentsSerializer.h"
#include <fstream>
#include <ThirdParty/cereal/cereal/archives/binary.hpp>
#include <ThirdParty/cereal/cereal/types/polymorphic.hpp>

namespace Plaza {
	SerializableTransform* ConvertTransform(Transform* transform) {
		SerializableTransform* serializableTransform = new SerializableTransform();
		serializableTransform->uuid = transform->uuid;
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
	void AssetsSerializer::SerializePrefab(Entity* entity, std::filesystem::path outPath) {
		std::ofstream outfile(outPath.string(), std::ios::binary);
		if (!outfile.is_open()) {
			std::cerr << "Error opening file for writing!" << std::endl;
			return;
		}

		uint32_t entityCount = 0;

		SerializablePrefab serializablePrefab{};
		serializablePrefab.assetUuid = Plaza::UUID::NewUUID();
		serializablePrefab.name = entity->name;
		serializablePrefab.entities.push_back(ConvertEntity(entity, entityCount, serializablePrefab.entities));
		serializablePrefab.entitiesCount = entityCount;

		std::ofstream os(outPath.string(), std::ios::binary);
		cereal::BinaryOutputArchive archive(os);
		Serp ser{ serializablePrefab };
		archive(ser);
		os.close();

		// serializablePrefab.serialize(archive);


	// Deserialize from binary

		{
			Serp loadedPrefab;
			std::ifstream is(outPath.string(), std::ios::binary);
			cereal::BinaryInputArchive archive2(is);
			archive2(loadedPrefab);
			std::cout << loadedPrefab.data.assetUuid << "\n";
		}

		//std::ofstream file(outPath.string(), std::ios::binary);
		//if (!file.is_open()) {
		//	std::cerr << "Error: Failed to open file for writing: " << outPath.string() << std::endl;
		//	return;
		//}
		/*
		// Write asset UUID
		file.write(reinterpret_cast<const char*>(&serializablePrefab.assetUuid), sizeof(serializablePrefab.assetUuid));

		// Write name length and name
		uint32_t nameLength = static_cast<uint32_t>(serializablePrefab.name.length());
		file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
		file.write(serializablePrefab.name.c_str(), nameLength);

		// Write entities count
		file.write(reinterpret_cast<const char*>(&serializablePrefab.entitiesCount), sizeof(serializablePrefab.entitiesCount));

		// Write entities
		for (const auto& entity : serializablePrefab.entities) {
			uint32_t entityNameLength = static_cast<uint32_t>(entity.name.length());
			file.write(reinterpret_cast<const char*>(&entityNameLength), sizeof(entityNameLength));
			file.write(entity.name.c_str(), entityNameLength);

			// Write entity UUID
			file.write(reinterpret_cast<const char*>(&entity.entityUuid), sizeof(entity.entityUuid));
			// Write parent UUID
			file.write(reinterpret_cast<const char*>(&entity.parentUuid), sizeof(entity.parentUuid));
			// Write children count
			file.write(reinterpret_cast<const char*>(&entity.childrenCount), sizeof(entity.childrenCount));
			// Write children UUIDs
			for (const auto& childUuid : entity.childrenUuid) {
				file.write(reinterpret_cast<const char*>(&childUuid), sizeof(childUuid));
			}
			// Write components count
			file.write(reinterpret_cast<const char*>(&entity.componentsCount), sizeof(entity.componentsCount));
			// Write components
			for (auto& component : entity.components) {
				auto componentType = component.second.type().name();

				file.write(reinterpret_cast<const char*>(&component.first), sizeof(SerializableComponentType));
				if (componentType == typeid(SerializableTransform*).name())
				{
					SerializableTransform* transform = std::any_cast<SerializableTransform*>(component.second);
					file.write(reinterpret_cast<const char*>(&transform->uuid), sizeof(transform->uuid));
					file.write(reinterpret_cast<const char*>(&transform->type), sizeof(transform->type));
					file.write(reinterpret_cast<const char*>(&transform->position), sizeof(transform->position));
					file.write(reinterpret_cast<const char*>(&transform->rotation), sizeof(transform->rotation));
					file.write(reinterpret_cast<const char*>(&transform->scale), sizeof(transform->scale));
				}
				else if (componentType == typeid(SerializableMeshRenderer*).name())
				{
					SerializableMeshRenderer* meshRenderer = std::any_cast<SerializableMeshRenderer*>(component.second);
					ComponentsSerializer::SerializeMeshRenderer(meshRenderer, file);
				}
				else if (componentType == typeid(SerializableCollider*).name())
				{
					SerializableCollider* collider = std::any_cast<SerializableCollider*>(component.second);
					ComponentsSerializer::SerializeCollider(collider, file);
				}

			}
		}
		*/

		//file.close();

		//// Write serializablePrefab to the file
		//outfile.write(reinterpret_cast<const char*>(&serializablePrefab), sizeof(serializablePrefab));
		//if (!outfile) {
		//	std::cerr << "Error writing data to file!" << std::endl;
		//	return;
		//}
		//
		//// Write entities count to the file
		//outfile.write(reinterpret_cast<const char*>(&entityCount), sizeof(entityCount));
		//if (!outfile) {
		//	std::cerr << "Error writing entities count to file!" << std::endl;
		//	return;
		//}
		//
		//outfile.close();
	}
}