#include "Engine/Core/PreCompiledHeaders.h"
#include "Material.h"

namespace Plaza {
	void Material::LoadDeserializedTextures() {
		diffuse = AssetsManager::GetTexture(mDeserializedTexturesUuid[0]);
		normal = AssetsManager::GetTexture(mDeserializedTexturesUuid[1]);
		metalness = AssetsManager::GetTexture(mDeserializedTexturesUuid[2]);
		roughness = AssetsManager::GetTexture(mDeserializedTexturesUuid[3]);
		height = AssetsManager::GetTexture(mDeserializedTexturesUuid[4]);
		aoMap = AssetsManager::GetTexture(mDeserializedTexturesUuid[5]);
	}
}

