#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsSerializer.h"
#include "Engine/Core/AssetsManager/Metadata/Metadata.h"

namespace Plaza {
	void AssetsSerializer::SerializeAssetByExtension(Asset* asset) {
		if (asset->GetExtension() == Standards::metadataExtName)
			AssetsSerializer::SerializeFile<Metadata::MetadataStructure>(*static_cast<Metadata::MetadataStructure*>(asset), asset->mAssetPath.string(), Application::Get()->mSettings.mMetaDataSerializationMode);
		//else
		//	AssetsManager::NewAsset<Asset>(asset->mAssetUuid, asset->mAssetPath.string());

		if (asset->GetExtension() == Standards::materialExtName)
			AssetsSerializer::SerializeFile<Material>(*static_cast<Material*>(AssetsManager::GetMaterial(asset->mAssetUuid)), asset->mAssetPath.string(), Application::Get()->mSettings.mMaterialSerializationMode);
		else if (asset->GetExtension() == Standards::animationExtName)
			AssetsSerializer::SerializeFile<Animation>(*static_cast<Animation*>(AssetsManager::GetAnimation(asset->mAssetUuid)), asset->mAssetPath.string(), Application::Get()->mSettings.mAnimationSerializationMode);
	}
}