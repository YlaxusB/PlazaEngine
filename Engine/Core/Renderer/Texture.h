#pragma once
#include <string>
#include <vector>
#include "RendererTypes.h"

namespace Plaza {
	class Texture {
	public:
		uint64_t mAssetUuid;
		std::string type = "";
		std::string path = "";
		glm::vec4 rgba = glm::vec4(255.0f);
		int mIndexHandle = -1;

		uint64_t mDescriptorCount = 1;
		PlTextureType mTextureType = PL_TYPE_2D;
		PlViewType mViewType = PL_VIEW_TYPE_2D;
		PlTextureFormat mFormat = PL_FORMAT_R8G8B8A8_UNORM;
		PlImageUsage mImageUsage = PL_IMAGE_USAGE_COLOR_ATTACHMENT;
		glm::vec3 mResolution = glm::vec3(1, 1, 1);
		uint8_t mMipCount = 1;
		uint16_t mLayersCount = 1;
		std::string mName = "";
		std::string mPath = "";
		bool mIsHdr = false;

		std::vector<PlImageLayout> mFutureLayouts = std::vector<PlImageLayout>();

		Texture(uint64_t descriptorCount, PlImageUsage imageUsage, PlTextureType imageType, PlViewType viewType, PlTextureFormat format, glm::vec3 resolution, uint8_t mipCount, uint16_t layersCount, const std::string& name)
			: mDescriptorCount(descriptorCount), mImageUsage(imageUsage), mTextureType(imageType), mViewType(viewType), mFormat(format), mResolution(resolution), mMipCount(mipCount), mLayersCount(layersCount), mName(name) {
		}

		bool SameAs(Texture& other) {
			return (
				this->path == other.path &&
				this->rgba == other.rgba &&
				this->type == other.type
				);
		}

		Texture() = default;

		virtual void Load(std::string relativePath = "") {}
		virtual VkDescriptorSet GetDescriptorSet() {
			return VkDescriptorSet();
		}

		Texture(std::string type) {
			this->type = type;
		}

		// Assignment operator
		Texture& operator=(const Texture& other) {
			if (this != &other) {
				type = other.type;
				path = other.path;
				rgba = other.rgba;
			}
			return *this;
		}

		bool IsTextureEmpty() const {
			return (rgba == glm::vec4(255.0f) && path.empty()) || (rgba == glm::vec4(255.0f) && mIndexHandle < 0);
		}

		virtual unsigned int GetTextureID() {
			return 0;
		}

		virtual ImTextureID GetImGuiTextureID() {
			return 0;
		}

		virtual void Destroy() {
			return;
		}

		~Texture() = default;

		template <class Archive>
		void serialize(Archive& archive) {
			archive(mAssetUuid, type, rgba);
		}
	};
}

/*
#include "Engine/Core/PreCompiledHeaders.h"
#include "Texture.h"
#include "Engine/Core/ModelLoader/ModelLoader.h"
namespace Plaza {
	void Texture::Load(std::string relativePath) {
		if (!path.empty()) {
			if (!relativePath.empty() && path.starts_with(".\\")) {
				this->id = ModelLoader::TextureFromFile(relativePath + "\\" + path);
			}
			else if (!relativePath.empty()) {
				this->id = ModelLoader::TextureFromFile(relativePath + "\\" + path);
			}
			else
				this->id = ModelLoader::TextureFromFile(path);
			this->rgba = glm::vec4(INFINITY);
		}
	}
}
*/