#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsLoader.h"
#include "Engine/Core/AssetsManager/Serializer/AssetsSerializer.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"
#include <ThirdParty/cereal/cereal/archives/binary.hpp>

namespace Plaza {
	SerializablePrefab DeserializePrefab(std::string path) {
		SerializablePrefab prefab{};

		Serp loadedPrefab;
		std::ifstream is(path, std::ios::binary);
		cereal::BinaryInputArchive archive(is);
		archive(loadedPrefab);
		prefab = loadedPrefab.data;
		std::cout << loadedPrefab.data.assetUuid << "\n";
		is.close();
		prefab = loadedPrefab.data;
		//std::ifstream file(path, std::ios::binary);
		//if (!file.is_open()) {
		//	std::cerr << "Error: Failed to open file for reading: " << path << std::endl;
		//	return prefab;
		//}

		//file.read(reinterpret_cast<char*>(&prefab.assetUuid), sizeof(prefab.assetUuid));

		//uint32_t nameLength;
		//file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
		//prefab.name.resize(nameLength);
		//file.read(&prefab.name[0], nameLength);

		//file.read(reinterpret_cast<char*>(&prefab.entitiesCount), sizeof(prefab.entitiesCount));

		//// Read entities
		//prefab.entities.resize(prefab.entitiesCount);
		//for (auto& entity : prefab.entities) {
		//	uint32_t entityNameLength;
		//	file.read(reinterpret_cast<char*>(&entityNameLength), sizeof(entityNameLength));
		//	entity.name.resize(entityNameLength);
		//	file.read(&entity.name[0], entityNameLength);

		//	file.read(reinterpret_cast<char*>(&entity.entityUuid), sizeof(entity.entityUuid));
		//	file.read(reinterpret_cast<char*>(&entity.parentUuid), sizeof(entity.parentUuid));
		//	file.read(reinterpret_cast<char*>(&entity.childrenCount), sizeof(entity.childrenCount));

		//	entity.childrenUuid.resize(entity.childrenCount);
		//	for (auto& childUuid : entity.childrenUuid) {
		//		file.read(reinterpret_cast<char*>(&childUuid), sizeof(childUuid));
		//	}

		//	file.read(reinterpret_cast<char*>(&entity.componentsCount), sizeof(entity.componentsCount));
		//	//entity.components.(entity.componentsCount);
		//	for (unsigned int i = 0; i < entity.componentsCount; ++i) {
		//		SerializableComponentType type;
		//		file.read(reinterpret_cast<char*>(&type), sizeof(SerializableComponentType));

		//		// Read Components
		//		if (type == SerializableComponentType::TRANSFORM)
		//		{
		//			SerializableTransform deserializedTransform{};
		//			file.read(reinterpret_cast<char*>(&deserializedTransform.uuid), sizeof(deserializedTransform.uuid));
		//			file.read(reinterpret_cast<char*>(&deserializedTransform.type), sizeof(deserializedTransform.type));
		//			file.read(reinterpret_cast<char*>(&deserializedTransform.position), sizeof(deserializedTransform.position));
		//			file.read(reinterpret_cast<char*>(&deserializedTransform.rotation), sizeof(deserializedTransform.rotation));
		//			file.read(reinterpret_cast<char*>(&deserializedTransform.scale), sizeof(deserializedTransform.scale));

		//			entity.components.emplace(type, deserializedTransform);
		//		}
		//		else if (type == SerializableComponentType::MESH_RENDERER)
		//		{
		//			SerializableMeshRenderer deserializedMeshRenderer{};
		//			file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.uuid), sizeof(deserializedMeshRenderer.uuid));
		//			file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.type), sizeof(deserializedMeshRenderer.type));

		//			file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.materialUuid), sizeof(uint64_t));

		//			/* Read vertices, normals, uvs, indices */
		//			file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.assetUuid), sizeof(uint64_t));

		//			file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.verticesCount), sizeof(uint64_t));
		//			deserializedMeshRenderer.serializedMesh.vertices.resize(deserializedMeshRenderer.serializedMesh.verticesCount);
		//			for (unsigned int i = 0; i < deserializedMeshRenderer.serializedMesh.verticesCount; ++i) {
		//				file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.vertices[i]), sizeof(glm::vec3));
		//			}
		//			file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.normalsCount), sizeof(uint64_t));
		//			deserializedMeshRenderer.serializedMesh.normals.resize(deserializedMeshRenderer.serializedMesh.normalsCount);
		//			for (unsigned int i = 0; i < deserializedMeshRenderer.serializedMesh.normalsCount; ++i) {
		//				file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.normals[i]), sizeof(glm::vec3));
		//			}
		//			file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.uvsCount), sizeof(uint64_t));
		//			deserializedMeshRenderer.serializedMesh.uvs.resize(deserializedMeshRenderer.serializedMesh.uvsCount);
		//			for (unsigned int i = 0; i < deserializedMeshRenderer.serializedMesh.uvsCount; ++i) {
		//				file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.uvs[i]), sizeof(glm::vec2));
		//			}
		//			file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.indicesCount), sizeof(uint64_t));
		//			deserializedMeshRenderer.serializedMesh.indices.resize(deserializedMeshRenderer.serializedMesh.indicesCount);
		//			for (unsigned int i = 0; i < deserializedMeshRenderer.serializedMesh.indicesCount; ++i) {
		//				file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.indices[i]), sizeof(unsigned int));
		//			}
		//			file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.bonesHoldersCount), sizeof(uint64_t));
		//			deserializedMeshRenderer.serializedMesh.indices.resize(deserializedMeshRenderer.serializedMesh.bonesHoldersCount);
		//			for (unsigned int i = 0; i < deserializedMeshRenderer.serializedMesh.bonesHoldersCount; ++i) {
		//				file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.bonesHolders[i]), sizeof(Plaza::BonesHolder));
		//			}
		//			file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.uniqueBonesCount), sizeof(uint64_t));
		//			deserializedMeshRenderer.serializedMesh.indices.resize(deserializedMeshRenderer.serializedMesh.uniqueBonesCount);
		//			for (unsigned int i = 0; i < deserializedMeshRenderer.serializedMesh.uniqueBonesCount; ++i) {
		//				file.read(reinterpret_cast<char*>(&deserializedMeshRenderer.serializedMesh.uniqueBones[i]), sizeof(Plaza::Bone));
		//			}

		//			entity.components.emplace(type, deserializedMeshRenderer);
		//		}
		//		else if (type == SerializableComponentType::COLLIDER)
		//		{
		//			SerializableCollider deserializedCollider{};
		//			file.read(reinterpret_cast<char*>(&deserializedCollider.uuid), sizeof(deserializedCollider.uuid));
		//			file.read(reinterpret_cast<char*>(&deserializedCollider.type), sizeof(deserializedCollider.type));

		//			file.read(reinterpret_cast<char*>(&deserializedCollider.shapesCount), sizeof(deserializedCollider.shapesCount));
		//			deserializedCollider.shapes.resize(deserializedCollider.shapesCount);
		//			for (unsigned int i = 0; i < deserializedCollider.shapesCount; ++i) {
		//				file.read(reinterpret_cast<char*>(&deserializedCollider.shapes[i].shape), sizeof(deserializedCollider.shapes[i].shape));
		//				file.read(reinterpret_cast<char*>(&deserializedCollider.shapes[i].meshUuid), sizeof(deserializedCollider.shapes[i].meshUuid));
		//				file.read(reinterpret_cast<char*>(&deserializedCollider.shapes[i].scale), sizeof(deserializedCollider.shapes[i].scale));
		//			}

		//			entity.components.emplace(type, deserializedCollider);
		//		}
		//	}
		//}

		//file.close();
		return prefab;
	}

	void LoadDeserializedEntity(const SerializableEntity& deserializedEntity, std::unordered_map<uint64_t, uint64_t>& equivalentUuids, bool loadToScene) {
		Entity* newEntity = new Entity(deserializedEntity.name, Application->activeScene->mainSceneEntity, loadToScene);
		newEntity->equivalentPrefabUuid = deserializedEntity.entityUuid;
		equivalentUuids.emplace(deserializedEntity.entityUuid, newEntity->uuid);

		Mesh* mesh = nullptr;
		for (std::shared_ptr<SerializableComponents> component : deserializedEntity.components) {
			if (typeid(*component.get()) == typeid(SerializableTransform)) {
				SerializableTransform deserializedTransform = *(SerializableTransform*)(component.get());
				newEntity->GetComponent<Transform>()->SetRelativePosition(deserializedTransform.position);
				newEntity->GetComponent<Transform>()->SetRelativeRotation(deserializedTransform.rotation);
				newEntity->GetComponent<Transform>()->SetRelativeScale(deserializedTransform.scale);
			}
			if (typeid(*component.get()) == typeid(SerializableMeshRenderer)) {
				SerializableMeshRenderer deserializedMeshRenderer = *(SerializableMeshRenderer*)(component.get());
				SerializableMesh* deserializedMesh = &deserializedMeshRenderer.serializedMesh;
				mesh = AssetsManager::GetMesh(deserializedMesh->assetUuid);
				MeshRenderer* meshRenderer = new MeshRenderer(mesh, Application->activeScene->GetMaterial(deserializedMeshRenderer.materialUuid));
				meshRenderer->instanced = true;
				meshRenderer->material = Application->activeScene->GetMaterial(deserializedMeshRenderer.materialUuid);
				RenderGroup* newRenderGroup = new RenderGroup(meshRenderer->mesh, meshRenderer->material);
				meshRenderer->renderGroup = Application->activeScene->AddRenderGroup(newRenderGroup);
				newEntity->AddComponent<MeshRenderer>(meshRenderer);
			}
			if (typeid(*component.get()) == typeid(SerializableCollider)) {
				SerializableCollider deserializedCollider = *(SerializableCollider*)(component.get());

				Collider* collider = new Collider(deserializedCollider.uuid);
				for (unsigned int i = 0; i < deserializedCollider.shapesCount; ++i) {
					collider->CreateShape(deserializedCollider.shapes[i].shape, newEntity->GetComponent<Transform>(), mesh);
				}
				newEntity->AddComponent<Collider>(collider);
			}
		}
		/*
		Entity* obj = new Entity(name, parent, addToScene);
		obj->changingName = true;
		Application->activeScene->entities.at(obj->uuid).changingName = true;
		Gui::Hierarchy::Item::firstFocus = true;
		obj->GetComponent<Transform>()->UpdateChildrenTransform();
		MeshRenderer* meshRenderer = new MeshRenderer(*mesh, Scene::DefaultMaterial());
		meshRenderer->instanced = true;
		//meshRenderer->mesh = new Mesh(*mesh);
		meshRenderer->material = Scene::DefaultMaterial();
		RenderGroup* newRenderGroup = new RenderGroup(meshRenderer->mesh, meshRenderer->material);
		meshRenderer->renderGroup = Application->activeScene->AddRenderGroup(newRenderGroup);
		//meshRenderer->renderGroup->material = make_shared<Material>(*Scene::DefaultMaterial());
		obj->AddComponent<MeshRenderer>(meshRenderer);
		Editor::selectedGameObject = obj;
		*/
	}

	void AssetsLoader::LoadPrefabToMemory(Asset* asset) {
		if (!asset)
			return;
		SerializablePrefab deserializedPrefab = DeserializePrefab(asset->mPath.string());

		for (const SerializableEntity& deserializedEntity : deserializedPrefab.entities) {
			for (const std::shared_ptr<SerializableComponents> component : deserializedEntity.components) {
				//if (component->type == SerializableComponentType::MESH_RENDERER) {
				if (typeid(*component.get()) == typeid(SerializableMeshRenderer)) {
					SerializableMeshRenderer deserializedMeshRenderer = *(SerializableMeshRenderer*)(component.get());//std::any_cast<SerializableMeshRenderer>(deserializedEntity.components.find(SerializableComponentType::MESH_RENDERER)->second);

					SerializableMesh* deserializedMesh = &deserializedMeshRenderer.serializedMesh;
					Mesh* mesh = &Application->mRenderer->CreateNewMesh(deserializedMesh->vertices, deserializedMesh->normals, deserializedMesh->uvs, std::vector<glm::vec3>(), std::vector<glm::vec3>(), deserializedMesh->indices, *Scene::DefaultMaterial(), false);
					mesh->uuid = deserializedMesh->assetUuid;
					mesh->meshId = deserializedMesh->assetUuid;

					AssetsManager::AddMesh(mesh);
					//AssetsManager::mLoadedMeshes.emplace(mesh->meshId, mesh);
				}
			}
		}
		AssetsManager::mLoadedModels.emplace(deserializedPrefab.assetUuid, new LoadedModel{ deserializedPrefab.assetUuid, deserializedPrefab });
	}

	void AssetsLoader::LoadPrefabToScene(LoadedModel* model, bool loadToScene) {
		std::unordered_map<uint64_t, uint64_t> equivalentUuids = std::unordered_map<uint64_t, uint64_t>(); // Left is the prefab uuid, and right is the entity uuid
		for (const SerializableEntity& deserializedEntity : model->mSerializablePrefab.entities) {
			LoadDeserializedEntity(deserializedEntity, equivalentUuids, loadToScene);
		}

		for (const SerializableEntity& deserializedEntity : model->mSerializablePrefab.entities) {
			Entity* equivalentEntity = Application->activeScene->GetEntity(equivalentUuids.find(deserializedEntity.entityUuid)->second);
			Entity* equivalentEntityParent = Application->activeScene->GetEntity(equivalentEntity->equivalentPrefabParentUuid);
			if (deserializedEntity.parentUuid != 0) {
				//Entity* equivalentEntity = Application->activeScene->GetEntity(deserializedEntity.parentUuid);
				Entity* oldParent = Application->activeScene->GetEntity(equivalentEntity->parentUuid);
				if (equivalentUuids.find(deserializedEntity.parentUuid) != equivalentUuids.end()) {
					Entity* newParent = Application->activeScene->GetEntity(equivalentUuids.at(deserializedEntity.parentUuid));
					Application->activeScene->GetEntity(equivalentEntity->uuid)->ChangeParent(oldParent, newParent);
				}
			}
		}
	}

	void AssetsLoader::LoadPrefab(Asset* asset) {
		if (AssetsManager::mLoadedModels.find(asset->mAssetUuid) != AssetsManager::mLoadedModels.end()) {
			LoadPrefabToScene(AssetsManager::mLoadedModels.at(asset->mAssetUuid), true);
		}
		else {
			LoadPrefabToMemory(asset);
			if (AssetsManager::mLoadedModels.find(asset->mAssetUuid) != AssetsManager::mLoadedModels.end())
				LoadPrefabToScene(AssetsManager::mLoadedModels.at(asset->mAssetUuid), true);
		}
	}
}