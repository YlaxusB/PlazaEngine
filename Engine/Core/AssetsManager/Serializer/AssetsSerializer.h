#pragma once


namespace Plaza {
	enum SerializableComponentType {
		TRANSFORM,
		MESH_RENDERER,
		COLLIDER
	};
	struct SerializableComponents {
		uint64_t uuid;
		SerializableComponentType type;
	};

	struct SerializableTransform : public SerializableComponents {
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};

	struct SerializableMaterial {
		uint64_t assetUuid;
		glm::vec3 diffuseColor;
		uint64_t diffuseUuid;
		uint64_t normalUuid;
		float roughnessFloat;
		uint64_t roughnessUuid;
		float metalnessFloat;
		uint64_t metalnessUuid;
		float intensity;
		float shininess;
	};

	struct SerializableMesh {
		uint64_t assetUuid;
		AssetType assetType;
		uint64_t materialAssetUuid;
		uint64_t verticesCount;
		std::vector<glm::vec3> vertices;
		uint64_t normalsCount;
		std::vector<glm::vec3> normals;
		uint64_t uvsCount;
		std::vector<glm::vec2> uvs;
		uint64_t indicesCount;
		std::vector<unsigned int> indices;
	};

	struct SerializableMeshRenderer : public SerializableComponents {
		uint64_t materialUuid;
		SerializableMesh serializedMesh{};
	};

	struct SerializableColliderShape {
		Plaza::ColliderShape::ColliderShapeEnum shape;
		uint64_t meshUuid;
		glm::vec3 scale;
	};

	struct SerializableCollider : public SerializableComponents {
		uint64_t colliderUuid;
		uint32_t shapesCount;
		std::vector<SerializableColliderShape> shapes;
	};

	struct SerializableEntity {
		std::string name;
		uint64_t entityUuid;
		uint64_t parentUuid;
		uint32_t childrenCount;
		std::vector<uint64_t> childrenUuid;
		uint32_t componentsCount;
		std::map<SerializableComponentType, std::any> components{};
	};
	struct SerializablePrefab {
		uint64_t assetUuid;
		std::string name;
		uint32_t entitiesCount;
		std::vector<SerializableEntity> entities{};
	};

	// Function to serialize an unordered map
	template<typename Key, typename Value>
	void SerializeMap(std::ofstream& file, const std::unordered_map<Key, Value>& map) {
		size_t size = map.size();
		file.write(reinterpret_cast<const char*>(&size), sizeof(size)); // Write the size of the map

		for (const auto& pair : map) {
			file.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first)); // Write the key
			file.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second)); // Write the value
		}
	}

	class AssetsSerializer {
	public:
		static void SerializeAsset(std::filesystem::path filePath, std::filesystem::path outPath) {
			
		}

		static void SerializeMaterial(Material* material, std::filesystem::path outPath);
		static void SerializePrefab(Entity* mainEntity, std::filesystem::path outPath);
		static void SerializeMesh(Mesh* mesh, std::filesystem::path outPath);
	};
}