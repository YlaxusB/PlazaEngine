#include "Engine/Core/PreCompiledHeaders.h"
#include "ComponentsConverter.h"

namespace Plaza {
	SerializableMaterial ComponentsConverter::ConvertMaterial(Material* material) {
		SerializableMaterial  serializedMaterial{};
		serializedMaterial.assetUuid = material->mAssetUuid;
		serializedMaterial.diffuseColor = material->diffuse->rgba;
		serializedMaterial.normalUuid = material->normal->mAssetUuid;
		serializedMaterial.roughnessFloat = material->roughnessFloat;
		serializedMaterial.roughnessUuid = material->roughness->mAssetUuid;
		serializedMaterial.metalnessFloat = material->metalnessFloat;
		serializedMaterial.metalnessUuid = material->metalness->mAssetUuid;
		serializedMaterial.intensity = material->intensity;
		serializedMaterial.shininess = material->shininess;

		return serializedMaterial;
	}
}