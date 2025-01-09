#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsLoader.h"
#include "Engine/Core/AssetsManager/Serializer/AssetsSerializer.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"
#include <ThirdParty/cereal/cereal/archives/binary.hpp>
#include <future>
#include "Engine/Core/Renderer/Model.h"
#include "Engine/Core/AssetsManager/Asset.h"
#include "Engine/Components/Core/Prefab.h"

namespace Plaza {
	void AssetsLoader::LoadAsset(Asset* asset) {
		if (asset->GetExtension() == Standards::metadataExtName)
			AssetsLoader::LoadMetadata(asset);
		else
			AssetsManager::NewAsset<Asset>(asset->mAssetUuid, asset->mAssetPath.string());

		if (asset->GetExtension() == Standards::modelExtName)
			AssetsLoader::LoadModel(asset);
		if (asset->GetExtension() == Standards::prefabExtName)
			AssetsLoader::LoadPrefab(asset);
		else if (asset->GetExtension() == Standards::materialExtName)
			AssetsLoader::LoadMaterial(asset, Application::Get()->mSettings.mCommonSerializationMode);
		else if (asset->GetExtension() == Standards::animationExtName)
			AssetsLoader::LoadAnimation(asset, Application::Get()->mSettings.mAnimationSerializationMode);
	}

	std::shared_ptr<Model> AssetsLoader::LoadModel(Asset* asset) {
		std::shared_ptr<Model> model = AssetsSerializer::DeSerializeFile<Model>(asset->mAssetPath.string(), Application::Get()->mSettings.mModelSerializationMode);
		AssetsManager::AddModel(model);

		for (auto& [key, mesh] : model->mMeshes) {
			Mesh* newMesh = VulkanRenderer::GetRenderer()->CreateNewMesh(mesh->vertices, mesh->normals, mesh->uvs, mesh->tangent, mesh->indices, mesh->materialsIndices, mesh->usingNormal, mesh->bonesHolder, {});
			newMesh->uuid = key;
			AssetsManager::AddMesh(newMesh);
		}

		return model;
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
		// FIX: Remake model importers
	}

	void AssetsLoader::LoadPrefabToMemory(const std::string& path) {
		// FIX: Remake model importers
	}

	void AssetsLoader::LoadPrefabToScene(Model* model, bool loadToScene) {
		// FIX: Remake model importers
	}

	static std::vector<std::future<void>> futures;
	void AssetsLoader::LoadPrefab(Asset* asset) {
		if (!AssetsManager::GetPrefab(asset->mAssetUuid)) {
			std::shared_ptr<Prefab> prefab = AssetsSerializer::DeSerializeFile<Prefab>(asset->mAssetPath.string(), Application::Get()->mSettings.mPrefabSerializationMode);
			AssetsManager::AddPrefab(prefab);
		}
		// FIX: Remake model importers
		//Asset assetCopy = *asset;
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