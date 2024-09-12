#pragma once
#include "Engine/Core/AssetsManager/AssetsType.h"
#include "Engine/Core/AssetsManager/AssetsManager.h"
#include <ThirdParty/cereal/cereal/archives/json.hpp>
#include <ThirdParty/cereal/cereal/types/string.hpp>
#include <ThirdParty/cereal/cereal/types/vector.hpp>
#include <ThirdParty/cereal/cereal/cereal.hpp>
#include <ThirdParty/cereal/cereal/archives/binary.hpp>
#include <ThirdParty/cereal/cereal/types/map.hpp>
#include <ThirdParty/cereal/cereal/types/polymorphic.hpp>
#include <ThirdParty/cereal/cereal/types/utility.hpp>



namespace Plaza {
	enum class SerializableComponentType : std::uint8_t {
		TRANSFORM,
		MESH_RENDERER,
		COLLIDER
	};
	struct SerializableComponents {
		uint64_t uuid = 0;
		SerializableComponentType type = SerializableComponentType::TRANSFORM;
		virtual void sayType() = 0;

		template <class Archive>
		void serialize(Archive& archive) {
			archive(uuid, static_cast<uint8_t>(type));
		}
	};

	struct SerializableTransform : public SerializableComponents {
		void sayType() {};
		glm::vec3 position = glm::vec3(0.0f);
		glm::quat rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
		glm::vec3 scale = glm::vec3(1.0f);

		template <class Archive>
		void serialize(Archive& archive) {
			archive(cereal::base_class<SerializableComponents>(this), position, rotation, scale);
		}
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
		uint64_t tangentCount;
		std::vector<glm::vec3> tangent;
		uint64_t indicesCount;
		std::vector<unsigned int> indices;
		std::vector<unsigned int> materialsIndices;
		uint64_t bonesHoldersCount;
		std::vector<Plaza::BonesHolder> bonesHolders;
		uint64_t uniqueBonesCount;
		std::vector<Plaza::Bone> uniqueBones;

		template <class Archive>
		void serialize(Archive& archive) {
			archive(assetUuid, assetType, materialAssetUuid, verticesCount, vertices,
				normalsCount, normals, uvsCount, uvs, tangentCount, tangent, indicesCount, indices, materialsIndices,
				bonesHoldersCount, bonesHolders, uniqueBonesCount, uniqueBones);
		}
	};

	struct SerializableMeshRenderer : public SerializableComponents {
		void sayType() {};
		std::vector<uint64_t> materialsUuid;
		SerializableMesh serializedMesh{};

		template <class Archive>
		void serialize(Archive& archive) {
			archive(cereal::base_class<SerializableComponents>(this), materialsUuid, serializedMesh);
		}
	};

	struct SerializableColliderShape {
		Plaza::ColliderShape::ColliderShapeEnum shape;
		uint64_t meshUuid;
		glm::vec3 scale;

		template <class Archive>
		void serialize(Archive& archive) {
			archive(shape, meshUuid, scale);
		}
	};

	struct SerializableCollider : public SerializableComponents {
		void sayType() {};
		uint64_t colliderUuid = 0;
		uint32_t shapesCount = 0;
		std::vector<SerializableColliderShape> shapes{};

		template <class Archive>
		void serialize(Archive& archive) {
			archive(cereal::base_class<SerializableComponents>(this), colliderUuid, shapesCount, shapes);
		}
	};



	struct SerializableEntity {
		std::string name;
		uint64_t entityUuid;
		uint64_t parentUuid;
		uint32_t childrenCount;
		std::vector<uint64_t> childrenUuid;
		uint32_t componentsCount;
		//std::map<SerializableComponentType, std::any> components{};
		std::vector<std::shared_ptr<SerializableComponents>> components{};

		template <class Archive>
		void serialize(Archive& archive) {
			archive(name, entityUuid, parentUuid, childrenCount, childrenUuid, componentsCount, components);

			if (Archive::is_loading::value) {
				//components.clear();
				for (uint32_t i = 0; i < componentsCount; ++i) {
					//int type;
					//archive(type);
					//components.push_back(createComponent(SerializableComponentType(type)));
					//archive(*components.back());
				}
			}
			else {
				for (const auto& component : components) {
					//if (component->type != SerializableComponentType::TRANSFORM) {
					//SerializableComponentType type = component->type;
					//archive(cereal::make_nvp("type", static_cast<int>(type)));
					////archive(static_cast<int&>(type)); 
					////archive(*component);
					//if (type == SerializableComponentType::TRANSFORM)
					//	archive(*(SerializableTransform*)(component.get()));
					//if (type == SerializableComponentType::MESH_RENDERER)
					//	archive(*(SerializableMeshRenderer*)(component.get()));
					////if (type == SerializableComponentType::COLLIDER)
					////	archive(*(SerializableCollider*)(component.get()));
				//}
				}
			}
		}
	private:
		std::shared_ptr<SerializableComponents> createComponent(SerializableComponentType type) {
			switch (type) {
			case SerializableComponentType::TRANSFORM:
				return std::make_shared<SerializableTransform>();
			case SerializableComponentType::MESH_RENDERER:
				return std::make_shared<SerializableMeshRenderer>();
			case SerializableComponentType::COLLIDER:
				return std::make_shared<SerializableCollider>();
			default:
				// Handle unknown type
				return nullptr;
			}
		}


	};
	struct SerializablePrefab {
		uint64_t assetUuid;
		std::string name;
		uint32_t entitiesCount;
		std::vector<SerializableEntity> entities{};

		template <class Archive>
		void serialize(Archive& archive) {
			archive(assetUuid, name, entitiesCount, entities);
		}
	};

	struct Serp {
		SerializablePrefab data;

		template <class Archive>
		void save(Archive& ar) const
		{
			ar(data);
		}


		template <class Archive>
		void load(Archive& ar)
		{
			ar(data);
		}
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
		static SerializablePrefab SerializePrefab(Entity* mainEntity, std::filesystem::path outPath);
		static void SerializeMesh(Mesh* mesh, std::filesystem::path outPath);
		static void SerializeAnimation(Animation& animation, std::filesystem::path outPath);

		template <typename T>
		static void SerializeFile(T& file, std::string path) {
			std::ofstream os(path, std::ios::binary);
			cereal::BinaryOutputArchive archive(os);
			archive(file);
			os.close();
		}

		template <typename T>
		static std::shared_ptr<T> DeSerializeFile(std::string path) {
			std::ifstream is(path, std::ios::binary);
			cereal::BinaryInputArchive archive(is);
			std::shared_ptr<T> file = std::make_shared<T>();
			archive(*file.get());
			is.close();
			static_cast<Asset*>(file.get())->mAssetPath = std::filesystem::path{ path };
			return file;
		}
	};
}

namespace cereal {
	template <class Archive> void serialize(Archive& archive, glm::vec2& v) {
		archive(v.x, v.y);
	}

	template <class Archive> void serialize(Archive& archive, glm::vec3& v) {
		archive(v.x, v.y, v.z);
	}

	template <class Archive> void serialize(Archive& archive, glm::vec4& v) {
		archive(v.x, v.y, v.z, v.w);
	}

	template <class Archive> void serialize(Archive& archive, glm::quat& q) {
		archive(q.w, q.x, q.y, q.z);
	}

	template <class Archive> void serialize(Archive& archive, glm::mat4& mat) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				archive(cereal::make_nvp("m" + std::to_string(i) + std::to_string(j), mat[i][j]));
			}
		}
	}

	template <class Archive>
	void serialize(Archive& archive, SerializableComponentType& state) {
		std::uint8_t value = static_cast<
			std::uint8_t>(state);
		archive(value);
		state = static_cast<SerializableComponentType>(value);
	}

	//template <class Archive>
	//void serialize(Archive& archive, Plaza::ColliderShape::ColliderShapeEnum& state) {
	//	std::uint8_t value = static_cast<
	//		std::uint8_t>(state);
	//	archive(value);
	//	state = static_cast<State>(value);
	//}

	//template <class Archive>
	//void serialize(Archive& archive, SerializableComponentType& type) {
	//	archive(static_cast<int>(type)); // Serialize the enum as its underlying integer type
	//}

	//template <class Archive>
	//void serialize(Archive& archive, Plaza::ColliderShape::ColliderShapeEnum& type) {
	//	archive(static_cast<int>(type)); // Serialize the enum as its underlying integer type
	//}

	//template<class Archive>
	//void serialize(Archive& archive, std::shared_ptr<Plaza::SerializableComponents> ptr) {
	//	if (ptr) {
	//		archive(ptr->type); // Serialize type information
	//		archive(*ptr); // Serialize the component
	//	}
	//	else {
	//		// Serialize null pointer
	//		//archive(cereal::make_nvp("null", nullptr));
	//	}
	//}


}


CEREAL_REGISTER_TYPE(SerializableTransform);
CEREAL_REGISTER_TYPE(SerializableMeshRenderer);
CEREAL_REGISTER_TYPE(SerializableCollider);
//CEREAL_REGISTER_TYPE(SerializableComponentType);
//CEREAL_REGISTER_TYPE(Plaza::ColliderShape::ColliderShapeEnum);
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializableComponents, SerializableTransform);
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializableComponents, SerializableMeshRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(SerializableComponents, SerializableCollider);