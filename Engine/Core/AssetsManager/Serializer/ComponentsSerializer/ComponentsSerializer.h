#pragma once
#include "Engine/Core/AssetsManager/Serializer/AssetsSerializer.h"

namespace Plaza {
	class ComponentsSerializer {
	public:
		static void SerializeMaterial(SerializableMaterial& material, std::ofstream& file) {
			file.write(reinterpret_cast<const char*>(&material.diffuseColor), sizeof(uint64_t));
		}
		static void SerializeMeshRenderer(SerializableMeshRenderer* meshRenderer, std::ofstream& file) {
			file.write(reinterpret_cast<const char*>(&meshRenderer->uuid), sizeof(meshRenderer->uuid));
			file.write(reinterpret_cast<const char*>(&meshRenderer->type), sizeof(meshRenderer->type));

			/* TODO: FIX MATERIAL SERIALIZATION */
			//file.write(reinterpret_cast<const char*>(&meshRenderer->materialUuid), sizeof(uint64_t));

			/* Mesh */
			file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.assetUuid), sizeof(uint64_t));

			file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.verticesCount), sizeof(uint64_t));
			for (unsigned int i = 0; i < meshRenderer->serializedMesh.vertices.size(); ++i) {
				file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.vertices[i]), sizeof(glm::vec3));
			}
			file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.normalsCount), sizeof(uint64_t));
			for (unsigned int i = 0; i < meshRenderer->serializedMesh.normals.size(); ++i) {
				file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.normals[i]), sizeof(glm::vec3));
			}
			file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.uvsCount), sizeof(uint64_t));
			for (unsigned int i = 0; i < meshRenderer->serializedMesh.uvs.size(); ++i) {
				file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.uvs[i]), sizeof(glm::vec2));
			}
			file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.indicesCount), sizeof(uint64_t));
			for (unsigned int i = 0; i < meshRenderer->serializedMesh.indices.size(); ++i) {
				file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.indices[i]), sizeof(unsigned int));
			}
			file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.bonesHoldersCount), sizeof(uint64_t));
			for (unsigned int i = 0; i < meshRenderer->serializedMesh.bonesHolders.size(); ++i) {
				file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.bonesHolders[i]), sizeof(Plaza::BonesHolder));
			}
			file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.uniqueBonesCount), sizeof(uint64_t));
			for (unsigned int i = 0; i < meshRenderer->serializedMesh.uniqueBones.size(); ++i) {
				file.write(reinterpret_cast<const char*>(&meshRenderer->serializedMesh.uniqueBones[i]), sizeof(Plaza::Bone));
			}
		}

		static void SerializeCollider(SerializableCollider* collider, std::ofstream& file) {
			file.write(reinterpret_cast<const char*>(&collider->uuid), sizeof(collider->uuid));
			file.write(reinterpret_cast<const char*>(&collider->type), sizeof(collider->type));

			file.write(reinterpret_cast<const char*>(&collider->shapesCount), sizeof(collider->shapesCount));
			for (unsigned int i = 0; i < collider->shapesCount; ++i) {
				file.write(reinterpret_cast<const char*>(&collider->shapes[i].shape), sizeof(collider->shapes[i].shape));
				file.write(reinterpret_cast<const char*>(&collider->shapes[i].meshUuid), sizeof(collider->shapes[i].meshUuid));
				file.write(reinterpret_cast<const char*>(&collider->shapes[i].scale), sizeof(collider->shapes[i].scale));
			}
		}
	};
}