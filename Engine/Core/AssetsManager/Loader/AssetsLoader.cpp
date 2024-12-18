#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsLoader.h"
#include "Engine/Core/AssetsManager/Serializer/AssetsSerializer.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"
#include <ThirdParty/cereal/cereal/archives/binary.hpp>
#include <future>

namespace Plaza {
	void AssetsLoader::LoadAsset(Asset* asset) {
		if (asset->GetExtension() == Standards::metadataExtName)
			AssetsLoader::LoadMetadata(asset);
		else
			AssetsManager::NewAsset<Asset>(asset->mAssetUuid, asset->mAssetPath.string());

		if (asset->GetExtension() == Standards::modelExtName)
			AssetsLoader::LoadPrefab(asset);
		else if (asset->GetExtension() == Standards::materialExtName)
			AssetsLoader::LoadMaterial(asset, Application::Get()->mSettings.mCommonSerializationMode);
		else if (asset->GetExtension() == Standards::animationExtName)
			AssetsLoader::LoadAnimation(asset, Application::Get()->mSettings.mAnimationSerializationMode);
	}

	std::shared_ptr<Scene> AssetsLoader::LoadScene(Asset* asset, SerializationMode serializationMode) {
		std::shared_ptr<Scene> scene = AssetsSerializer::DeSerializeFile<Scene>(asset->mAssetPath.string(), serializationMode);
		Scene::GetEditorScene()->Copy(scene.get());
		return scene;
	}

	SerializablePrefab AssetsLoader::DeserializePrefab(const std::string& path, const SerializationMode& serializationMode) {
		SerializablePrefab prefab{};

		std::shared_ptr<Serp> loadedPrefab = AssetsSerializer::DeSerializeFile<Serp>(path, serializationMode);

		prefab = loadedPrefab->data;
		prefab = loadedPrefab->data;

		return prefab;
	}

	void AssetsLoader::LoadDeserializedEntity(const SerializableEntity& deserializedEntity, std::unordered_map<uint64_t, uint64_t>& equivalentUuids, bool loadToScene) {
		Entity* newEntity = new Entity(deserializedEntity.name, Scene::GetActiveScene()->mainSceneEntity, loadToScene);
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
				MeshRenderer* meshRenderer = new MeshRenderer(mesh, AssetsManager::GetMaterialsVector(deserializedMeshRenderer.materialsUuid));
				meshRenderer->instanced = true;
				meshRenderer->mMaterials = AssetsManager::GetMaterialsVector(deserializedMeshRenderer.materialsUuid);
				RenderGroup* newRenderGroup = new RenderGroup(meshRenderer->mesh, meshRenderer->mMaterials);
				//meshRenderer->renderGroup = Scene::GetActiveScene()->AddRenderGroup(newRenderGroup);
				newEntity->AddComponent<MeshRenderer>(meshRenderer);
			}
			if (typeid(*component.get()) == typeid(SerializableCollider)) {
				SerializableCollider deserializedCollider = *(SerializableCollider*)(component.get());

				Collider* collider = new Collider(deserializedCollider.uuid);
				for (unsigned int i = 0; i < deserializedCollider.shapesCount; ++i) {
					// TODO: WARNING: ADD COLLIDER SHAPES AGAIN
					//collider->CreateShape(deserializedCollider.shapes[i].shape, newEntity->GetComponent<Transform>(), mesh);
				}
				newEntity->AddComponent<Collider>(collider);
			}
		}
	}

	void AssetsLoader::LoadPrefabToMemory(const std::string& path) {
		//if (!asset)
		//	return;
		SerializablePrefab deserializedPrefab = AssetsLoader::DeserializePrefab(path, Application::Get()->mSettings.mModelSerializationMode);

		for (const SerializableEntity& deserializedEntity : deserializedPrefab.entities) {
			for (const std::shared_ptr<SerializableComponents> component : deserializedEntity.components) {
				//if (component->type == SerializableComponentType::MESH_RENDERER) {
				if (typeid(*component.get()) == typeid(SerializableMeshRenderer)) {
					SerializableMeshRenderer deserializedMeshRenderer = *(SerializableMeshRenderer*)(component.get());//std::any_cast<SerializableMeshRenderer>(deserializedEntity.components.find(SerializableComponentType::MESH_RENDERER)->second);

					SerializableMesh* deserializedMesh = &deserializedMeshRenderer.serializedMesh;
					std::vector<glm::vec3> tangents;
					Mesh* mesh = Application::Get()->mRenderer->CreateNewMesh(deserializedMesh->vertices, deserializedMesh->normals, deserializedMesh->uvs, deserializedMesh->tangent, deserializedMesh->indices, deserializedMesh->materialsIndices, false, deserializedMesh->bonesHolders, deserializedMesh->uniqueBones);

					/* TEMPORARY: TODO: FIX THE UNIQUE BONES */
					for (int i = 0; i < deserializedMesh->uniqueBones.size(); ++i) {
						mesh->uniqueBonesInfo.emplace(deserializedMesh->uniqueBones[i].mId, deserializedMesh->uniqueBones[i]);
					}

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
			Entity* equivalentEntity = Scene::GetActiveScene()->GetEntity(equivalentUuids.find(deserializedEntity.entityUuid)->second);
			Entity* equivalentEntityParent = Scene::GetActiveScene()->GetEntity(equivalentEntity->equivalentPrefabParentUuid);
			if (deserializedEntity.parentUuid != 0) {
				//Entity* equivalentEntity = Scene::GetActiveScene()->GetEntity(deserializedEntity.parentUuid);
				Entity* oldParent = Scene::GetActiveScene()->GetEntity(equivalentEntity->parentUuid);
				if (equivalentUuids.find(deserializedEntity.parentUuid) != equivalentUuids.end()) {
					Entity* newParent = Scene::GetActiveScene()->GetEntity(equivalentUuids.at(deserializedEntity.parentUuid));
					Scene::GetActiveScene()->GetEntity(equivalentEntity->uuid)->ChangeParent(oldParent, newParent);
				}
			}
		}
	}

	static std::vector<std::future<void>> futures;
	void AssetsLoader::LoadPrefab(Asset* asset) {
		Asset assetCopy = *asset;
		//Application::Get()->mThreadsManager->mAssetsLoadingThread->AddToParallelQueue([assetCopy]() {
		//	//futures.push_back(std::async(std::launch::async, [assetCopy]() {
		//	if (AssetsManager::mLoadedModels.find(assetCopy.mAssetUuid) != AssetsManager::mLoadedModels.end()) {
		//		LoadPrefabToScene(AssetsManager::mLoadedModels.at(assetCopy.mAssetUuid), true);
		//	}
		//	else {
		//		//static std::mutex queueMutex;
		//		//std::lock_guard<std::mutex> lock(queueMutex);
		//		LoadPrefabToMemory(assetCopy.mAssetPath.string());
		//		if (AssetsManager::mLoadedModels.find(assetCopy.mAssetUuid) != AssetsManager::mLoadedModels.end())
		//			LoadPrefabToScene(AssetsManager::mLoadedModels.at(assetCopy.mAssetUuid), true);
		//	}
		//	//	}));
		//	});
	}
}