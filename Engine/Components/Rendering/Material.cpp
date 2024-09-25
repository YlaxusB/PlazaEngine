#include "Engine/Core/PreCompiledHeaders.h"
#include "Material.h"

namespace Plaza {
	void Material::GetDeserializedTextures() {
		if (mDeserializedTexturesUuid[0]) diffuse = std::shared_ptr<Texture>(AssetsManager::GetTexture(mDeserializedTexturesUuid[0]));
		if (mDeserializedTexturesUuid[1]) normal = std::shared_ptr<Texture>(AssetsManager::GetTexture(mDeserializedTexturesUuid[1]));
		if (mDeserializedTexturesUuid[2]) metalness = std::shared_ptr<Texture>(AssetsManager::GetTexture(mDeserializedTexturesUuid[2]));
		if (mDeserializedTexturesUuid[3]) roughness = std::shared_ptr<Texture>(AssetsManager::GetTexture(mDeserializedTexturesUuid[3]));
		if (mDeserializedTexturesUuid[4]) height = std::shared_ptr<Texture>(AssetsManager::GetTexture(mDeserializedTexturesUuid[4]));
		if (mDeserializedTexturesUuid[5]) aoMap = std::shared_ptr<Texture>(AssetsManager::GetTexture(mDeserializedTexturesUuid[5]));
	}
}

